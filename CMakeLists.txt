cmake_minimum_required(VERSION 3.8)

project(autoTrade)


#find_package(libmongoc-static-1.0 REQUIRED)
#message(${MONGOC_STATIC_INCLUDE_DIRS})
#message(${MONGOC_STATIC_LIBRARIES})
#message(${MONGOC_STATIC_DEFINITIONS})

#find_package(libmongocxx REQUIRED)
#message(${LIBMONGOCXX_INCLUDE_DIRS})
#message(${LIBMONGOCXX_LIBRARY_DIRS})
#message(${LIBMONGOCXX_LIBRARIES})

#find_package(libbsoncxx REQUIRED)
#message(${LIBBSONCXX_INCLUDE_DIRS})
#message(${LIBBSONCXX_LIBRARY_DIRS})
#message(${LIBBSONCXX_LIBRARIES})

#include_directories(${MONGOC_STATIC_INCLUDE_DIRS})
#include_directories(${LIBMONGOCXX_INCLUDE_DIRS})
#include_directories(${LIBBSONCXX_INCLUDE_DIRS})

#link_directories(${LIBMONGOCXX_LIBRARY_DIRS})
#link_directories(${LIBBSONCXX_LIBRARY_DIRS})


add_subdirectory(src)