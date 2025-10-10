/**
 * File: Interfaces.h
 * Description: The declaration of Interfaces class for Gantt chart axis.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#ifndef QTMERMAIDWIDGET_H
#define QTMERMAIDWIDGET_H

#include <QGraphicsView>
#include <QTextDocument>
#include <QSyntaxHighlighter>

#include "ChartGraph.h"

namespace hbchart {

    /**
     * @brief The type of the chart (the valiable is used to identify the chart type)
     */
    enum class QtMermaidType : std::uint8_t {
        NONE,       ///< No chart type
        FLOW,       ///< Flowchart
        GANTT       ///< Gantt chart
    };

    /**
     * @brief The interface for the QtMermaid Core
     */
    class Core {
      public:
        /**
         * @brief The destructor of the Core interface
         * @note The Core interface is responsible for managing the chart data, layout, and rendering.
         */
        virtual ~Core() = default;

        /**
         * @brief Get the scene of the chart
         * @details The scene is responsible for managing the chart items, including nodes, edges, and labels.
         * @return The scene of the chart
         * @note The function should be overridden by the subclass to return the scene of the chart.
         */
        virtual QGraphicsScene* getScene() = 0;

        /**
         * @brief Get the view of the chart
         * @details The view is responsible for displaying the chart on the screen.
         * @return The view of the chart
         * @note The function should be overridden by the subclass to return the view of the chart.
         */
        virtual QGraphicsView* getView() = 0;

        /**
         * @brief Set the chart data
         * @details The function is responsible for setting the chart data, including nodes, edges, and labels.
         * @param graph The chart graph data
         * @note The function should be overridden by the subclass to set the chart data.
         */
        virtual void setData(const ChartGraph& graph) = 0;

        /**
         * @brief Update the chart scene
         * @details The function is responsible for updating the chart scene, including nodes, edges, and labels.
         * @note The function should be overridden by the subclass to update the chart scene.
         */
        virtual void updateScene() = 0;

        /**
         * @brief Apply the chart layout
         * @details The function is responsible for applying the chart layout, including nodes, edges, and labels.
         * @note The function should be overridden by the subclass to apply the chart layout.
         */
        virtual void applyLayout() = 0;

        /**
         * @brief Draw the chart connectors
         * @details The function is responsible for drawing the chart connectors, including nodes, edges, and labels.
         * @note The function should be overridden by the subclass to draw the chart connectors.
         */
        virtual void drawConnectors() = 0;

        /**
         * @brief Update the chart coordinate
         * @details The function is responsible for updating the chart coordinate, including nodes, edges, and labels.
         * @note The function should be overridden by the subclass to update the chart coordinate.
         */
        virtual void updateCoordinate() = 0; 
    };

    /**
     * @brief The interface for the QtMermaid Renderer
     * @note The renderer is responsible for rendering the chart graph to the view. 
     * Core subclasses would be used in the renderer to access the chart graph data.
     */
    class Renderer {
      public:
        /**
         * @brief The destructor of the Renderer interface
         */
        virtual ~Renderer() = default;

        /**
         * @brief Render the chart graph to the view
         * @details The function is responsible for rendering the chart graph to the view.
         * @param graph The chart graph data
         * @note The function should be overridden by the subclass to render the chart graph to the view.
         */
        virtual void render(const ChartGraph& graph) = 0;

        /**
         * @brief Get the view of the chart
         * @details The function is responsible for getting the view of the chart.
         * @return The view of the chart
         * @note The function should be overridden by the subclass to get the view of the chart.
         */
        virtual QGraphicsView* getView() = 0;
    };

    /**
     * @brief The interface for the QtMermaid Parser
     * @note The parser is responsible for parsing the input text to the chart graph data.
     */
    class Parser {
      public:
        /**
         * @brief The destructor of the Parser interface
         */
        virtual ~Parser() = default;

        /**
         * @brief Parse the input text to the chart graph data
         * @details The function is responsible for parsing the input text to the chart graph data.
         * @param input The input text
         * @return The chart graph data
         * @note The function should be overridden by the subclass to parse the input text to the chart graph data.
         */
        virtual ChartGraph* parse(const QString& input) = 0;
    };

    /**
     * @brief The interface for the QtMermaid Highlighter
     * @note The highlighter is responsible for highlighting the input text.
     */
    class SyntaxHighlighter : public QSyntaxHighlighter {
    public:
        /**
         * @brief The destructor of the SyntaxHighlighter interface
         */
        ~SyntaxHighlighter() override = default;

        /**
         * @brief Highlight the input text
         * @details The function is responsible for highlighting the input text.
         * @param input The input text
         * @note The function should be overridden by the subclass to highlight the input text.
         */
        virtual void setErrorLine(int line) = 0;
    
    protected: 
        // Subclasses implement highlightBlock
        using QSyntaxHighlighter::QSyntaxHighlighter; // Inherit constructors, in protected section to avoid warnings
    };
} // namespace hbchart

#endif // QTMERMAIDWIDGET_H