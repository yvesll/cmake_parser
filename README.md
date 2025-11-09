# cmake_parser

## Introduction

This is a simple c++ lib which use CMake's own lexical parser to get te AST.

At this point, the cmStandardLexer source file taken from [CMake 3.23.3](https://github.com/Kitware/CMake/releases/tag/v3.29.3).
The lib is aimed to parse a cmake file and output to a structured format like yaml or json.

## Usage

```shell
cmparser <file>
```

Take this repo's CMakeLists.txt as example, it will print following in pipe:

```yaml
- original_name: cmake_minimum_required
  line: 4
  line_end: 4
  args:
    - value: VERSION
      line: 4
    - value: 3.22.0
      line: 4
- original_name: project
  line: 6
  line_end: 6
  args:
    - value: cmparser
      line: 6
- original_name: add_library
  line: 8
  line_end: 15
  args:
    - value: cmparser
      line: 8
    - value: LexerParser/cmListFileLexer.c
      line: 9
    - value: LexerParser/cmListFileLexer.in.l
      line: 10
    - value: LexerParser/cmStandardLexer.h
      line: 11
    - value: LexerParser/EncodingC.c
      line: 12
    - value: cmParser.cpp
      line: 13
    - value: cmParser.h
      line: 14
- original_name: target_include_directories
  line: 17
  line_end: 21
  args:
    - value: cmparser
      line: 18
    - value: PUBLIC
      line: 19
    - value: ${CMAKE_CURRENT_BINARY_DIR}
      line: 20
    - value: ${CMAKE_CURRENT_SOURCE_DIR}/LexerParser
      line: 21
- original_name: add_executable
  line: 23
  line_end: 24
  args:
    - value: example
      line: 23
    - value: main.cpp
      line: 24
- original_name: target_link_libraries
  line: 26
  line_end: 26
  args:
    - value: example
      line: 26
    - value: PRIVATE
      line: 26
    - value: cmparser
      line: 26
```

## Build

Clone the project and run the following commands:

```shell
mkdir build && cd build
cmake ..
cmake --build . --target cmparser
```

You can also build the example target to try the lib

```shell
cmake --build . --target example

example ../CMakeLists.txt
```
