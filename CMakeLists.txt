cmake_minimum_required(VERSION 3.22)

project(Task_DBus)

add_subdirectory(IConfigFileManager)

add_subdirectory(IConfigStorage)

add_subdirectory(AppConfig)

add_subdirectory(DBusConfigAdapter)

add_subdirectory(DialogueServer)

add_subdirectory(DialogueClient)

add_subdirectory(JsonConfigFileManager)

add_subdirectory(ConfigurationManager)

add_subdirectory(ConfigApplication)

add_subdirectory(Tests)