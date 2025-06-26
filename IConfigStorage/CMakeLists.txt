cmake_minimum_required(VERSION 3.22)
project(IConfigStorage)

set(CMAKE_CXX_STANDARD 20)

add_library(IConfigStorage INTERFACE)

target_include_directories(IConfigStorage INTERFACE include)