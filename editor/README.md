# QtMermaidEditor

## Overview
QtMermaidEditor is a mermaid diagram editing tool based on the Qt framework. It provides an intuitive editing interface and real-time preview functionality, supporting the editing, parsing, rendering, and type detection of mermaid syntax. It aims to offer users a convenient experience for creating mermaid diagrams.


## Key Components

### 1. QtMermaidEditor Class
- **Positioning**: Core editing component, inherits from `QWidget`, integrating the editing area, preview area, and related functional modules.
- **Core Members**:
  - Editing area: `QtMermaidTextEdit* m_editor` for inputting mermaid text.
  - Preview area: `QWidget* m_preview` and `QGraphicsView* m_view` for displaying the rendered diagram.
  - Functional modules: `std::unique_ptr<Parser>` (parser), `std::unique_ptr<Renderer>` (renderer), `std::unique_ptr<SyntaxHighlighter>` (syntax highlighter), `QtMermaidTypeDetector* m_typeDetector` (type detector).
  - Interface layout: `QSplitter* m_splitter` (splitter), `QVBoxLayout* m_layout` (vertical layout), enabling split-screen display of the editing area and preview area.
- **Main Functions**:
  - Initializes the interface layout and functional modules.
  - Synchronizes editing content and preview effects through the `updateContent()` method.


### 2. QtMermaidTextEdit Class
- **Positioning**: Custom text editing control, inherits from `QPlainTextEdit`, designed specifically for mermaid syntax editing.
- **Core Features**:
  - Line number display: Real-time line number display through the `QtMermaidLineNumArea` component, supporting dynamic updates of line number width and position.
  - Event handling: Overrides `keyPressEvent` and `resizeEvent` to optimize editing interactions and window adaptation.
  - Auxiliary functions: Supports current line highlighting and synchronizes line number area updates when editing content changes.


### 3. QtMermaidLineNumArea Class
- **Positioning**: Line number display widget, inherits from `QWidget`, linked with `QtMermaidTextEdit`.
- **Functions**:
  - Dynamically calculates and draws line numbers based on the number of lines in the text editing area and font size.
  - Ensures correct size and display effect of the line number area through `sizeHint()` and `paintEvent()`.


### 4. QtMermaidTypeDetector Class
- **Positioning**: Diagram type detection tool.
- **Core Method**:
  - Static method `detectType(const QString& input)`: Analyzes the input mermaid text and returns the corresponding diagram type (`QtMermaidType`).


### 5. QtMermaidHighlighter Class
- **Positioning**: Syntax highlighter, inherits from `SyntaxHighlighter`.
- **Functions**:
  - Highlights mermaid keywords and syntax structures based on predefined regular expression rules (`HighlightingRule`) to improve readability.
  - Supports error line marking: Marks error lines through the `setErrorLine(int line)` method and highlights them with a specific format (`m_errorFormat`).


### 6. RegistryInitializer Class
- **Positioning**: Component registration initialization tool.
- **Functions**:
  - Calls the `initializeRegistries()` method to register default and extended components (such as `FlowRenderer`, `FlowHighlighter`, `FlowParser`, etc.) at program startup, ensuring that functional modules like parsers and renderers can load normally.


## Features
- **Syntax Editing**: Supports convenient input of mermaid text, with line number display and current line highlighting.
- **Syntax Highlighting**: Distinguishes mermaid syntax with colors based on regular rules to enhance readability.
- **Type Detection**: Automatically identifies the mermaid diagram type (e.g., flowchart, sequence diagram) corresponding to the input text.
- **Real-time Preview**: Synchronously displays editing content and rendered diagram effects through a splitter layout.
- **Error Prompt**: Supports marking error lines to help users locate syntax issues.


## Dependencies
- Qt framework (`QWidget`, `QTextEdit`, `QSplitter`, `QVBoxLayout`, etc.).
- Internal project components: `Interfaces.h` (interface definitions), `Factory.h` (component factory).


## License
MIT License  
Copyright (c) 2025 Barbossa Zhang