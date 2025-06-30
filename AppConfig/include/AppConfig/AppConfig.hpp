#pragma once

#include <IConfigStorage/IConfigStorage.hpp>
#include <mutex>

/**
 * @class AppConfig
 * @brief Thread-safe implementation of application configuration storage
 *
 * Implements IConfigStorage interface with mutex protection for thread safety.
 * Stores configuration parameters in memory as key-value pairs.
 */
class AppConfig : public IConfigStorage
{
   public:
    /**
     * @brief Construct a new AppConfig object
     * @param app_name Application name for this configuration
     * @param config Initial configuration parameters
     */
    AppConfig(std::string, std::map<std::string, sdbus::Variant>);

    /**
     * @brief Get all configuration parameters (thread-safe)
     * @return All of current parameters
     */
    std::map<std::string, sdbus::Variant> getAllParameters() const override;

    /**
     * @brief Set a configuration parameter (thread-safe)
     * @param key Parameter name
     * @param value New parameter value
     */
    void setParameter(const std::string&, const sdbus::Variant&) override;

    /**
     * @brief Get the application name
     * @return Application name string
     */
    [[nodiscard]] inline std::string getAppName() const override { return app_name_; }

   private:
    mutable std::mutex mutex_;
    std::string app_name_;
    std::map<std::string, sdbus::Variant> parameters_;
};
