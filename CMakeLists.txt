cmake_minimum_required(VERSION 2.8)
project(chirp)

add_library(backend backend.cc)
add_library(service service.cc)

add_executable(chirp chirp.cc)
target_link_libraries(chirp backend service)
