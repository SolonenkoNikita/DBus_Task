cmake_minimum_required(VERSION 3.22)
project(ConfigApplication)

set(CMAKE_CXX_STANDARD 20)

add_library (ConfigApplication STATIC source/ConfigApplication.cpp)

target_include_directories(ConfigApplication PUBLIC include)