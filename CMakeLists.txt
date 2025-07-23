cmake_minimum_required(VERSION 3.10)
project(dagr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define los directorios donde buscar headers
include_directories(include)

# Fuente principal
set(SOURCES
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/dag/runtime.cpp
    src/ast/ast_printer.cpp
    # src/ast/ast.cpp (si necesitás implementar funciones)
    src/dag/dag_builder.cpp
    src/dag/dag_executor.cpp
    src/dag/dag.cpp
    # src/codegen/codegen.cpp
)

add_executable(dagr_compiler ${SOURCES})
