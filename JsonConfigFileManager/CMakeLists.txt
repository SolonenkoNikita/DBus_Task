cmake_minimum_required(VERSION 3.22)
project(JsonConfigFileManager)

set(CMAKE_CXX_STANDARD 20)

add_library (JsonConfigFileManager STATIC source/JsonConfigFileManager.cpp)

target_link_libraries(JsonConfigFileManager IConfigFileManager)

target_include_directories(JsonConfigFileManager PUBLIC include)