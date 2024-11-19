project(ioctrl_test)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

SET(CMAKE_C_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-gcc")
SET(CMAKE_CXX_COMPILER "arm-rockchip830-linux-uclibcgnueabihf-g++")

add_executable(ioctrl_test ioctrl.c)
