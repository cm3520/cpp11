cmake_minimum_required(VERSION 2.8)

project(cpp)

set(CMAKE_CXX_COMPILER g++)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11 -g" ${CMAKE_CXX_FLAGS})

aux_source_directory(. SRC)

add_library(HelloWorld SHARED ${SRC})