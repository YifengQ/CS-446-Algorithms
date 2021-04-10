cmake_minimum_required(VERSION 3.13)
project(Sim02_QinYifeng)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(CMAKE_COMPILER_IS_GNUCC)

include_directories(.)

add_executable(Sim02_QinYifeng
        config.cpp
        config.h
        main.cpp
        meta.h
        meta.cpp
        PCB.cpp
        PCB.h
        simulation.cpp
        simulation.h)
