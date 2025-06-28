#pragma once

#include <memory>
#include <sdbus-c++/IObject.h>
#include <sdbus-c++/sdbus-c++.h>

#include <IConfigStorage/IConfigStorage.hpp>

static const std::string INTERFACE_NAME = "com.system.configurationManager.Application.Configuration"; 

class DBusConfigAdapter
{
public:
    using ConfigurationMap = std::map<std::string, sdbus::Variant>;

    DBusConfigAdapter(std::unique_ptr<IConfigStorage> storage, sdbus::IConnection& connection);
    ~DBusConfigAdapter() = default;

    void registerDBusInterface();

private:
    void onChangeConfiguration(const std::string& key, const sdbus::Variant& value);
    ConfigurationMap onGetConfiguration();
    void emitConfigurationChangedSignal();

    std::unique_ptr<IConfigStorage> storage_;
    std::unique_ptr<sdbus::IObject> dbusObject_;
    std::string interfaceName_ = INTERFACE_NAME;
};