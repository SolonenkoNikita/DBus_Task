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

/**
 * @class DBusConfigAdapter
 * @brief Adapts IConfigStorage to D-Bus interface
 *
 * Provides D-Bus interface for remote configuration management:
 * - Methods for getting/changing configuration
 * - Signals for configuration change notifications
*/
class DBusConfigAdapter 
{
public:
    using ConfigurationMap = std::map<std::string, sdbus::Variant>;

    /**
     * @brief Construct a new DBusConfigAdapter
     * @param storage Configuration storage implementation
     * @param connection D-Bus connection to use
    */
    DBusConfigAdapter(std::unique_ptr<IConfigStorage>, sdbus::IConnection&);

    /**
     * @brief Register D-Bus interface and methods
     *
     * Registers the following D-Bus API:
     * - ChangeConfiguration(key: string, value: variant) → void
     * - GetConfiguration() → dict<string,variant>
     * - configurationChanged(dict<string,variant>) signal
    */
    void registerDBusInterface();

private:

    /**
     * @brief Handle configuration change request
     * @param key Parameter name
     * @param value New parameter value
     * @throws sdbus::Error on failure
    */
    void onChangeConfiguration(const std::string&, const sdbus::Variant&);

    /**
     * @brief Handle configuration read request
     * @return Current configuration map
    */
    ConfigurationMap onGetConfiguration();

    /**
     * @brief Emit configuration changed signal
    */
    void emitConfigurationChangedSignal();

    std::unique_ptr<IConfigStorage> storage_;
    std::unique_ptr<sdbus::IObject> dbus_object_;
    std::string interface_name_ = INTERFACE_NAME;
};