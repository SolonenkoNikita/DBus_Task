#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include <atomic>
#include <chrono>
#include <nlohmann/json.hpp>
#include <string>
#include <thread>

static const std::string DEFAULT_CONFIG_DIR = ".config/com.system.configurationManager/";
static const std::string DEFAULT_CONFIG_FILE = "confManagerApplication1.json";
static const std::string DEFAULT_CONFIG_CONTENT = R"({
    "Timeout": 2000,
    "TimeoutPhrase": "Hello from config client"
})";
static const std::string DBUS_SERVICE = "com.system.configurationManager";
static const std::string DBUS_OBJECT = "/com/system/configurationManager/Application/confManagerApplication1";
static const std::string DBUS_INTERFACE = "com.system.configurationManager.Application.Configuration";

/**
 * @class ConfigApplication
 * @brief A client application that subscribes to D-Bus signals and periodically prints a configurable message.
 *
 * This class implements a D-Bus client application which:
 * - Connects to the D-Bus service `com.system.configurationManager`
 * - Subscribes to the signal `configurationChanged` on the object
 *   `/com/system/configurationManager/Application/confManagerApplication1`
 * - Updates internal configuration values (`Timeout`, `TimeoutPhrase`) upon receiving the signal
 * - Periodically prints the `TimeoutPhrase` every `Timeout` milliseconds
 */
class ConfigApplication
{
   public:
    /**
     * @brief Constructor. Initializes the application.
     *
     * The constructor ensures that the config file exists, loads its contents,
     * and sets up the D-Bus connection and signal subscription.
     */
    ConfigApplication();

    /**
     * @brief Starts the application's main loop.
     *
     * Runs a background thread that periodically prints the current phrase.
     * Waits for user input (Enter) to stop execution.
     */
    void run();

    /**
     * @brief Destructor. Cleans up resources.
     *
     * Stops the worker thread and releases D-Bus proxy and connection.
     */
    ~ConfigApplication();

   private:
    /**
     * @brief Loads initial configuration from a JSON file.
     *
     * Tries to load `Timeout` and `TimeoutPhrase` from the config file.
     * If the file doesn't exist or is invalid, default values are used.
     */
    void loadInitialConfig();

    /**
     * @brief Sets up D-Bus connection and signal handler.
     *
     * Establishes a session bus connection, creates a D-Bus proxy,
     * and subscribes to the `configurationChanged` signal.
     */
    void setupDBusConnection();

    /**
     * @brief Applies new configuration received via D-Bus signal.
     *
     * Updates `timeout_` and `timeout_phrase_` if corresponding keys are present.
     * @param config Map of configuration parameters as received from D-Bus.
     */
    void applyNewConfig(const std::map<std::string, sdbus::Variant>&);

    /**
     * @brief Background thread function for printing messages.
     *
     * Loops while `running_` is true, sleeping for `timeout_` milliseconds
     * and then printing `timeout_phrase_`.
     */
    void printLoop();

    /**
     * @brief Ensures that the config directory and file exist.
     *
     * Creates the config directory and writes default content if needed.
     */
    void ensureConfigFileExists();

    /**
     * @brief Expands `~` in paths to the home directory.
     * @param path Path that may contain `~` at the beginning.
     * @return Expanded path with `~` replaced by `$HOME`.
     * @throws std::runtime_error if $HOME environment variable is not set.
     */
    std::string expandPath(const std::string&) const;

    std::atomic<bool> running_{false};
    std::thread worker_thread_;
    std::unique_ptr<sdbus::IConnection> connection_;
    std::unique_ptr<sdbus::IProxy> dbus_proxy_;

    std::chrono::milliseconds timeout_{1000};
    std::string timeout_phrase_{"Default message"};
    std::string config_file_path_;
};