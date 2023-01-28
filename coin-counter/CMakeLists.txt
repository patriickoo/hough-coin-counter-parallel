cmake_minimum_required(VERSION 3.9)
project(hough_coin_counter_parallel)

set(CMAKE_CXX_STANDARD 11)
find_package(MPI REQUIRED)
set(CMAKE_C_COMPILER /opt/homebrew/bin/mpicc)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})



add_executable(hough_coin_counter_parallel main.c hcc_functions.c hcc_lib.h)
add_executable(edge_detection edge_detection.cpp)
add_executable(draw_circles draw_circles.cpp)

target_link_libraries(hough_coin_counter_parallel ${MPI_C_LIBRARIES})

#find_library(OpenCV)
find_package(OpenCV)

include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(edge_detection ${OpenCV_LIBS})
target_link_libraries(draw_circles ${OpenCV_LIBS})
