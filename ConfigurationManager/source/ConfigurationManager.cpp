#include "ConfigurationManager/ConfigurationManager.hpp"

#include <cstdlib>
#include <iostream>

namespace fs = std::filesystem;

ConfigurationManager::ConfigurationManager(std::unique_ptr<IConfigFileManager> config_loader, std::string config_dir)
    : config_loader_(std::move(config_loader)), custom_config_dir_(std::move(config_dir))
{
}

std::string ConfigurationManager::getConfigDirectoryPath() const
{
    if (!custom_config_dir_.empty())
        return custom_config_dir_;

    const char* home = std::getenv("HOME");
    if (!home)
        throw std::runtime_error("HOME environment variable not set");

    return std::string(home) + PART_OF_CONFIG_PATH;
}

bool ConfigurationManager::isValidConfigFile(const fs::path& path) const
{
    return path.extension() == ".json" || path.extension() == ".conf";
}

void ConfigurationManager::run()
{
    connection_ = sdbus::createSessionBusConnection();
    connection_->requestName(REQUEST_NAME);
    loadConfigsFromDirectory();

    std::cout << STARTED;
    connection_->enterEventLoop();
}

void ConfigurationManager::loadConfigsFromDirectory()
{
    const std::string dir_path = getConfigDirectoryPath();

    if (!fs::exists(dir_path))
    {
        std::cerr << "Config directory not found, creating: " << dir_path << '\n';
        fs::create_directories(dir_path);
        return;
    }

    for (const auto& entry : fs::directory_iterator(dir_path))
    {
        if (entry.is_regular_file() && isValidConfigFile(entry.path()))
        {
            const std::string app_name = entry.path().stem().string();

            try
            {
                auto params = config_loader_->load(entry.path().string());
                auto adapter = std::make_unique<DBusConfigAdapter>(
                    std::make_unique<AppConfig>(app_name, std::move(params)), *connection_);
                adapter->registerDBusInterface();
                adapters_.emplace_back(std::move(adapter));
            }
            catch (const std::exception& e)
            {
                std::cerr << "Error loading config " << entry.path() << ": " << e.what() << '\n';
            }
        }
    }
}