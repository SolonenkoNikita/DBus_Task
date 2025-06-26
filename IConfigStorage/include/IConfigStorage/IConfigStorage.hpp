#pragma once

#include <map>
#include <string>

#include <sdbus-c++/sdbus-c++.h>

class IConfigStorage 
{
public:
    virtual std::map<std::string, sdbus::Variant> getAllParameters() const = 0;
    
    virtual void setParameter(const std::string& key, const sdbus::Variant& value) = 0;
    
    virtual std::string getAppName() const = 0;

    virtual ~IConfigStorage() = default;
};