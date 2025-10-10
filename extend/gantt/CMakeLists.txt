cmake_minimum_required(VERSION 3.20)
project(QtMermaidGanttExtend VERSION 1.0.0 LANGUAGES CXX)

# Policy (Support for standalone builds)
cmake_policy(SET CMP0167 NEW)

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

# Create the GanttExtend library (as a shared or static library)
add_library(GanttExtend STATIC ${SOURCE_FILES} ${HEADER_FILES})
# set alias for the library
add_library(QtMermaid::GanttExtend ALIAS GanttExtend)
# Link the interface
target_link_libraries(GanttExtend PUBLIC QtMermaid::Interface)
# Link Qt libraries to the project
target_link_libraries(GanttExtend PUBLIC Qt5::Widgets Qt5::Core Qt5::Gui)
# set target properties, e.g., export name, visibility, etc.
target_include_directories(
    GanttExtend PUBLIC 
    $<INSTALL_INTERFACE:include/QtMermaid/extend/gantt>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
# Debug _d suffix
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set_target_properties(GanttExtend PROPERTIES DEBUG_POSTFIX "_d")
endif()

# Install
install(TARGETS GanttExtend
    EXPORT QtMermaidTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include/QtMermaid/extend/gantt
)

install(FILES ${HEADER_FILES} DESTINATION include/QtMermaid/extend/gantt)