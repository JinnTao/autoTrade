
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project(autoTrade LANGUAGES C CXX)

#prepare
set(CMAKE_PREFIX_PATH "src/utility/spdlog" "mongo-cxx-driver" "mongo-c-driver" "C:/boost_1_67_0/boost_1_67_0")

# Enforce the C++ standard, and disable extensions.
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror")
endif()
# NOTE: For this to work, the CMAKE_PREFIX_PATH variable must be set to point to the directory that
# was used as the argument to CMAKE_INSTALL_PREFIX when building libmongocxx.
find_package(libmongocxx REQUIRED)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    find_package(Boost 1.56.0 REQUIRED)
endif()


# spdlog package
 find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src)