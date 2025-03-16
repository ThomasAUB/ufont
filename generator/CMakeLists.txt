cmake_minimum_required(VERSION 3.5)            # CMake version check

project(# Create project
    ufont
    VERSION 0.0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)

# set(CMAKE_C_FLAGS "STB_TRUETYPE_IMPLEMENTATION" "STB_IMAGE_WRITE_IMPLEMENTATION")

set(CMAKE_C_FLAGS "-lm")

include_directories($ { CMAKE_SOURCE_DIR })

add_executable(Generator main.c)

set(CPACK_PROJECT_NAME $ { PROJECT_NAME })
set(CPACK_PROJECT_VERSION $ { PROJECT_VERSION })