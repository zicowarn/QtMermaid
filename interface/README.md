# QtMermaid Interface Module

The `interface` module is the core definition layer of the QtMermaid library, providing a set of standardized interfaces and data structures that unify the behavior specifications for different chart types. It enables flexible expansion of new chart types through interface abstraction, while ensuring consistency in the overall architecture.


## Overview

The interface module focuses on defining:
- Basic data structures for chart data storage
- Core interface specifications for chart processing (parsing, rendering, editing, etc.)
- Chart type enumeration for type identification
- Foundation for the factory pattern to create chart components


## Core Data Structures

### `ChartGraph`
The base class for all chart data structures, serving as a unified base type for different chart data. It uses polymorphism to support various specific chart data structures.

```cpp
struct ChartGraph {
    virtual ~ChartGraph() = default;
};
```

### Derived Data Structures
Specific chart types implement their own data structures by inheriting from `ChartGraph`:
- `FlowGraph`: Stores data for flowcharts, including nodes, edges, and their relationships.
  ```cpp
  struct FlowGraph : ChartGraph {
      // std::vector<Node> nodes;
      // std::vector<Edge> edges;
  };
  ```
- `GanttGraph`: Stores data for Gantt charts, including sections, tasks, and milestones.
  ```cpp
  struct GanttGraph : ChartGraph {
      // std::vector<Section> sections;
      // std::vector<Task> tasks;
      // std::vector<Milestone> milestones;
  };
  ```


## Chart Type Enumeration

`QtMermaidType` is used to identify different chart types, facilitating type-based component creation in the factory pattern:

```cpp
enum class QtMermaidType : std::uint8_t {
    NONE,       ///< No chart type
    FLOW,       ///< Flowchart
    GANTT       ///< Gantt chart
};
```


## Core Interfaces

### `Core`
The core interface for chart management, responsible for overall control of chart data, layout, and scene rendering.

Key methods:
- `getScene()`: Returns the `QGraphicsScene` for managing chart elements
- `getView()`: Returns the `QGraphicsView` for displaying the chart
- `setData(const ChartGraph& graph)`: Sets chart data
- `updateScene()`: Updates the chart scene (elements, positions, etc.)
- `applyLayout()`: Applies layout algorithms to arrange chart elements
- `drawConnectors()`: Draws connection lines between elements (e.g., edges in flowcharts)
- `updateCoordinate()`: Updates coordinate information of chart elements


### `Renderer`
The interface for chart rendering, responsible for drawing chart data onto the view.

Key methods:
- `render(const ChartGraph& graph)`: Renders the chart data to the view
- `getView()`: Returns the associated `QGraphicsView`


### `Parser`
The interface for text parsing, responsible for converting input text (with specific syntax) into `ChartGraph` data structures.

Key method:
- `parse(const QString& input)`: Parses input text and returns the corresponding `ChartGraph` object


### `SyntaxHighlighter`
Inherits from `QSyntaxHighlighter`, responsible for syntax highlighting and error marking of input text for different chart types.

Key method:
- `setErrorLine(int line)`: Marks a specific line as having an error


## Relationship Between Interfaces

The interfaces work together in a coordinated manner:
1. `Parser` converts text input to `ChartGraph` data
2. `Core` manages the data and controls the overall chart logic (layout, scene updates)
3. `Renderer` uses data from `Core` to render the chart to the view
4. `SyntaxHighlighter` enhances the editing experience by highlighting syntax in the input text


## Usage for Extension

To add a new chart type, implement these interfaces according to the following steps:
1. Define a new type in `QtMermaidType`
2. Create a data structure inheriting from `ChartGraph`
3. Implement `Core`, `Renderer`, `Parser`, and `SyntaxHighlighter` subclasses for the new type
4. Register the implementations with the factory using registration macros (e.g., `REGISTER_CORE`, `REGISTER_PARSER`)

This design ensures that new chart types can be integrated without modifying existing core logic, adhering to the open-closed principle.