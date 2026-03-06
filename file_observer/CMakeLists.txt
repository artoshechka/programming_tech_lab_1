cmake_minimum_required(VERSION 3.2)
project(file_observer LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    file_observer.hpp
    src/file_observer.cpp
)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
        logger
)