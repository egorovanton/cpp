cmake_minimum_required(VERSION 3.3)
project(ifmo_cpp_LazyString)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_library(LazyString LazyString.cpp)
install(TARGETS LazyString DESTINATION lib)
install(FILES LazyString.h DESTINATION include)