cmake_minimum_required(VERSION 3.16)
project(logger LANGUAGES CXX)

add_library(${PROJECT_NAME} STATIC
    log_levels.hpp
    log_settings.hpp
    ilogger.hpp
    logger_factory.hpp
    logger_macros.hpp
    src/app_logger.hpp
    src/app_logger.cpp
    src/log_entry_stream.hpp
    src/log_entry_stream.cpp
    src/log_settings.cpp
    src/observer_logger.hpp
    src/observer_logger.cpp
    src/thread_safe_logger.hpp
    src/thread_safe_logger.cpp
    src/logger_factory.cpp
)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC
        Qt${QT_VERSION_MAJOR}::Core
)