cmake_minimum_required(VERSION 3.8)

project(autoTrade)

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)

file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc) 
file(GLOB_RECURSE HEADER_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.h)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_FILES})

include_directories(core
                    ctp
                    datatypes
                    mathutils
                    obj
                    pautotrade
                    strategy
                    ThostTraderApi
                    utility/ta-lib/include
                    utility/easylogging	)
            
link_directories(ThostTraderApi
                 utility/ta-lib/lib
)

add_executable(autoTrade ${CPP_FILES} ${HEADER_FILES})

target_link_libraries(autoTrade
                        thostmduserapi
                        thosttraderapi
                        ta_abstract_cdd
                        ta_common_cdd
                        ta_func_cdd
                        ta_libc_cdd)