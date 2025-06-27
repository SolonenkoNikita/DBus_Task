#pragma once

#include <memory>
#include <sdbus-c++/sdbus-c++.h>

#include <IConfigStorage/IConfigStorage.hpp>

class DBusConfigAdapter
{
public:
    DBusConfigAdapter(sdbus::IConnection& connection, 
                     const std::string& objectPath,
                     std::shared_ptr<IConfigStorage> config);
    ~DBusConfigAdapter();

    void registerInterface();
    
    void ChangeConfiguration(const std::string& key, const sdbus::Variant& value);
    std::map<std::string, sdbus::Variant> GetConfiguration();

private:
    std::shared_ptr<IConfigStorage> config_;
    std::unique_ptr<sdbus::IObject> dbusObject_;
};