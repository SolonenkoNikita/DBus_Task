cmake_minimum_required(VERSION 3.22)
project(DialogueClient)

set(CMAKE_CXX_STANDARD 20)

find_package(sdbus-c++ REQUIRED)
find_package(nlohmann_json 3.9.1 REQUIRED)

if(TARGET sdbus-c++::sdbus-c++)
    set(SDBUS_TARGET sdbus-c++::sdbus-c++)
elseif(TARGET sdbus-cpp::sdbus-cpp)
    set(SDBUS_TARGET sdbus-cpp::sdbus-cpp)
else()
    find_library(SDBUS_LIB sdbus-c++)
    add_library(sdbus-c++-lib INTERFACE IMPORTED)
    set_target_properties(sdbus-c++-lib PROPERTIES
        INTERFACE_LINK_LIBRARIES "${SDBUS_LIB}"
    )
    set(SDBUS_TARGET sdbus-c++-lib)
endif()

add_executable(DialogueClient source/main.cpp)

target_link_libraries(DialogueClient 
                    ConfigApplication
                    nlohmann_json::nlohmann_json
                    ${SDBUS_TARGET}
)