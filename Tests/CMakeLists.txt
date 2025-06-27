cmake_minimum_required(VERSION 3.22)
project(Tests)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)

find_package(GTest REQUIRED)
find_package(nlohmann_json 3.9.1 REQUIRED)

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

FetchContent_MakeAvailable(googletest)

add_executable(Tests
    source/main.cpp
    source/json.cpp
)

target_link_libraries(Tests
    PRIVATE
    GTest::GTest
    GTest::Main
    JsonConfigFileManager
    nlohmann_json::nlohmann_json
    ${SDBUS_TARGET}
)