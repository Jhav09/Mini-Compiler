cmake_minimum_required(VERSION 3.10)
project(MiniLLVMCompiler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(mini_compiler
    main.cpp
    lexer.h
    parser.h
    ast.h
    codegen.h
)

llvm_map_components_to_libnames(llvm_libs
    Core
    Support
    OrcJIT
    ExecutionEngine
    Native
)

target_link_libraries(mini_compiler ${llvm_libs})
