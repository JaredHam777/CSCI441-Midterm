cmake_minimum_required(VERSION 3.15)
project(MP)



set(CMAKE_CXX_STANDARD 14)

add_executable(MP main.cpp)

include_directories("Z:/CSCI441/include")
target_link_directories(MP PUBLIC "Z:/CSCI441/lib")

target_link_libraries(MP opengl32 glfw3 glew32.dll gdi32)