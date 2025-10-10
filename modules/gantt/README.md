# QtMermaid - Gantt Chart Module

## Overview
The Gantt Chart Module is a component of QtMermaid, a Qt-based diagram generation framework. It provides functionality for creating and visualizing Gantt charts, which are used to display project schedules, task timelines, and milestones. This module handles the rendering, layout, and interaction of Gantt chart elements using Qt's graphics framework.

## Features
- Supports multiple time units for axis display:
  - Numeric values (e.g., 1.5)
  - Time units (seconds, minutes, hours)
  - Formatted time (hh:mm, hh:mm:ss, 12-hour AM/PM)
  - Date formats (2025-01-01)
  - Week-related formats (Monday, 2025-W01)
- Automatic layout of tasks, sections, and milestones
- Interactive view with zoom and pan functionality
- Configurable grid lines (primary and secondary)
- Support for hierarchical tasks (parent-child relationships)
- Visual differentiation of sections with color mapping
- Dynamic updates when model data changes
- Customizable axis labels and formatting
- Safety margins and boundary settings for chart layout

## Dependencies
- Qt 5.15.1 or higher
- C++17 or above

Ensure all dependencies are installed before building the module.

## Installation
### Run the project:
```bash
./QtMermaid
```

## Usage
### Basic Concepts
- **Tasks**: Represent activities with start and end times, support hierarchy (parent/child tasks)
- **Milestones**: Mark key points in time
- **Sections**: Group related tasks
- **Time Units**: Configure how time is displayed on the axis
- **Grid Lines**: Help visualize time intervals (primary and secondary)

### Example Structure
A typical Gantt chart configuration includes:
```cpp
// Example model setup
auto model = new GanttChartModel(TimeUnitType::SECOND);
model->setTitle("Project Timeline");

// Add sections
model->addSection("Design");
model->addSection("Development");

// Add tasks
model->addTask(nullptr, "UI Design", "Create user interface", 0, 10, true); // duration 10 units
model->addTask(nullptr, "Backend Dev", "Implement server logic", 5, 20, true);

// Add milestones
model->addMilestone(nullptr, "Design Complete", "UI design finalized", 10);
model->addMilestone(nullptr, "Launch", "Project launch", 25);
```

### Key Configurations
- Set view time unit:
  ```cpp
  scene->setViewUnit(ViewTimeUnit::TIME_HHMM); // Display axis as hh:mm
  ```

- Configure grid lines:
  ```cpp
  coordinate->setShowXAxisPrimaryGridLine(true);
  coordinate->setPrimaryGridLineCount(10);
  ```

- Adjust zoom:
  ```cpp
  view->setZoom(1.5, 1.0); // 150% width zoom
  ```

## Limitations
- Does not support custom task shapes beyond the default rounded rectangle
- Limited to top-down layout orientation
- No support for resource allocation visualization
- Basic time formatting only; complex custom date/time formats may require extension
- Milestones are displayed as simple markers with limited customization

## License
This module is part of the QtMermaid project and is licensed under the MIT License.

Qt License
This module uses the Qt framework, which is licensed under the LGPL v3 (or GPL v3, depending on your use). You must comply with the terms of the Qt license when using this software.

See the Qt license at: https://www.qt.io/terms-conditions

## Contributing
Contributions to the Gantt Chart Module are welcome! Please fork the repository and submit a pull request with your changes. For bug reports or feature requests, create an issue in the project tracker.

Initial release features:
- Basic Gantt chart rendering
- Task hierarchy support
- Multiple time unit display
- Zoom and pan interactions
- Grid line configuration

## Acknowledgments
- Mermaid for inspiration in diagram syntax and visualization concepts
- Qt framework for providing the graphics and UI foundation
- Gantt chart visualization best practices from project management standards