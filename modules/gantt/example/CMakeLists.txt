# Define the example source file
set(EXAMPLE_SOURCE_FILES
    simple_demo.cpp
)

# Add the executable for the example
add_executable(QtMermaidGanttExample ${EXAMPLE_SOURCE_FILES})

# Link library (from the src folder) and Qt libraries
target_link_libraries(QtMermaidGanttExample QtMermaid::GanttCore Qt5::Widgets Qt5::Core Qt5::Gui)

# Optionally, you can set a target installation rule for the example
if(INSTALL_DEMOS)
    install(TARGETS QtMermaidGanttExample DESTINATION bin)
endif()
