cmake_minimum_required(VERSION 3.10)

project(LoggingService)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Optional: disable compiler-specific extensions

# For MSVC, std::filesystem should be available with C++17 standard.

set(LOGGING_SERVICE_SOURCES
    LoggingService.cpp
)

set(LOGGING_SERVICE_HEADERS
    LoggingService.h
)

add_library(LoggingService STATIC
    ${LOGGING_SERVICE_SOURCES}
    ${LOGGING_SERVICE_HEADERS}
)

# Public include directories: other targets linking against LoggingService
# will automatically get this include directory.
target_include_directories(LoggingService PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include> # For installation, if you plan to install
)

# Ensure this library can be found by parent CMakeLists.txt
# No specific command needed here if add_subdirectory is used correctly in parent.
