#include "ConfigApplication/ConfigApplication.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

ConfigApplication::ConfigApplication() : config_file_path_(expandPath("~/" + DEFAULT_CONFIG_DIR + DEFAULT_CONFIG_FILE))
{
    std::cout << "Initializing ConfigApplication...\n";
    ensureConfigFileExists();
    loadInitialConfig();
    setupDBusConnection();
}

ConfigApplication::~ConfigApplication()
{
    running_ = false;
    if (worker_thread_.joinable())
        worker_thread_.join();
}

std::string ConfigApplication::expandPath(const std::string& path) const
{
    std::string result = path;
    if (result[0] == '~')
    {
        const char* home = std::getenv("HOME");
        if (!home)
            throw std::runtime_error("HOME environment variable not set");
        result.replace(0, 1, home);
    }
    return result;
}

void ConfigApplication::ensureConfigFileExists()
{
    fs::path full_path(config_file_path_);
    fs::path dir_path = full_path.parent_path();

    std::cout << "Config file location: " << full_path << '\n';

    if (!fs::exists(dir_path))
    {
        std::cout << "Creating config directory...\n";
        fs::create_directories(dir_path);
    }

    if (!fs::exists(full_path))
    {
        std::cout << "Creating default config file...\n";
        std::ofstream config_file(full_path);
        if (!config_file)
            throw std::runtime_error("Failed to create config file: " + config_file_path_);
        config_file << DEFAULT_CONFIG_CONTENT;
    }
}

void ConfigApplication::loadInitialConfig()
{
    try
    {
        std::cout << "Loading config from: " << config_file_path_ << '\n';
        std::ifstream config_file(config_file_path_);
        if (!config_file)
            throw std::runtime_error("Could not open config file");

        json config;
        config_file >> config;

        if (config.contains("Timeout"))
            timeout_ = std::chrono::milliseconds(config["Timeout"]);

        if (config.contains("TimeoutPhrase"))
            timeout_phrase_ = config["TimeoutPhrase"];

        std::cout << "Loaded config - Timeout: " << timeout_.count() << "ms, Phrase: '" << timeout_phrase_ << "'\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Warning: Could not load config - " << e.what() << "\nUsing default values\n";
    }
}

void ConfigApplication::setupDBusConnection()
{
    try
    {
        std::cout << "Setting up D-Bus connection...\n";
        connection_ = sdbus::createSessionBusConnection();

        std::cout << "Creating proxy for service: " << DBUS_SERVICE << ", object: " << DBUS_OBJECT << '\n';
        dbus_proxy_ = sdbus::createProxy(*connection_, DBUS_SERVICE, DBUS_OBJECT);

        std::cout << "Subscribing to configuration changes...\n";
        dbus_proxy_->uponSignal("configurationChanged")
            .onInterface(DBUS_INTERFACE)
            .call(
                [this](const std::map<std::string, sdbus::Variant>& config)
                {
                    std::cout << "\nReceived configuration update:\n";
                    for (const auto& [key, val] : config)
                    {
                        try
                        {
                            if (val.containsValueOfType<uint32_t>())
                                std::cout << "  " << key << " = " << val.get<uint32_t>() << '\n';
                            else if (val.containsValueOfType<std::string>())
                                std::cout << "  " << key << " = " << val.get<std::string>() << '\n';
                            else if (val.containsValueOfType<bool>())
                                std::cout << "  " << key << " = " << (val.get<bool>() ? "true" : "false") << '\n';
                            else
                                std::cout << "  " << key << " = [unprintable type]" << '\n';
                        }
                        catch (const sdbus::Error& e)
                        {
                            std::cerr << "Error printing config value: " << e.what() << '\n';
                        }
                    }
                    applyNewConfig(config);
                });

        dbus_proxy_->finishRegistration();
        std::cout << "D-Bus connection established successfully\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "D-Bus connection failed: " << e.what() << '\n';
        throw;
    }
}

void ConfigApplication::applyNewConfig(const std::map<std::string, sdbus::Variant>& config)
{
    try
    {
        bool updated = false;

        if (config.count("Timeout"))
        {
            timeout_ = std::chrono::milliseconds(config.at("Timeout").get<uint32_t>());
            std::cout << "Updated Timeout to: " << timeout_.count() << "ms\n";
            updated = true;
        }
        if (config.count("TimeoutPhrase"))
        {
            timeout_phrase_ = config.at("TimeoutPhrase").get<std::string>();
            std::cout << "Updated TimeoutPhrase to: '" << timeout_phrase_ << "'\n";
            updated = true;
        }

        if (updated)
            std::cout << "Configuration applied successfully\n";
        else
            std::cout << "No relevant configuration changes found\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error applying new config: " << e.what() << '\n';
    }
}

void ConfigApplication::printLoop()
{
    while (running_)
    {
        std::this_thread::sleep_for(timeout_);
        std::cout << timeout_phrase_ << '\n';
    }
}

void ConfigApplication::run()
{
    running_ = true;
    worker_thread_ = std::thread(&ConfigApplication::printLoop, this);

    std::thread dbus_thread([this]() { connection_->enterEventLoop(); });

    std::cout << "Client started. Press Enter to exit...\n";
    std::cin.ignore();

    running_ = false;
    connection_->leaveEventLoop();
    if (dbus_thread.joinable())
        dbus_thread.join();
}