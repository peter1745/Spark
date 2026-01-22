cmake_minimum_required(VERSION 4.0)

project(spk-interp LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_COMPILE_WARNING_AS_ERROR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(spk-interp)

target_sources(spk-interp
    PRIVATE
        main.c

        interpreter/token.c
        interpreter/lexer.c
        interpreter/expressions.c
        interpreter/printer.c
        interpreter/ast_interpreter.c
        interpreter/parser.c)

target_compile_options(spk-interp
    PRIVATE
        -std=gnu23
        -g3
        -Wall
        -Wextra
        -Wconversion
        -Wno-sign-conversion
        -pedantic
        -Werror
        -Wshadow
        #-Wno-missing-designated-field-initializers
        -Wno-deprecated-declarations
        -Wno-unused-parameter)
