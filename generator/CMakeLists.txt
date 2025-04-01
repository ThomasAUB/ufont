cmake_minimum_required(VERSION 3.24)

project(# Create project
    Generator
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_C_FLAGS "STB_TRUETYPE_IMPLEMENTATION" "STB_IMAGE_WRITE_IMPLEMENTATION")

set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-lm")

include_directories(${CMAKE_SOURCE_DIR})

set(SOURCE_FILES
    main.c
    ufnt_writer.c
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
