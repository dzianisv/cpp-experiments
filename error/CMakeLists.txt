cmake_minimum_required(VERSION 3.16)
project(test)

add_executable(test main.cpp)
set_property(TARGET test PROPERTY CXX_STANDARD 20)