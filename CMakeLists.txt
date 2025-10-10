cmake_minimum_required(VERSION 3.20)
project(QtMermaid VERSION 1.0 LANGUAGES CXX)
include(CMakePackageConfigHelpers)

# Global Options
option(INSTALL_DEMOS "Install demo executables (e.g., QtMermaidFlowExample)" ON)
option(BUILD_DOCS "Build and install Doxygen documentation" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy static analysis" OFF)

# --help-policy CMP0167
cmake_policy(SET CMP0167 NEW)

if(MSVC)
    # Or simplified to /utf-8 parameters (supported by VS2015 and above)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_AUTOMOC ON)

# Enable verbose makefile output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Enable export of compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the default build type to Debug
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

# Set the default install prefix to the binary directory
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "Default install prefix" FORCE)
endif()

# Enable clang-tidy static analysis (if requested)
if(ENABLE_CLANG_TIDY AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy enabled: ${CLANG_TIDY_EXE}")
        
        # Key modification: Add -config-file Parameters explicitly specify configuration files
        set(CLANG_TIDY_CONFIG "${CMAKE_SOURCE_DIR}/.clang-tidy")
        if(EXISTS ${CLANG_TIDY_CONFIG})
            set(CMAKE_CXX_CLANG_TIDY 
            "${CLANG_TIDY_EXE}"
            "-p=${CMAKE_BINARY_DIR}"
            "--extra-arg=-Wno-unknown-warning-option"
            )
            message(STATUS "Using clang-tidy config: ${CLANG_TIDY_CONFIG}")
        else()
            message(WARNING "clang-tidy config file not found at ${CLANG_TIDY_CONFIG}")
        endif()
    else()
        message(WARNING "clang-tidy not found, skipping analysis")
    endif()
endif()

# Qt Dependency Configuration
list(APPEND CMAKE_PREFIX_PATH ${Qt5_ROOT})
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Charts REQUIRED)
find_package(Qt5Test REQUIRED)

# Include Directories
include_directories(${Qt5_INCLUDE_DIRS})

# Subdirectories
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/interface)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/flow)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/gantt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extend/flow)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extend/gantt)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/editor)

# Install Top-Level Headers (if any)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION include/QtMermaid
        FILES_MATCHING PATTERN "*.h")

# CMakeConfig file
configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QtMermaidConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/QtMermaidConfig.cmake
    INSTALL_DESTINATION lib/cmake/QtMermaid
)

# Generate and install the QtMermaidConfigVersion.cmake file
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/QtMermaidConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# Install the CMakeConfig files
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/QtMermaidConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/QtMermaidConfigVersion.cmake
    DESTINATION lib/cmake/QtMermaid
)

# Export the targets for the top-level project
install(EXPORT QtMermaidTargets
    FILE QtMermaidTargets.cmake
    NAMESPACE QtMermaid::
    DESTINATION lib/cmake/QtMermaid
)

# Build and install documentation (if requested)
if (BUILD_DOCS)
    find_package(Doxygen OPTIONAL_COMPONENTS dot)
    if (DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/output)
        set(DOXYGEN_RECURSIVE YES)
        set(DOXYGEN_GENERATE_HTML YES)
        set(DOXYGEN_HAVE_DOT YES)
        set(DOXYGEN_CALL_GRAPH YES)
        set(DOXYGEN_CALLER_GRAPH YES)
        set(DOXYGEN_EXTRACT_ALL YES)
        doxygen_add_docs(docs
            ${CMAKE_CURRENT_SOURCE_DIR}/core/flow/include
            ${CMAKE_CURRENT_SOURCE_DIR}/core/flow/src
            ${CMAKE_CURRENT_SOURCE_DIR}/core/gantt/include
            ${CMAKE_CURRENT_SOURCE_DIR}/core/gantt/src
            ${CMAKE_CURRENT_SOURCE_DIR}/interface
            ALL
        )
    endif()
endif()