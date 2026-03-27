cmake_minimum_required(VERSION 3.16)

project(file_observer LANGUAGES CXX)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(${PROJECT_NAME} STATIC
    ifile_watcher.hpp
    file_observer.hpp
    src/file_observer.cpp
    src/polling_file_watcher.cpp
    src/file_watcher_factory.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        logger
        Qt${QT_VERSION_MAJOR}::Core
)
