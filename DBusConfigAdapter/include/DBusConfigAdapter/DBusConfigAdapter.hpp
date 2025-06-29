#pragma once

#include <memory>
#include <sdbus-c++/IObject.h>
#include <sdbus-c++/sdbus-c++.h>

#include <IConfigStorage/IConfigStorage.hpp>

static const std::string INTERFACE_NAME = "com.system.configurationManager.Application.Configuration"; 
static const std::string PATH = "/com/system/configurationManager/Application/";
static const std::string ERROR_CREATE = "Failed to create D-Bus object for path: ";
static const std::string CHANGE = "ChangeConfiguration";
static const std::string GET = "GetConfiguration";
static const std::string SIGNAL = "configurationChanged";

class DBusConfigAdapter 
{
public:
    using ConfigurationMap = std::map<std::string, sdbus::Variant>;

    DBusConfigAdapter(std::unique_ptr<IConfigStorage>, sdbus::IConnection&);

    void registerDBusInterface();

private:
    void onChangeConfiguration(const std::string&, const sdbus::Variant&);
    ConfigurationMap onGetConfiguration();
    void emitConfigurationChangedSignal();

    std::unique_ptr<IConfigStorage> storage_;
    std::unique_ptr<sdbus::IObject> dbus_object_;
    std::string interface_name_ = INTERFACE_NAME;
};