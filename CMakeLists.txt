
cmake_minimum_required(VERSION 3.1...3.28)

project(
  WWTests
  VERSION 1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory(test)
enable_testing()
