#include "JsonConfigFileManager/JsonConfigFileManager.hpp"

#include <stdexcept>

static void validateConfig(const nlohmann::json& j)
{
    if (!j.contains("Timeout") || !j["Timeout"].is_number_unsigned())
        throw std::runtime_error(MISSING_TM);

    if (!j.contains("TimeoutPhrase") || !j["TimeoutPhrase"].is_string())
        throw std::runtime_error(MISSING_TM_PHRASE);
}

sdbus::Variant JsonConfigFileManager::jsonToVariant(const nlohmann::json& j)
{
    if (j.is_number_unsigned())
        return j.get<uint32_t>();
    else if (j.is_number_integer())
        return j.get<uint32_t>();
    else if (j.is_string())
        return j.get<std::string>();
    else if (j.is_boolean())
        return j.get<bool>();
    throw std::runtime_error(UNSUPPORTED_JS);
}

nlohmann::json JsonConfigFileManager::variantToJson(const sdbus::Variant& variant)
{
    if (variant.containsValueOfType<uint32_t>())
        return variant.get<uint32_t>();
    else if (variant.containsValueOfType<std::string>())
        return variant.get<std::string>();
    else if (variant.containsValueOfType<int32_t>())
        return variant.get<int32_t>();
    else if (variant.containsValueOfType<bool>())
        return variant.get<bool>();
    throw std::runtime_error(UNSUPPORTED_VAR);
}

std::map<std::string, sdbus::Variant> JsonConfigFileManager::load(const std::string& file_path)
{
    std::ifstream file(file_path);
    if (!file.is_open())
        throw std::runtime_error(ERROR_OPEN + file_path);

    try
    {
        nlohmann::json j;
        file >> j;
        validateConfig(j);

        std::map<std::string, sdbus::Variant> config;
        for (auto& [key, value] : j.items()) config[key] = jsonToVariant(value);
        return config;
    }
    catch (const nlohmann::json::exception& e)
    {
        throw std::runtime_error("JSON error: " + std::string(e.what()));
    }
}

void JsonConfigFileManager::save(const std::string& file_path, const std::map<std::string, sdbus::Variant>& config)
{
    nlohmann::json j;
    for (const auto& [key, value] : config) j[key] = variantToJson(value);

    std::ofstream file(file_path);
    file << j.dump(4);
}