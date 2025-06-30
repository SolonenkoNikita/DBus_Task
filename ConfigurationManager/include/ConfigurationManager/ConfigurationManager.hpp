#pragma once

#include <AppConfig/AppConfig.hpp>
#include <DBusConfigAdapter/DBusConfigAdapter.hpp>
#include <IConfigFileManager/IConfigFileManager.hpp>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

static const std::string PART_OF_CONFIG_PATH = "/.config/com.system.configurationManager/";
static const std::string STARTED = "Configuration manager started successfully\n";
static const std::string REQUEST_NAME = "com.system.configurationManager";

/**
 * @class ConfigurationManager
 * @brief Main service class that manages application configurations over D-Bus
 *
 * This class is responsible for:
 * - Loading configuration files from a directory
 * - Creating D-Bus adapters for each configuration
 * - Managing the D-Bus connection and event loop
 */
class ConfigurationManager
{
   public:
    /**
     * @brief Construct a new Configuration Manager
     * @param config_loader File manager implementation for loading configs
     * @param config_dir Optional custom configuration directory path
     */
    explicit ConfigurationManager(std::unique_ptr<IConfigFileManager>, std::string = "");

    /**
     * @brief Start the configuration manager service
     *
     * This will:
     * 1. Establish D-Bus connection
     * 2. Request service name
     * 3. Load configurations
     * 4. Enter event loop
     */
    void run();

   private:
    /**
     * @brief Load all configurations from the config directory
     */
    void loadConfigsFromDirectory();

    /**
     * @brief Get the configuration directory path
     * @return Full path to configuration directory
     * @throws std::runtime_error if HOME env var is not set (when using default path)
     */
    std::string getConfigDirectoryPath() const;

    /**
     * @brief Check if file has valid configuration extension
     * @param path File path to check
     * @return true if file extension is .json or .conf (! maybe another solution)
     */
    bool isValidConfigFile(const std::filesystem::path&) const;

    std::unique_ptr<sdbus::IConnection> connection_;
    std::unique_ptr<IConfigFileManager> config_loader_;
    std::vector<std::unique_ptr<DBusConfigAdapter>> adapters_;
    std::string custom_config_dir_;
};