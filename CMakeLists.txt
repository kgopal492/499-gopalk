cmake_minimum_required(VERSION 3.0.2)
project(chirp VERSION 1.0)

find_package(glog 0.3.5 REQUIRED)
find_package(gflags REQUIRED)

add_executable(chirp commandline.cc)
target_link_libraries(chirp glog::glog)
target_link_libraries(chirp gflags::gflags)
