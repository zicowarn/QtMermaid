# interface/CMakeLists.txt

# Header files
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Interfaces.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ChartGraph.h
)

add_library(Interface INTERFACE)
add_library(QtMermaid::Interface ALIAS Interface)

# Link Qt libraries
target_link_libraries(Interface INTERFACE Qt5::Core)

# Include dirs
target_include_directories(Interface
    INTERFACE
        $<INSTALL_INTERFACE:include/QtMermaid/interface>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

# Install headers
install(FILES ${HEADER_FILES}
        DESTINATION include/QtMermaid/interface)

# Export the interface library
install(TARGETS Interface
        EXPORT QtMermaidTargets)
