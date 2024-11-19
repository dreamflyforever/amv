project(opencv-mobile-test)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-gcc")
SET(CMAKE_CXX_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-g++")

set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(opencv-mobile-camera main.cpp)

target_link_libraries(opencv-mobile-camera ${OpenCV_LIBS})
