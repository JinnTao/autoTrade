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
						ta_abstract_cdr
						ta_abstract_cmr
						ta_abstract_csd
						ta_abstract_csr
						ta_common_cdr
						ta_common_cmr
						ta_common_csd
						ta_common_csr
						ta_func_cdr
						ta_func_cmr
						ta_func_csd
						ta_func_csr
						ta_libc_cdd
						ta_libc_cdr
						ta_libc_cmr
						ta_libc_csd
						ta_libc_csr)