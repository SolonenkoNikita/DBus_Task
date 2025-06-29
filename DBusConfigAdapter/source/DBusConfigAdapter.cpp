#include "DBusConfigAdapter/DBusConfigAdapter.hpp"

#include <iostream>

DBusConfigAdapter::DBusConfigAdapter(std::unique_ptr<IConfigStorage> storage, sdbus::IConnection& connection)
            : storage_(std::move(storage))
{
    const auto object_path = PATH + storage_->getAppName();
    dbus_object_ = sdbus::createObject(connection, object_path);
    if (!dbus_object_) 
        throw std::runtime_error(ERROR_CREATE + object_path);
}

void DBusConfigAdapter::registerDBusInterface()
{
    dbus_object_->registerMethod(CHANGE).onInterface(interface_name_)
        .withInputParamNames("key", "value")
        .implementedAs([this](const std::string& key, const sdbus::Variant& value) 
        {
            this->onChangeConfiguration(key, value);
        });

    dbus_object_->registerMethod(GET).onInterface(interface_name_)
        .withOutputParamNames("configuration")
        .implementedAs([this]() -> std::map<std::string, sdbus::Variant> 
        {
            return this->onGetConfiguration();
        });

    dbus_object_->registerSignal(SIGNAL).onInterface(interface_name_)
        .withParameters<std::map<std::string, sdbus::Variant>>("configuration");

    dbus_object_->finishRegistration();
}

void DBusConfigAdapter::onChangeConfiguration(const std::string& key, const sdbus::Variant& value)
{
    try 
    {
        storage_->setParameter(key, value);
        emitConfigurationChangedSignal();
    } 
    catch (const std::exception& e) 
    {
        throw sdbus::Error("com.system.configurationManager.Error.InvalidArgs", e.what());
    }
}

DBusConfigAdapter::ConfigurationMap DBusConfigAdapter::onGetConfiguration()
{
    return storage_->getAllParameters();
}

void DBusConfigAdapter::emitConfigurationChangedSignal()
{
    auto signal = dbus_object_->createSignal(interface_name_, SIGNAL);
    signal << storage_->getAllParameters();
    dbus_object_->emitSignal(signal);
}