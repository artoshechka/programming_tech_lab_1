cmake_minimum_required(VERSION 3.16)

project(file_observer_project LANGUAGES CXX)

option(RECURSIVE_ENCODER_BUILD_TESTS "Build unit tests" OFF)

include(${CMAKE_SOURCE_DIR}/cmake/create_test.cmake)

if(RECURSIVE_ENCODER_BUILD_TESTS)
    include(CTest)
endif()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(WIN32)
    set(CMAKE_PREFIX_PATH "")
endif()

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_subdirectory(file_observer)
add_subdirectory(logger)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        file_observer
        logger
        Qt${QT_VERSION_MAJOR}::Core
)

if(RECURSIVE_ENCODER_BUILD_TESTS)
  get_property(FILE_OBSERVER_TEST_TARGETS GLOBAL PROPERTY FILE_OBSERVER_TEST_TARGETS)
  if(FILE_OBSERVER_TEST_TARGETS)
    add_custom_target(build_tests DEPENDS ${FILE_OBSERVER_TEST_TARGETS})
  else()
    add_custom_target(build_tests)
  endif()
endif()
