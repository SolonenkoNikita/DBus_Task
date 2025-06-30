#include "AppConfig/AppConfig.hpp"

AppConfig::AppConfig(std::string app_name, std::map<std::string, sdbus::Variant> config)
    : app_name_(std::move(app_name)), parameters_(std::move(config))
{
}

std::map<std::string, sdbus::Variant> AppConfig::getAllParameters() const
{
    std::lock_guard<std::mutex> lock(mutex_);
    return parameters_;
}

void AppConfig::setParameter(const std::string& key, const sdbus::Variant& value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    parameters_[key] = value;
}