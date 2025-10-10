# Define the example source file
set(EXAMPLE_SOURCE_FILES
    simple_demo2.cpp
)

# Add the executable for the example
add_executable(QtMermaidFlowExample ${EXAMPLE_SOURCE_FILES})

# Link QtMermaidFlow library (from the src folder) and Qt libraries
target_link_libraries(QtMermaidFlowExample QtMermaid::FlowCore Qt5::Widgets Qt5::Core Qt5::Gui)

# Optionally, you can set a target installation rule for the example
if(INSTALL_DEMOS)
    install(TARGETS QtMermaidFlowExample DESTINATION bin)
endif()
