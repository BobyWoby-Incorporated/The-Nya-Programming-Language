cmake_minimum_required(VERSION 3.22)
project(Nya)

set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})

add_executable(Nya main.cpp
        Parser.h
        Parser.cpp
        Lexer.cpp
        Lexer.h
        NyaUtilites.h
        AST.h)

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(Nya ${llvm_libs})