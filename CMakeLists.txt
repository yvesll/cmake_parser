cmake_minimum_required(VERSION 3.22.0)

project(cmparser)


add_library(cmparser
        LexerParser/cmListFileLexer.c
        LexerParser/cmListFileLexer.in.l
        LexerParser/cmStandardLexer.h
        LexerParser/EncodingC.c
        cmParser.cpp
        cmParser.h
)

target_include_directories(
        cmparser
        PUBLIC
        "${CMAKE_CURRENT_BINARY_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/LexerParser")

add_executable(example
        main.cpp)

target_link_libraries(example PRIVATE cmparser)
