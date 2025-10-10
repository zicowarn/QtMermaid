/**
 * File: FlowChartScene.h
 * Description: The declaration of the FlowChartScene class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWCHARTSCENE_H
#define FLOWCHARTSCENE_H

#include <QGraphicsScene>

#include "FlowChartModel.h"
#include "FlowChartShapeItem.h"

namespace hbchart {

    constexpr int DEFAULT_GRID_SIZE = 20;       ///< Default grid size
    constexpr qreal CHART_MAX_WIDTH = 600.0;    ///< Minimum chart width
    constexpr qreal CHART_MIN_HEIGHT = 400.0;   ///< Minimum chart height
    constexpr qreal CHART_MARGIN = 30.0;        ///< Scene margin
    constexpr qreal CHART_SAFETY_MARGIN = 50.0; ///< Safety margin

    /**
     * @brief Draw debug point, draw debug node information (text + small black dots)
     * @param scene The scene to draw on
     * @param center The center of the point
     * @param text The text of the point
     * @param color The color of the point (default: Qt::darkGray)
     * @note This function is used for debugging only
     */
    void drawDebugPoint(QGraphicsScene* scene, const QPointF& center, const QString& text, const QColor& color = Qt::darkGray);

    /**
     * @brief Draw debug rectangle
     * @param scene The scene to draw on
     * @param rect The rectangle to draw
     * @param color The color of the rectangle (default: Qt::darkRed)
     * @note This function is used for debugging only
     */
    void drawDebugRect(QGraphicsScene* scene, const QRectF& rect, const QColor& color = Qt::darkRed);

    /**
     * @brief Flowchart scene
     * @details Inherits from QGraphicsScene, used for drawing flowcharts
     */
    class FlowChartScene : public QGraphicsScene {
        Q_OBJECT

      private:
        FlowChartModel* m_model;                                        ///< Flowchart model
        qreal m_gridSize;                                               ///< Grid size
        QList<QColor> m_levelColors;                                    ///< Color table for different levels
        QHash<const FlowNode*, QGraphicsItem*> m_nodeRegularItems;             ///< Node items
        QHash<const FlowNode*, QGraphicsItem*> m_virtualNodeItems;             ///< Virtual items
        QHash<const FlowNode*, QGraphicsItem*> m_subgraphNodeItems;          ///< Subgraph items
        QHash<const FlowChartModel*, QGraphicsItem*> m_subgraphItems;   ///< Subgraph items
        FlowChartLabel* m_titleItem;                                    ///< Title item
        qreal m_minChartWidth;                                          ///< Minimum chart width
        qreal m_minChartHeight;                                         ///< Minimum chart height
        qreal m_chartMargin;                                            ///< Chart margin
        qreal m_saftyMargin;                                            ///< Safety margin
        qreal m_viewScale;                                              ///< View scale factor
        bool m_showControlPoints;                                       ///< Whether to show control points

        // Debugging and using
        bool m_DrawChartBoundary;                                       ///< Whether to draw chart boundary
        QRectF m_chartInnerRect;                                        ///< Chart inner rect
        QRectF m_chartOuterRect;                                        ///< Chart outer rect

      public:
        /**
         * @brief Constructor
         * @param model Flowchart model
         * @param parent Parent object
         */
        explicit FlowChartScene(FlowChartModel* model, QObject* parent = nullptr);
        ~FlowChartScene() override = default; ///< Destructor

        /**
         * @brief Get the grid size.
         * @return The grid size.
         */
        [[nodiscard]] qreal getGridSize() const;

        /**
         * @brief Set minimum chart width
         * @param width Minimum chart width
         * @note unit pixel
         */
        void setChartMinWidth(qreal width);

        /**
         * @brief Get minimum chart width
         * @return Minimum chart width
         */
        [[nodiscard]] qreal chartMinWidth() const;

        /**
         * @brief Set minimum chart height
         * @param height Minimum chart
         * @note unit pixel
         */
        void setChartMinHeight(qreal height);

        /**
         * @brief Get minimum chart height
         * @return Minimum chart height
         */
        [[nodiscard]] qreal chartMinHeight() const;

        /**
         * @brief Set chart margin
         * @param margin Chart margin
         * @note unit pixel
         */
        void setChartMargin(qreal margin);

        /**
         * @brief Get chart margin
         * @return Chart margin
         */
        [[nodiscard]] qreal chartMargin() const;

        /**
         * @brief Set chart safety margin (margin outside of outer rect)
         * @param margin Chart safety margin
         * @note unit pixel
         */
        void setSaftyMargin(qreal margin);

        /**
         * @brief Get chart safety margin (margin outside of outer rect)
         * @return Chart safety margin
         */
        [[nodiscard]] qreal getSaftyMargin() const;

        /**
         * @brief Set the level colors
         * @param colors List of colors
         */
        void setLevelColors(const QList<QColor>& colors);

        /**
         * @brief Set scale factor
         * @param scaleFactor Scale factor
         */
        void updateViewScaleFactor(qreal scaleFactor);

        /**
         * @brief Get the title item
         * @return The title item
         */
        [[nodiscard]] FlowChartLabel* getTitleItem() const;

        /**
         * @brief Set whether to draw chart boundary
         * @param draw Whether to draw chart boundary
         * @note debug use only
         */
        void setDrawChartBoundary(bool draw);

        /**
         * @brief Get chart inner rect
         * @return Chart inner rect
         * @note debug use only
         */
        [[nodiscard]] QRectF chartInnerRect() const;

        /**
         * @brief Set chart inner rect
         * @param rect Chart inner
         * @note debug use only
         */
        void setChartInnerRect(const QRectF& rect);

        /**
         * @brief Set chart outer rect
         * @param rect Chart outer rect
         * @note debug use only
         */
        void setChartOuterRect(const QRectF& rect);

        /**
         * @brief Get chart outer rect
         * @return Chart outer rect
         * @note debug use only
         */
        [[nodiscard]] QRectF chartOuterRect() const;

        /**
         * @brief Get the node item by node (exclude virtual nodes)
         * @param node The node (all types) to find
         * @return The node item, nullptr if not found
        */
        [[nodiscard]] QGraphicsItem* getNodeItem(const FlowNode* node) const;

        /**
         * @brief Get the node item by node (regular only)
         * @param node The node (regular) to find
         * @return The node item, nullptr if not found
        */
        [[nodiscard]] QGraphicsItem* getRegularNodeItem(const FlowNode* node) const;

        /**
         * @brief Get the virtual node item by node (virtual only)
         * @param node The node (virtual) to find
         * @return The node item, nullptr if not found
        */
        [[nodiscard]] QGraphicsItem* getVirtualNodeItem(const FlowNode* node) const;

        /**
         * @brief Get the subgraph node item by node (subgraph only)
         * @param node The node (subgraph) to find
         * @return The node item, nullptr if not found
        */
        [[nodiscard]] QGraphicsItem* getSubgraphNodeItem(const FlowNode* subgraph) const;

        /**
         * @brief Get the subgraph item by subgraph (subgraph only)
         * @param subgraph The subgraph to find
         * @return The subgraph item, nullptr if not found
         */
        [[nodiscard]] QGraphicsItem* getSubgraphItem(const FlowChartModel* subgraph) const;

        /**
         * @brief apply view scale factor to all items in the scene
         * @param scale Scale factor
         * @param graphCenter Center point of the graph
         */
        void applyViewScale(qreal scale, QPointF graphCenter);

        /**
         * @brief Set whether to show control points
         * @param show Whether to show control points
         */
        void setShowControlPoints(bool show);

        /**
         * @brief Get whether to show control points
         * @return Whether to show control points
         */
        [[nodiscard]] bool showControlPoints() const;

        /**
         * @brief Update the scene
         * @details This node is called by the model when the model data changes. It updates the
         * scene according to the changes.
         */
        void updateScene();

      protected:
        /**
         * @brief Draw background
         * @details Draw grid lines
         * @param painter Painter object
         * @param rect Drawing area
         * @note Overrides parent class node to draw grid lines
         */
        void drawBackground(QPainter* painter, const QRectF& rect) override;

      private:
        /**
         * @brief Add a title to the scene
         * @param title The title to add
         * @note This node is called by updateScene() to add a title to the scene
         */
        void addTitle(const FlowTitle& title);

        /**
         * @brief Add nodes to the scene recursively
         * @param model The model to add nodes from
         * @param processed A set of processed models, used to avoid infinite recursion
         * @note This node is called by updateScene() to add nodes recursively
         */
        void addNodesRecursively(const FlowChartModel* model, QSet<const FlowChartModel*>* processed = nullptr);

        /**
         * @brief Add a virtual node to the scene
         * @param section The section to add
         * @note This node is called by updateScene() to add a section to the scene
         */
        void addVirtualNode(const FlowNode* node);

        /**
         * @brief Add a node or subgraph to the scene (exclude virtual nodes)
         * @param section The section to add
         * @note This node is called by updateScene() to add a section to the scene
         */
        void addNode(const FlowNode* node);

    };
} // namespace hbchart

#endif
