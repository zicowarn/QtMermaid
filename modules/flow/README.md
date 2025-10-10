# QtMermaid - Flow Chart Module

## Overview
The Flow Chart Module is a component of QtMermaid, a Qt-based diagram generation framework. It provides functionality for creating and visualizing flowcharts, which are used to display process flows, data pipelines, and system architectures. This module handles the rendering, layout, and interaction of flowchart elements (nodes, edges, subgraphs) using Qt's graphics framework, based on a directed acyclic graph (DAG) model.


## Features
- Supports core flowchart elements:
  - Nodes (regular nodes with custom labels and shapes)
  - Edges (with labels, styles, and automatic path calculation)
  - Virtual nodes (for optimizing cross-layer edge display)
  - Subgraphs (nested hierarchical structures with independent levels)
- Flexible layout options:
  - Horizontal and vertical layout directions
  - Automatic layer calculation via topological sorting (Kahn's algorithm, DFS)
  - Optimized node spacing and layer spacing
  - Recursive subgraph layout management
- Interactive capabilities:
  - Mouse-driven zoom and pan operations
  - Dynamic view adjustment to fit the entire chart
  - Control point visibility for edge customization
- Visual customization:
  - Configurable node styles (colors, borders, fonts)
  - Edge style options (thickness, arrows, self-loop handling)
  - Title customization (text, font, color)
  - Grid background with adjustable size
- Structural management:
  - Cycle detection to maintain DAG integrity
  - Cross-subgraph edge handling
  - Layer validation and optimization
  - Automatic bounds calculation for chart elements


## Dependencies
- Qt 5.15.1 or higher (requires `QGraphics` module)
- C++17 or above


## Installation
### Run the project:
```bash
./QtMermaid
```


## Usage
### Basic Concepts
- **Nodes**: Fundamental building blocks representing steps or components (regular nodes) or auxiliary elements for layout optimization (virtual nodes).
- **Edges**: Directed connections between nodes, supporting labels, custom paths, and self-loops.
- **Subgraphs**: Nested structures for grouping related nodes, with independent levels (root level = 0).
- **Layers**: Hierarchical levels of nodes calculated via topological sorting, determining layout position.
- **Layout Direction**: Horizontal (nodes arranged left-to-right) or vertical (nodes arranged top-to-bottom).


### Example Structure
A typical flowchart configuration includes:
```cpp
// Example model setup
auto model = new FlowChartModel(nullptr);
model->setTitle("Data Processing Flow");
model->setLayoutDirection(LayoutDirection::VERTICAL);

// Add nodes
model->addNode(nullptr, NodeId("A"), NodeLabel("Input Data"));
model->addNode(nullptr, NodeId("B"), NodeLabel("Data Cleaning"));
model->addNode(nullptr, NodeId("C"), NodeLabel("Feature Extraction"));
model->addNode(nullptr, NodeId("D"), NodeLabel("Output Results"));

// Add edges
model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("B"));
model->addEdge(nullptr, SourceId("B"), nullptr, TargetId("C"));
model->addEdge(nullptr, SourceId("C"), nullptr, TargetId("D"));
model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("D")); // Cross-layer edge

// Initialize core components
auto scene = new FlowChartScene(model, nullptr);
auto view = new FlowChartView(scene, nullptr);
auto layouter = new FlowChartLayouter(scene, model);
auto connector = new FlowChartConnector(scene, model);

// Apply layout and render
scene->updateScene();
layouter->apply();
connector->updateAll();
```


### Key Configurations
- Set layout direction:
  ```cpp
  model->setLayoutDirection(LayoutDirection::HORIZONTAL); // Arrange nodes left-to-right
  ```

- Adjust spacing:
  ```cpp
  layouter->setNodeSpacing(20); // Set distance between nodes in the same layer
  ```

- Customize edge appearance:
  ```cpp
  // For a specific edge (after creation)
  auto edgeItem = scene->getEdgeItem(edge); // Hypothetical method
  edgeItem->setEdgeStyle(EdgeStyle::THICKSOLIDDOUBLEARROW);
  edgeItem->setLabel("Processes");
  ```

- Show/hide control points for edges:
  ```cpp
  scene->setShowControlPoints(true); // Enable control points for edge adjustment
  ```

- Configure title style:
  ```cpp
  auto title = model->title();
  title.font = QFont("Arial", 14, QFont::Bold);
  title.color = Qt::darkBlue;
  model->setTitle(title);
  ```


## Limitations
- Limited to directed acyclic graphs (DAGs); cyclic graphs require manual adjustment.
- Custom node shapes are restricted to predefined types (e.g., rounded rectangles) without easy extension.
- Complex nested subgraphs may cause layout performance degradation.
- Edge path optimization is basic; heavy cross-layer edges may result in visual clutter.
- Control point interaction is limited to visibility toggling, with no advanced editing tools.


## License
This module is part of the QtMermaid project and is licensed under the MIT License.

Qt License
This module uses the Qt framework, which is licensed under the LGPL v3 (or GPL v3, depending on your use). You must comply with the terms of the Qt license when using this software.

See the Qt license at: https://www.qt.io/terms-conditions


## Contributing
Contributions to the Flow Chart Module are welcome! Please fork the repository and submit a pull request with your changes. For bug reports or feature requests, create an issue in the project tracker.

Initial release features:
- Basic node and edge management
- Horizontal/vertical layout support
- Subgraph nesting
- Zoom and pan interactions
- Cycle detection


## Acknowledgments
- Mermaid for inspiration in diagram structure and visualization concepts
- Qt framework for providing the graphics and UI foundation
- Graph theory principles for topological sorting and layer calculation
- Flowchart design best practices from system engineering standards