file(GLOB_RECURSE EDITOR_HEADERS CONFIGURE_DEPENDS include/*.h)
file(GLOB_RECURSE EDITOR_SOURCES CONFIGURE_DEPENDS src/*.cpp)

set(CMAKE_AUTOMOC ON)

add_executable(QtMermaidEditor ${EDITOR_HEADERS} ${EDITOR_SOURCES})
target_include_directories(QtMermaidEditor PRIVATE include)
target_link_libraries(QtMermaidEditor PRIVATE Qt5::Widgets)
target_link_libraries(QtMermaidEditor PRIVATE QtMermaid::Interface QtMermaid::FlowExtend)
