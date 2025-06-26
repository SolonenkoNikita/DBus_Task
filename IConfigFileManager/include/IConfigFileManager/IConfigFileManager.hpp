#pragma once

#include <map>
#include <string>

#include <sdbus-c++/sdbus-c++.h>

class IConfigFileManager
{
public: 
    virtual std::map<std::string, sdbus::Variant> load(const std::string& path) = 0;

    virtual void save(const std::string& path, const std::map<std::string, sdbus::Variant>& config) = 0;

    virtual ~IConfigFileManager() = default;
};