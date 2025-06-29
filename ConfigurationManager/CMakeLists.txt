cmake_minimum_required(VERSION 3.22)
project(ConfigurationManager)

set(CMAKE_CXX_STANDARD 20)

add_library (ConfigurationManager STATIC source/ConfigurationManager.cpp)

target_link_libraries(ConfigurationManager IConfigFileManager DBusConfigAdapter AppConfig)

target_include_directories(ConfigurationManager PUBLIC include)