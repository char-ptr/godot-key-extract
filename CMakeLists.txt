cmake_minimum_required(VERSION 3.23)
project(godot_key_extract)

set(CMAKE_CXX_STANDARD 20)

add_library(godot_key_extract SHARED library.cpp ModuleWrapper.cpp ModuleWrapper.h)
