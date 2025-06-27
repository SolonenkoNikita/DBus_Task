cmake_minimum_required(VERSION 3.22)
project(DBusConfigAdapter)

set(CMAKE_CXX_STANDARD 20)

add_library (DBusConfigAdapter STATIC source/DBusConfigAdapter.cpp)

target_link_libraries(DBusConfigAdapter IConfigStorage)

target_include_directories(DBusConfigAdapter PUBLIC include)