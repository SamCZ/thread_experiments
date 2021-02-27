cmake_minimum_required(VERSION 3.17)
project(ThreadExperiments)

set(CMAKE_CXX_STANDARD 20)

add_executable(ThreadExperiments WorkerThread.hpp inline_test.hpp class_test.hpp main.cpp)