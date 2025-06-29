#include "DBusConfigAdapter/DBusConfigAdapter.hpp"
#include <iostream>

DBusConfigAdapter::DBusConfigAdapter(std::unique_ptr<IConfigStorage> storage, sdbus::IConnection& connection)
            : storage_(std::move(storage))
{
    const auto objectPath = "/com/system/configurationManager/Application/" + storage_->getAppName();
    dbusObject_ = sdbus::createObject(connection, objectPath);
    if (!dbusObject_) 
        throw std::runtime_error("Failed to create D-Bus object for path: " + objectPath);
}

void DBusConfigAdapter::registerDBusInterface()
{
    dbusObject_->registerMethod("ChangeConfiguration")
        .onInterface(interfaceName_)
        .withInputParamNames("key", "value")
        .implementedAs([this](const std::string& key, const sdbus::Variant& value) 
        {
            this->onChangeConfiguration(key, value);
        });

    dbusObject_->registerMethod("GetConfiguration")
        .onInterface(interfaceName_)
        .withOutputParamNames("configuration")
        .implementedAs([this]() -> std::map<std::string, sdbus::Variant> 
        {
            return this->onGetConfiguration();
        });

    dbusObject_->registerSignal("configurationChanged")
        .onInterface(interfaceName_)
        .withParameters<std::map<std::string, sdbus::Variant>>("configuration");

    dbusObject_->finishRegistration();
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
    auto signal = dbusObject_->createSignal(interfaceName_, "configurationChanged");
    signal << storage_->getAllParameters();
    dbusObject_->emitSignal(signal);
}