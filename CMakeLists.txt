cmake_minimum_required(VERSION 4.0)
project(Projetos_CLion)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Projetos_CLion
        main.cpp
        LoadFile.cpp
        LoadFile.h
        wordSearch.cpp
        wordSearch.h
)
