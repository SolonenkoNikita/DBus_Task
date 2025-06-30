#pragma once

#include <sdbus-c++/sdbus-c++.h>

#include <map>
#include <string>

/**
 * @class IConfigStorage
 * @brief Abstract interface for configuration storage management
 *
 * Provides methods for accessing and modifying application configuration parameters
 * stored as key-value pairs where values are D-Bus variants.
 */
class IConfigStorage
{
   public:
    /**
     * @brief Get all configuration parameters
     * @return Map of all configuration parameters (key-value pairs)
     */
    virtual std::map<std::string, sdbus::Variant> getAllParameters() const = 0;

    /**
     * @brief Set a configuration parameter
     * @param key Parameter name
     * @param value Parameter value as D-Bus variant
     */
    virtual void setParameter(const std::string& key, const sdbus::Variant& value) = 0;

    /**
     * @brief Get the application name this configuration belongs to
     * @return Application name as string
     */
    virtual std::string getAppName() const = 0;

    virtual ~IConfigStorage() = default;
};