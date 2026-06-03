cmake_minimum_required(VERSION 3.10)
project(MonteCarloOptionsPricer VERSION 1.0)

# Enforce modern C++ standards for smart pointers and concurrency
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find and link the required threading libraries for std::async
find_package(Threads REQUIRED)

# Specify the executable and its source file
add_executable(pricer main.cpp)

# Link the threading library to the executable
target_link_libraries(pricer PRIVATE Threads::Threads)