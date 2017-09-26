cmake_minimum_required(VERSION 3.8)

project(CodeLib)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)

file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

include_directories(core
                    ctp
                    datatypes
                    mathutils
                    obj
                    pautotrade
                    security
                    strategy
                    ThostTraderApi
                    utilities
                    ta-lib/include)
            
link_directories(ThostTraderApi)

add_executable(Main ${CPP_FILES} ${HEADER_FILES})

target_link_libraries(Main
                      thostmduserapi
                      thosttraderapi)
