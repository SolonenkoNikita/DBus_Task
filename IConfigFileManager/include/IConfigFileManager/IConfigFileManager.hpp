#pragma once

#include <map>
#include <string>

#include <sdbus-c++/sdbus-c++.h>

/**
 * @class IConfigFileManager
 * @brief Abstract interface for managing configuration files
 *
 * Provides methods for loading and saving configuration data in key-value
 * format, where values are stored as sdbus::Variant to support multiple data types.
*/
class IConfigFileManager
{
public: 

    /**
     * @brief Load configuration from file
     * @param path Path to the configuration file
     * @return std::map<std::string, sdbus::Variant> Key-value pairs of configuration
     * @throw std::runtime_error If file cannot be read or parsed
    */
    virtual std::map<std::string, sdbus::Variant> load(const std::string&) = 0;

    /**
     * @brief Save configuration to file
     * @param path Path to the configuration file
     * @param config Key-value pairs of configuration to save
     * @throw std::runtime_error If file cannot be written
    */
    virtual void save(const std::string&, const std::map<std::string, sdbus::Variant>&) = 0;

    /**
     * @brief Virtual destructor
    */
    virtual ~IConfigFileManager() = default;
};