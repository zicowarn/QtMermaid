cmake_minimum_required(VERSION 3.20)
project(QtMermaidFlow VERSION 1.0.0 LANGUAGES CXX)
include(CMakePackageConfigHelpers)

# Policy (Support for standalone builds)
cmake_policy(SET CMP0167 NEW)

# Option: INSTALL_DEMOS
if(NOT DEFINED INSTALL_DEMOS)
    option(INSTALL_DEMOS "Enable build and installation of example demos" OFF)
endif()

# Definition Options: Allow users to explicitly enable/disable Interface
option(BUILD_WITH_INTERFACE "Enable the option to build with interface" ON)

# Check if the Interface exists (by target name or file path)
if(BUILD_WITH_INTERFACE)
    if(NOT TARGET QtMermaid::Interface)  # Check if the Interface target exists
        # If the target does not exist, try checking the header file path (alternative plan)
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../interface/include/Interfaces.h")
            message(WARNING "Building FlowCore in standalone mode. Headers will be installed to include/qtflow.")
            set(BUILD_WITH_INTERFACE OFF CACHE BOOL "Disable With Interface" FORCE)
        endif()
    endif()
endif()

# Check Qt Dependency 
if(NOT Qt5_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Widgets Core Gui)
endif()

# Check C++ Standard and AUTOMOC
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()
if(NOT DEFINED CMAKE_AUTOMOC)
    set(CMAKE_AUTOMOC ON)
endif()

# Check Debug Options
if(NOT DEFINED CMAKE_VERBOSE_MAKEFILE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()
if(NOT DEFINED CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

# Find all header files in the include directory
file(GLOB_RECURSE HEADER_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

# Define the source files
file(GLOB_RECURSE SOURCE_FILES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

set(QTMERMAIDFLOW_INCLUDE_DIR include/QtMermaid/modules/flow)
set(QTMERMAIDFLOW_EXPORT_NAME QtMermaidTargets)
if(NOT BUILD_WITH_INTERFACE)
    set(QTMERMAIDFLOW_INCLUDE_DIR include)
    set(QTMERMAIDFLOW_EXPORT_NAME FlowCoreTargets)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/FlowCore.cpp")
        list(REMOVE_ITEM SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/FlowCore.cpp)
    endif()
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/include/FlowCore.h")
        list(REMOVE_ITEM HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/FlowCore.h)
    endif()
endif()

# Create the FlowCore library (as a shared or static library)
add_library(FlowCore STATIC ${SOURCE_FILES} ${HEADER_FILES})
# set alias for the library
add_library(QtMermaid::FlowCore ALIAS FlowCore)

# Enable try - catch for debugging
if(MSVC) 
    target_compile_options(FlowCore PRIVATE /EHsc)
else()
    target_compile_options(FlowCore PRIVATE -fexceptions)
endif()
#link the interface
if (BUILD_WITH_INTERFACE)
    target_link_libraries(FlowCore PUBLIC QtMermaid::Interface)
endif()
# link Qt libraries to the project
target_link_libraries(FlowCore PRIVATE Qt5::Widgets Qt5::Core Qt5::Gui)
# set target properties e.g., export name, visibility, etc.
target_include_directories(FlowCore
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
# Debug _d suffix
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(FlowCore PROPERTIES DEBUG_POSTFIX "_d")
endif()

install(TARGETS FlowCore
        EXPORT ${QTMERMAIDFLOW_EXPORT_NAME}
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION ${QTMERMAIDFLOW_INCLUDE_DIR}
    )

    install(FILES ${HEADER_FILES} DESTINATION ${QTMERMAIDFLOW_INCLUDE_DIR})

# Install in case standalone mode
if(NOT BUILD_WITH_INTERFACE)
    # Check if there are installation directory settings CMAKE_INSTALL_PREFIX
    if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
        message(WARNING "CMAKE_INSTALL_PREFIX is not set. use the default value: ${CMAKE_CURRENT_SOURCE_DIR}/install")
        set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install" CACHE PATH "Installation directory" FORCE)
    endif()

    message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

    set(FLOWCORE_INSTALL_CMAKE_DIR "lib/cmake")

    # Step 1: Write FlowCoreConfig.cmake.in to build temporary directory
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfig.cmake.in"
"@PACKAGE_INIT@

include(\"\${CMAKE_CURRENT_LIST_DIR}/FlowCoreTargets.cmake\")
")

    # Step 2: Write to the version file
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    # Step 3: Configure FlowCoreConfig.cmake
    configure_package_config_file(
        "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfig.cmake"
        INSTALL_DESTINATION ${FLOWCORE_INSTALL_CMAKE_DIR}
    )

    # Step 4: Install the Config file
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/FlowCoreConfigVersion.cmake"
        DESTINATION ${FLOWCORE_INSTALL_CMAKE_DIR}
    )

    # Step 5: Install targets export
    install(EXPORT FlowCoreTargets
        NAMESPACE QtMermaid::
        DESTINATION ${FLOWCORE_INSTALL_CMAKE_DIR}
    )

endif()

# add example
if(INSTALL_DEMOS OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_subdirectory(example)
endif()
