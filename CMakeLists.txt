project(utilsCpp)
set(CMAKE_CXX_STANDARD 17)

#######################################################################################
#Build target settings
#######################################################################################
add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

