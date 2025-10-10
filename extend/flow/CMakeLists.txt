cmake_minimum_required(VERSION 3.20)
project(QtMermaidFlowExtend VERSION 1.0.0 LANGUAGES CXX)

# Policy (Support for standalone builds)
cmake_policy(SET CMP0167 NEW)

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

# Create the FlowExtend library (as a shared or static library)
add_library(FlowExtend STATIC ${SOURCE_FILES} ${HEADER_FILES})
# set alias for the library
add_library(QtMermaid::FlowExtend ALIAS FlowExtend)
# Link the interface
target_link_libraries(FlowExtend PUBLIC QtMermaid::Interface)
# Link Qt libraries to the project
target_link_libraries(FlowExtend PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui)
# set target properties, e.g., export name, visibility, etc.
target_include_directories(
    FlowExtend PUBLIC 
    $<INSTALL_INTERFACE:include/QtMermaid/extend/flow>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
# Debug _d suffix
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(FlowExtend PROPERTIES DEBUG_POSTFIX "_d")
endif()

# Install
install(TARGETS FlowExtend
    EXPORT QtMermaidTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/QtMermaid/extend/flow
)

install(FILES ${HEADER_FILES} DESTINATION include/QtMermaid/extend/flow)