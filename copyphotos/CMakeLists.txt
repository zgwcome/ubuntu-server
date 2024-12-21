cmake_minimum_required(VERSION 3.5.0)
project(copyphotos VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
# LINK_DIRECTORIES(lib)

add_executable(copyphotos main.cpp copy.cpp inotify.cpp inotify-cxx.cpp)
