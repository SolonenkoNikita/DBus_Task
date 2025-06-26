cmake_minimum_required(VERSION 3.22)
project(IConfigFileManager)

set(CMAKE_CXX_STANDARD 20)

add_library (IConfigFileManager INTERFACE)

target_include_directories(IConfigFileManager INTERFACE include)