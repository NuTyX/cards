# Necessary CMake
project(flcards CXX)
cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_VERSION VERSION_LESS 3.1)
    set(CMAKE_CXX_STANDARD 11)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo
MinSizeRel."
       FORCE )
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCES *.c*)

file(GLOB_RECURSE HEADERS "*.h*")

# Add FlTk Dependency
find_package(FLTK REQUIRED)
include_directories (${FLTK_INCLUDE_DIR})

# Add Threads
find_package(Threads REQUIRED)

#add Curl
find_package(CURL REQUIRED)
include_directories (${CURL_INCLUDE_DIRS})

#add libarchive
find_package(LibArchive REQUIRED)
include_directories (${LibArchive_INCLUDE_DIRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

add_executable(flcards ${SOURCES})

target_link_libraries(flcards cards ${FLTK_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CURL_LIBRARIES} ${LibArchive_LIBRARIES})

install(TARGETS flcards DESTINATION /usr/bin)

