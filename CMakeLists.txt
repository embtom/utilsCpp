project(utilsCpp)
cmake_minimum_required(VERSION 3.10)

option (UTILSCPP_BUILD_TEST "Build tests of utilsCpp" ${UTILSCPP_BUILD_TEST})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_DEBUG_POSTFIX _g)
set(CMAKE_CXX_EXTENSIONS yes)

#######################################################################################
#Build Settings 
#######################################################################################
set(UTILSCPP_SOURCE "c++/fdSet.cpp")

set(UTILSCPP_HEADER "include/enum_reflect.hpp"
                    "include/error_msg.hpp"
                    "include/fdSet.h"
                    "include/make_map.h"
                    "include/make_unordered_map.h"
                    "include/make_vector.hpp"
                    "include/map_container.hpp"
                    "include/unique_opaque.h")

#######################################################################################
#Build target settings
#######################################################################################
add_library(${PROJECT_NAME} STATIC ${UTILSCPP_SOURCE} ${UTILSCPP_HEADER})
set_target_properties(${PROJECT_NAME} PROPERTIES 
    DEBUG_POSTFIX  ${CMAKE_DEBUG_POSTFIX}
    CXX_STANDARD   ${CMAKE_CXX_STANDARD}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

#######################################################################################
#Subdirectoris 
#######################################################################################
if (${UTILSCPP_BUILD_TEST})
    message(STATUS "Enabled utilsCpp Tests")
    add_subdirectory(test)
endif()