cmake_minimum_required(VERSION 3.22)
project(AppConfig)

set(CMAKE_CXX_STANDARD 20)

add_library (AppConfig STATIC source/AppConfig.cpp)

target_link_libraries(AppConfig IConfigStorage)

target_include_directories(AppConfig PUBLIC include)