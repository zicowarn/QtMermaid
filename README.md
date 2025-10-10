# QtMermaid

QtMermaid is an extensible chart component library based on the Qt framework, supporting parsing, rendering, and editing of various chart types. Through modular design and factory pattern, it realizes flexible expansion of chart functions, facilitating developers to quickly integrate or secondary develop.


## Project Introduction

QtMermaid is committed to providing a universal and easy-to-use chart solution, with core features including:
- **Multi-type Support**: Supports multiple basic chart types, with reserved extension interfaces in design for easy addition of new types
- **Modular Architecture**: Separates interfaces from implementations, enabling dynamic creation and management of components through factory pattern
- **Convenient Editing**: The accompanying sample editor provides syntax highlighting, real-time preview and other functions to simplify chart creation
- **Cross-platform Compatibility**: Based on the Qt framework, supporting mainstream operating systems such as Windows, Linux, and macOS


## Supported Chart Types

Currently supported chart types include:
- Flowchart: Used to display process steps, decision nodes, and flow relationships
- Gantt chart: Used for project schedule planning and time management, showing tasks, milestones, and timeline relationships


## Core Features

1. **Text Parsing**: Convert text with specific syntax into chart data structures
2. **Chart Rendering**: Realize high-quality chart drawing and display based on Qt graphics framework
3. **Syntax Highlighting**: Provide syntax highlighting and error marking for different chart types
4. **Flexible Expansion**: New chart types can be quickly added through registration mechanism without modifying core logic
5. **Sample Tool**: Provide a complete editor example to intuitively demonstrate the usage of components


## Project Structure

```
QtMermaid/
├── CMakeLists.txt          # Root directory build script for overall project compilation configuration
├── CMakePresets.json       # CMake preset configuration to simplify building in multiple environments
├── LICENSE                 # Project license file
├── cmake/
│   └── QtMermaidConfig.cmake.in  # Project installation configuration template, supporting external projects to reference via find_package
├── doc/
│   └── Doxyfile            # Doxygen configuration file for generating project API documentation
├── editor/                 # Sample editor module
│   ├── CMakeLists.txt      # Editor build script
│   ├── include/            # Editor-related header files (main window, text editing controls, etc.)
│   └── src/                # Editor implementation code
├── extend/                 # Chart extension module
│   └── [ChartType]/        # Subdirectories divided by chart type, storing parsers, highlighters, etc. for corresponding types
│       ├── include/        # Extension function header files
│       └── src/            # Extension function implementation code
├── interface/              # Core interface definition module
│   ├── CMakeLists.txt      # Interface module build script
│   └── include/            # Core interface and data structure definitions (chart data, factory interfaces, etc.)
└── modules/                # Chart core implementation module
    └── [ChartType]/        # Subdirectories divided by chart type, storing core implementations for corresponding types
        ├── CMakeLists.txt  # Module build script
        ├── README.md       # Module function description
        ├── example/        # Module usage sample code
        ├── include/        # Core implementation header files (scene, view, layout, etc.)
        └── src/            # Core implementation code
```


## Quick Start

### Dependencies
- Qt 5.15 or higher (compatible with Qt 6)
- CMake 3.16 or higher
- Compiler supporting C++17 (GCC 7+, Clang 5+, MSVC 2017+)


### Compilation Steps
1. Clone the repository and navigate to the directory
   ```bash
   git clone https://github.com/zicowarn/QtMermaid.git
   cd QtMermaid
   ```

2. Create a build directory and compile
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_PREFIX_PATH=/path/to/qt/installation
   make -j$(nproc)  # Linux/macOS
   # For Windows, you can open the generated solution with Visual Studio
   ```

3. Run the sample editor
   ```bash
   ./editor/QtMermaidEditor  # Path may vary by platform
   ```


## Extending New Chart Types

QtMermaid is designed with the factory pattern. To extend a new chart type, simply follow these steps:
1. Define the chart type enumeration (add in `QtMermaidType` in `Interfaces.h`)
2. Implement the chart data structure (inherit from `ChartGraph`)
3. Implement core interfaces (subclasses of `Core`, `Renderer`, `Parser`, `SyntaxHighlighter`)
4. Bind the type with the implementation through registration macros (`REGISTER_CORE`, `REGISTER_RENDERER`, etc.)
5. The new type can be directly created through the factory class without modifying existing logic


## Documentation

- **API Documentation**: Generate by executing `doxygen doc/Doxyfile`, then view `doc/html/index.html`
- **Module Description**: `README.md` of each chart module contains detailed functions and usage instructions
- **Sample Code**: The `example` directory of each module provides basic usage examples


## License

This project is open-source under the MIT License. For details, see the [LICENSE](LICENSE) file.


## Project Information

- Author: Barbossa Zhang
- Creation Time: 2025/04/22