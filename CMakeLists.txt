cmake_minimum_required(VERSION 3.10)
project(compilerProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(compiler 
    main.cpp
        LexicalAnalyzer/LexicalAnalyzer.cpp
        "SyntaxAnalyzer/SyntaxAnalyzer.cpp"
        "SyntaxAnalyzer/SyntaxAnalyzer.h"
        SyntaxAnalyzer/SyntaxTreeNode.h
)