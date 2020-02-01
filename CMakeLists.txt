cmake_minimum_required(VERSION 3.10)
project(utilsCpp)

######################################################
#Build Config
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX _g)
set(CMAKE_CXX_EXTENSIONS yes)

######################################################
#Build Options
option (UTILSCPP_BUILD_TEST "Build tests of utilsCpp" ON)

######################################################
#Build Sources and headers
set(UTILSCPP_SOURCE 
    "c++/fdSet.cpp"
)

set(UTILSCPP_HEADER 
    "include/enum_reflect.hpp"
    "include/error_msg.hpp"
    "include/fdSet.h"
    "include/make_map.h"
    "include/make_unordered_map.h"
    "include/make_vector.hpp"
    "include/map_container.hpp"
    "include/unique_opaque.h"
)

######################################################
#Build target
add_library(${PROJECT_NAME} STATIC ${UTILSCPP_SOURCE} ${UTILSCPP_HEADER})
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUG_POSTFIX  ${CMAKE_DEBUG_POSTFIX}
    CXX_STANDARD   ${CMAKE_CXX_STANDARD}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)

target_include_directories(${PROJECT_NAME} PUBLIC "include")

######################################################
#Subdirectoris 

include(CTest)
if (${BUILD_TESTING} AND ${UTILSCPP_BUILD_TEST})
    find_package(GTest REQUIRED)
    message(STATUS "Build with utilsCpp Unit Tests")
    enable_testing()
    add_subdirectory(tests)
endif()

