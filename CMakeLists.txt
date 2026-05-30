cmake_minimum_required(VERSION 3.10)
project(AddressBook C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# MSVC specific settings
if(MSVC)
    add_compile_options(/W4 /utf-8)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# Include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.c
    src/contact.c
    src/list.c
    src/fileio.c
    src/search.c
    src/validate.c
    src/utils.c
)

add_executable(address_book ${SOURCES})

# Copy data files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})