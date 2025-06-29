cmake_minimum_required(VERSION 3.22)
project(DialogueServer)

set(CMAKE_CXX_STANDARD 20)

find_package(sdbus-c++ REQUIRED)
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

add_executable(DialogueServer source/main.cpp)

target_link_libraries(DialogueServer 
                    AppConfig 
                    JsonConfigFileManager 
                    ConfigurationManager 
                    ${SDBUS_TARGET}
)