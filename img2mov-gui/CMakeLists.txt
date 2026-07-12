cmake_minimum_required(VERSION 3.16)

# Support both a standalone GUI build and inclusion from the root project.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    project(img2mov_gui LANGUAGES CXX)
    find_package(OpenCV REQUIRED)
    if(OpenCV_VERSION VERSION_LESS 4.0)
        message(FATAL_ERROR "img2mov-gui requires OpenCV 4 or newer")
    endif()
endif()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_executable(img2mov-gui
    main.cpp
    mainwindow.cpp
    mainwindow.h
    worker.cpp
    worker.h
    ffmpeg_write.cpp
    ffmpeg_write.h
)

target_compile_features(img2mov-gui PRIVATE cxx_std_17)
set_target_properties(img2mov-gui PROPERTIES
    CXX_EXTENSIONS OFF
    OUTPUT_NAME "img2mov-gui"
)

target_include_directories(img2mov-gui PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(img2mov-gui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    ${OpenCV_LIBS}
)

if(MSVC)
    target_compile_options(img2mov-gui PRIVATE /W4)
else()
    target_compile_options(img2mov-gui PRIVATE -Wall -Wextra -Wpedantic)
endif()

install(TARGETS img2mov-gui RUNTIME DESTINATION bin)
