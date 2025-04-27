cmake_minimum_required(VERSION 3.30)
project(tests/aizo1)

set(CMAKE_CXX_STANDARD 26)

add_executable(aizo1
        src/main.cpp
        src/Timer.cpp
        src/BoardGame.cpp
)

target_include_directories(aizo1
        PRIVATE
        include
)