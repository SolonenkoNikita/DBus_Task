#include "DBusConfigAdapter/DBusConfigAdapter.hpp"

DBusConfigAdapter::DBusConfigAdapter(sdbus::IConnection &connection,
                                     const std::string &objectPath,
                                     std::shared_ptr<IConfigStorage> config)
    : config_(std::move(config)),
      dbusObject_(sdbus::createObject(connection, objectPath)) {
  registerInterface();
}

DBusConfigAdapter::~DBusConfigAdapter() = default;

void DBusConfigAdapter::registerInterface() {
  const char *interfaceName =
      "com.system.configurationManager.Application.Configuration";

  dbusObject_->registerMethod(interfaceName, "ChangeConfiguration", "sv", "")
      .onInterface(interfaceName)
      .implementedAs(
          [this](const std::string &key, const sdbus::Variant &value) {
            this->ChangeConfiguration(key, value);
          });

  dbusObject_->registerMethod(interfaceName, "GetConfiguration", "", "a{sv}")
      .onInterface(interfaceName)
      .implementedAs([this]() { return this->GetConfiguration(); });

  dbusObject_->registerSignal(interfaceName, "configurationChanged", "a{sv}");

  dbusObject_->finishRegistration();
}

void DBusConfigAdapter::ChangeConfiguration(const std::string &key,
                                            const sdbus::Variant &value) {
  config_->setParameter(key, value);

  dbusObject_->emitSignal("configurationChanged")
      .onInterface("com.system.configurationManager.Application.Configuration")
      .withArguments(config_->getAllParameters());
}

std::map<std::string, sdbus::Variant> DBusConfigAdapter::GetConfiguration() {
  return config_->getAllParameters();
}