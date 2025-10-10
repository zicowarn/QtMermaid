/**
 * File: FlowChartLayouter.h
 * Description: The declaration of the FlowChartLayouter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWCHART_LAYOUTER_H
#define FLOWCHART_LAYOUTER_H

#include <QObject>
#include <QRectF>
#include <QVector>

#include "FlowChartModel.h"
#include "FlowChartScene.h"

namespace hbchart {

    /**
     * @brief Layer information
     */
    struct LayerInfo {
        qreal xMin = std::numeric_limits<qreal>::max();
        qreal xMax = -std::numeric_limits<qreal>::max();
        qreal yMin = std::numeric_limits<qreal>::max();
        qreal yMax = -std::numeric_limits<qreal>::max();
        qreal maxWidth = 0;
        qreal maxHeight = 0;

        [[nodiscard]] bool isValid() const { return xMin <= xMax && yMin <= yMax; }
    };

    /**
     * @brief Layer size
     */
    struct LayerSize {
      qreal regularSize = 0;           // Total size of regular nodes, height or width according to layout
      qreal virtualSize = 0;           // Total virtual node spacing, height or width according to layout
    };

    /**
     * @brief Flowchart layouter
     * @details Used for calculating the layout of flowcharts
     */
    class FlowChartLayouter : public QObject {
        Q_OBJECT

      private:
        FlowChartScene* m_scene;              ///< Flowchart scene
        FlowChartModel* m_model;              ///< Flowchart model
        QVector<LayerInfo> m_layerBounds;     ///< Layer bounds of the whole flowchart
        QVector<LayerInfo> m_tempLayerBounds; ///< Layer bounds of the current subgraph
        LayerInfo m_wholeLayerInfo;           ///< Whole layer information
        qreal m_layerPadding;                 ///< node margin
        qreal m_layerSpacing;                 ///< Layer spacing, spacing between layers
        qreal m_nodeSpacing;                  ///< node spacing

      Q_SIGNALS:
        /**
         * @brief Layout changed signal.
         */
        void eventLayoutChanged();

      public:
        /**
         * @brief Constructor.
         * @param scene Flowchart scene.
         * @param model Flowchart model.
         * @param parent Parent object.
         */
        explicit FlowChartLayouter(FlowChartScene* scene, FlowChartModel* model,
                                   QObject* parent = nullptr);
        ~FlowChartLayouter() override = default; ///< Destructor

        /**
         * @brief Set the node spacing.
         * @param spacing Node spacing.
         */
        void setNodeSpacing(qreal spacing) { m_nodeSpacing = spacing; }

        /**
         * @brief Apply the layout.
         */
        void apply();

      private:
        /**
         * @brief Collect the subgraph levels.
         * @param model Flowchart model.
         * @param level Current level.
         * @param levels Levels of subgraphs.
         * @param visited Visited subgraphs.
         * @details Collect the subgraph levels.
         */
        void collectSubgraphLevels(FlowChartModel* model, int level,
                                   QMap<int, QList<FlowChartModel*>>& levels,
                                   QSet<FlowChartModel*>& visited);

        /**
         * @brief Precompute the bounds of the flowchart subgraph.
         * @param subgraph Flowchart subgraph.
         * @param processed Processed subgraphs.
         * @details Precompute the bounds of the flowchart subgraph.
         */
        void precomputeSubgraphBounds(FlowChartModel* subgraph,
                                      QSet<FlowChartModel*>* processed = nullptr);

        /**
         * @brief Layout the flowchart subgraph.
         * @param subgraph Flowchart subgraph.
         * @param basePos Base position.
         * @param processed Processed subgraphs.
         * @details Layout the flowchart subgraph.
         */
        void layoutSubgraph(FlowChartModel* subgraph, const QPointF& basePos,
                            QSet<FlowChartModel*>* processed = nullptr);

        /**
         * @brief Adjust the position of the subgraph children.
         * @param subgraph Flowchart subgraph.
         * @param subgraphItem Subgraph item.
         */
        void adjustSubgraphChildren(FlowChartModel* subgraph, QGraphicsItem* subgraphItem);

        /**
         * @brief Compute the maximum size of the layer.
         * @param layer Layer of nodes.
         * @return Maximum size of the layer.
         */
        [[nodiscard]] QSizeF computeLayerMaxSize(const QVector<FlowNode*>& layer) const;

        /**
         * @brief Compute the total size of the layer.
         * @param layer Layer of nodes.
         * @param layerSize Layer size. (include regular node size and virtual node spacing)
         * @param isHorizontal Whether to layout in horizontal direction.
         * @return Total size of the layer.
         */
        QSizeF computeLayerTotalSize(const QVector<FlowNode*>& layer,
                                                    LayerSize& layerSize,
                                                    bool isHorizonta) const;

        /**
         * @brief Position the nodes in the layer.
         * @param layers Layers of nodes.
         * @param info Layer information.
         * @param startCoord Start coordinate.
         * @param isHorizontal Whether to layout in horizontal direction.
         * @param baseCoord Base coordinate.
         * @param saveLayerInfo Whether to save layer information.
         * @param layerIndex Layer index.
         */
        void positionNodesInLayer(const QVector<QVector<FlowNode*>>& layers, LayerInfo& info,
                                  qreal startCoord, bool isHorizontal, qreal baseCoord,
                                  bool saveLayerInfo, int layerIndex);

        /**
         * @brief apply the layout in horizontal direction.
         * @param layers Layers of nodes.
         * @param basePos Base position.
         * @param saveLayerInfo Whether to save layer information.
         */
        void applyHorizontal(const QVector<QVector<FlowNode*>>& layers, const QPointF& basePos,
                             bool saveLayerInfo = true);

        /**
         * @brief apply the layout in vertical direction.
         * @param layers Layers of nodes.
         * @param basePos Base position.
         * @param saveLayerInfo Whether to save layer information.
         */
        void applyVertical(const QVector<QVector<FlowNode*>>& layers, const QPointF& basePos,
                           bool saveLayerInfo = true);

        /**
         * @brief Compute the bounds from layers.
         * @param layerBounds Layer bounds.
         * @return Computed bounds.
         */
        [[nodiscard]] LayerInfo computeBoundsFromLayers(const QVector<LayerInfo>& layerBounds) const;

        /**
         * @brief Update the scene bounds.
         */
        void updateSceneBounds();

        /**
         * @brief Adjust the view.
         * @details Adjust the view to show the whole flowchart.
         */
        void adjustView();
    };

} // namespace hbchart

#endif // FLOWCHART_LAYOUTER_H
