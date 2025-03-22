cmake_minimum_required(VERSION 3.15)            # CMake version check

project(# Create project
    ufont
    VERSION 0.0.1
    LANGUAGES CXX
)

# set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_C_FLAGS "STB_TRUETYPE_IMPLEMENTATION" "STB_IMAGE_WRITE_IMPLEMENTATION")

# math library on linux
set(CMAKE_C_FLAGS "-lm")

include_directories(
    raster
    )



# add_executable(ufont src/main.c)
add_executable(ufont src/main.cpp)

set(CPACK_PROJECT_NAME $ { PROJECT_NAME })
set(CPACK_PROJECT_VERSION $ { PROJECT_VERSION })

# enable_testing()
# add_subdirectory(tests)
