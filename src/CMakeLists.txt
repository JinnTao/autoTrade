cmake_minimum_required(VERSION 3.8)

project(autoTrade)

#set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)

file(GLOB_RECURSE CPP_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp *.cc *.conf *.ini) 
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
                    utility/easylogging
                    utility/mongo
                    utility
                    ${LIBMONGOCXX_INCLUDE_DIRS}
                    ${Boost_INCLUDE_DIRS}
                    )
            
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
                        ta_libc_cdd
                        ${LIBMONGOCXX_LIBRARIES}
                        )
target_compile_definitions(autoTrade
  PRIVATE ${LIBMONGOCXX_DEFINITIONS}
)

# target_compile_options(ctp-marketdata-collector PRIVATE "/utf-8")
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()