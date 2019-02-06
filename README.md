Krishna Gopal
gopalk@usc.edu

registeruser

chirp
- needs to be associated with an ID
- username
- text
- parent_id

follow:
- associate each chirper with their following list
- do I need to associate each chirper with followers (whenever they make a chirp, broadcast it to people monitoring)

read:
- chirp_ID's need to be associated with replies
- each chirp in a vector, associated with each reply, vector of chirp and reply IDs

monitor:
- follower/following


cmake_minimum_required(VERSION 2.8)
project(chirp)

add_library(backend backend.cc)
add_library(service service.cc)

add_executable(chirp chirp.cc)
target_link_libraries(chirp backend service)
