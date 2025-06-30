#pragma once

#include <IConfigFileManager/IConfigFileManager.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

static const std::string MISSING_TM = "Invalid or missing Timeout (uint required)";
static const std::string MISSING_TM_PHRASE = "Invalid or missing TimeoutPhrase (string required)";
static const std::string UNSUPPORTED_JS = "Unsupported JSON type";
static const std::string UNSUPPORTED_VAR = "Unsupported variant type";
static const std::string ERROR_OPEN = "Cannot open config file: ";

/**
 * @class JsonConfigFileManager
 * @brief JSON-based implementation of IConfigFileManager
 *
 * Implements configuration file management using JSON format.
 * Supports basic data types through sdbus::Variant conversion.
 */
class JsonConfigFileManager : public IConfigFileManager
{
   public:
    /**
     * @brief Load configuration from JSON file
     * @param path Path to JSON configuration file
     * @return std::map<std::string, sdbus::Variant> Parsed configuration
     * @throw std::runtime_error If file is missing, malformed or validation fails
     *
     * Expected JSON format:
     * @code{.json}
     * {
     *     "Timeout": 1000,
     *     "TimeoutPhrase": "Default phrase"
     * }
     * @endcode
     */
    [[nodiscard]] std::map<std::string, sdbus::Variant> load(const std::string&) override;

    /**
     * @brief Save configuration to JSON file
     * @param path Destination file path
     * @param config Configuration data to save
     * @throw std::runtime_error If file cannot be created or written
     *
     * Creates pretty-printed JSON with 4-space indentation.
     */
    void save(const std::string&, const std::map<std::string, sdbus::Variant>&) override;

   private:
    /**
     * @brief Convert sdbus::Variant to nlohmann::json
     * @param variant Source variant value
     * @return nlohmann::json Converted JSON value
     * @throw std::runtime_error For unsupported variant types
     *
     * Supported variant types:
     * - uint32_t (unsigned integer)
     * - int32_t (signed integer)
     * - std::string
     * - bool
     */
    [[nodiscard]] static nlohmann::json variantToJson(const sdbus::Variant&);

    /**
     * @brief Convert nlohmann::json to sdbus::Variant
     * @param json Source JSON value
     * @return sdbus::Variant Converted variant value
     * @throw std::runtime_error For unsupported JSON types
     *
     * Supported JSON types:
     * - number (unsigned/signed)
     * - string
     * - boolean
     */
    [[nodiscard]] static sdbus::Variant jsonToVariant(const nlohmann::json&);
};