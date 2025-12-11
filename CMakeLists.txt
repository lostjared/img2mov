cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(img2mov LANGUAGES CXX)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build configuration is: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Compiler ID is: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags (removed -ansi which can conflict with C++11)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-Wall -pedantic -Ofast)
else()
    message(STATUS "Debug Mode")
    add_compile_options(-Wall -pedantic -g)
endif()

# Suppress OpenCV stitching module warnings
add_compile_options(-Wno-error=conversion)

# Source files
set(source_files 
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/img2mov.cpp
    ${CMAKE_SOURCE_DIR}/src/ffmpeg_write.cpp
)

# Find packages
find_package(OpenCV 4.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED IMPORTED_TARGET libavcodec libavformat libavutil)

# Check OpenCV version
if(${OpenCV_VERSION} LESS 3.0)
    message(FATAL_ERROR "Requires OpenCV 3 or greater")
endif()

message(STATUS "Found OpenCV version: ${OpenCV_VERSION}")

# Create executable
add_executable(img2mov ${source_files})

# Link libraries
target_link_libraries(img2mov PRIVATE 
    Threads::Threads 
    ${OpenCV_LIBS} 
    PkgConfig::FFMPEG 
    dl
)

# Include directories
target_include_directories(img2mov PRIVATE ${OpenCV_INCLUDE_DIRS})

# Install
install(TARGETS img2mov DESTINATION bin)