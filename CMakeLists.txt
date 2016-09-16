cmake_minimum_required(VERSION 3.6)
project(purehonours)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wpedantic")

include_directories(src)

set(HEADER_FILES
    src/ShoddyRepl/shoddy.cc
    src/ShoddyRepl/shoddy.h)

set(SOURCE_FILES
    src/PureHonours/purehonours.cc
    src/PureHonours/purehonours.h
    src/PureHonours/main.cc)

add_executable(purehonours ${SOURCE_FILES} ${HEADER_FILES})