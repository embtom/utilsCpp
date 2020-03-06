project(TST_ThreadLoop)

find_library(ATOMIC NAMES atomic libatomic.so.1)

######################################################
#Build Sources
set (SOURCES main.cpp)

######################################################
#Build target
add_executable(${PROJECT_NAME} ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    DEBUG_POSTFIX  ${CMAKE_DEBUG_POSTFIX}
    CXX_STANDARD   ${CMAKE_CXX_STANDARD}
    CXX_EXTENSIONS ${CMAKE_CXX_EXTENSIONS}
)
target_link_libraries(${PROJECT_NAME}  utilsCpp
                                       ${ATOMIC}
                                       GTest::GTest
                                       GTest::Main)

add_test(NAME ${PROJECT_NAME} COMMAND TST_ThreadLoop)