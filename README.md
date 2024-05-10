# cmake_parser
## Introduction
This is a simple c++ lib which use CMake's own lexical parser to get te AST.

At this point, the cmStandardLexer source file taken from [CMake 3.23.3](https://github.com/Kitware/CMake/releases/tag/v3.29.3).
The lib is aimed to parse a cmake file and output to a structured format like yaml or json.

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
Then it will print all functions in yaml format
