cmake_minimum_required(VERSION 3.15)
project(MP)



Set(CMAKE_CXX_STANDARD 17)

add_executable(MP main.cpp TrashCar.cpp TrashCar.h PistonCar.cpp PistonCar.h LightingShaderStructs.h)

include_directories("Z:/CSCI441/include")
target_link_directories(MP PUBLIC "Z:/CSCI441/lib")

target_link_libraries(MP opengl32 glfw3 glew32.dll gdi32)