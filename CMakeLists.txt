cmake_minimum_required(VERSION 3.10)
project(lisp-compiler)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify source files explicitly for better maintainability
set(SOURCES
        src/main.cpp
        src/parser.cpp
        src/ast.cpp
        src/generator.cpp
)

# Add the source directory as a definition
add_definitions(-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

# Add the executable target
add_executable(lisp-compiler ${SOURCES})

# Include project headers (modern way)
target_include_directories(lisp-compiler PRIVATE src)

# Use FetchContent to download PEGTL
include(FetchContent)
FetchContent_Declare(
        pegtl
        GIT_REPOSITORY https://github.com/taocpp/PEGTL.git
        GIT_TAG 3.2.8
)
FetchContent_MakeAvailable(pegtl)

# Link PEGTL to the target
target_link_libraries(lisp-compiler PRIVATE taocpp::pegtl)
