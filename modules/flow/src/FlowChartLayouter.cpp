/**
 * File: FlowChartLayouter.cpp
 * Description: The implementation of the FlowChartLayouter class. Sugiyama Style Hierarchical
 * Layout Algorithm Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 *
 * Note: Reference Sugiyama and Darge
 */
#include "FlowChartLayouter.h"

#include <QFontMetrics>
#include <QGraphicsItem>
#include <QtMath>

#include "FlowChartView.h" // Include DEFAULT_ZOOM_MIN, DEFAULT_ZOOM_MAX

#define FLOW_LAYOUT_DEBUG

namespace hbchart {

    constexpr qreal DEFAULT_LAYER_PADDING = 10;
    constexpr qreal DEFAULT_LAYER_SPACING = 20;
    constexpr qreal DEFAULT_NODE_SPACING = 15;

    FlowChartLayouter::FlowChartLayouter(FlowChartScene* scene, FlowChartModel* model,
                                         QObject* parent)
        : QObject(parent), m_scene(scene), m_model(model) {
        m_layerPadding = DEFAULT_LAYER_PADDING;
        m_layerSpacing = DEFAULT_LAYER_SPACING;
        m_nodeSpacing = DEFAULT_NODE_SPACING; // Node spacing
    }

    void FlowChartLayouter::apply() {
        if (m_model == nullptr || m_scene == nullptr) {
            return;
        }
        //
        m_layerBounds.clear();
        m_tempLayerBounds.clear();
        m_wholeLayerInfo = LayerInfo();

        // Collect sub-layer levels
        QMap<int, QList<FlowChartModel*>> subgraphLevels;
        QSet<FlowChartModel*> visited;
        collectSubgraphLevels(m_model, 0, subgraphLevels, visited);

        // Finish the size adjustment from bottom to top
#ifdef FLOW_LAYOUT_DEBUG
        qDebug() << "Processing subgraphs from bottom to top:";
#endif
        for (auto its = --subgraphLevels.constEnd(); its != subgraphLevels.constBegin(); --its) {
#ifdef FLOW_LAYOUT_DEBUG
            qDebug() << "Level" << its.key() << ":";
#endif
            for (auto* subgraph : its.value()) {
#ifdef FLOW_LAYOUT_DEBUG
                qDebug() << "Processing subgraph" << subgraph->modelID();
#endif
                precomputeSubgraphBounds(subgraph);
            }
        }

        // Layout root diagram
        layoutSubgraph(m_model, QPointF(0, 0));
        // Update the scene boundary (including title)
        updateSceneBounds();
        // Adjust the view
        adjustView();

        emit eventLayoutChanged();
    }

    void FlowChartLayouter::collectSubgraphLevels(FlowChartModel* model, int level,
                                                  QMap<int, QList<FlowChartModel*>>& levels,
                                                  QSet<FlowChartModel*>& visited) {
        if (model == nullptr || visited.contains(model)) {
            return;
        }
        visited.insert(model);
        levels[level].append(model);
        for (auto* node : model->nodes()) {
            if (node->type == NodeType::SUBGRAPH && node->subgraph != nullptr) {
                collectSubgraphLevels(node->subgraph, level + 1, levels, visited);
            }
        }
    }

    void FlowChartLayouter::precomputeSubgraphBounds(FlowChartModel* subgraph,
                                                     QSet<FlowChartModel*>* processed) {
        if (subgraph == nullptr) {
            return;
        }

        // Initialize processed collection
        QSet<FlowChartModel*> localProcessed;
        if (processed == nullptr) {
            processed = &localProcessed;
        }

        // Prevent repeated processing
        if (processed->contains(subgraph)) {
#ifdef FLOW_LAYOUT_DEBUG
            qDebug() << "Skipping already processed subgraph in precompute:" << subgraph->modelID();
#endif
            return;
        }
        processed->insert(subgraph);

        // Get the node level (simplified assumption: single layer)
        auto layers = subgraph->calculateLayers();
        if (layers.isEmpty()) {
            qDebug() << "No layers in subgraph" << subgraph->modelID();
            return;
        }

        // Temporary layout to generate node scene coordinates
        m_tempLayerBounds.clear();
        const QPointF basePos(0, 0);
        if (subgraph->effectiveLayoutDirection() == LayoutDirection::HORIZONTAL) {
            applyHorizontal(layers, basePos, false); // No hierarchical information is saved
        } else {
            applyVertical(layers, basePos, false);
        }

        // Calculate the boundaries of subgraphs
        auto* subgraphItem = m_scene->getSubgraphItem(subgraph);
        if (subgraphItem != nullptr) {
            auto* rectItem = dynamic_cast<SubgraphShapeItemRect*>(subgraphItem);
            if (rectItem != nullptr) {
                LayerInfo bounds = computeBoundsFromLayers(m_tempLayerBounds);
                QRectF rect(bounds.xMin - m_layerPadding, bounds.yMin - m_layerPadding,
                            bounds.maxWidth, bounds.maxHeight);
#ifdef FLOW_LAYOUT_DEBUG
                qDebug() << "Subgraph" << subgraph->modelID() << "Rect:" << rect;
#endif
                rectItem->setSize(rect.size());
                // Reset subgraph position to (0, 0)
                rectItem->setPos(0, 0); // Reset sub-picture
#ifdef FLOW_LAYOUT_DEBUG
                qDebug() << "Subgraph" << subgraph->modelID()
                         << "InnRect:" << rectItem->innerRect();
#endif
            }
        }
    }

    void FlowChartLayouter::layoutSubgraph(FlowChartModel* subgraph, const QPointF& basePos, // NOLINT(readability-function-cognitive-complexity)
                                           QSet<FlowChartModel*>* processed) {
        //
        if (subgraph == nullptr) {
            return;
        }

        // Initialize processed collection
        QSet<FlowChartModel*> localProcessed;
        if (processed == nullptr) {
            processed = &localProcessed;
        }

        // Prevent repeated processing
        if (processed->contains(subgraph)) {
#ifdef FLOW_LAYOUT_DEBUG
            qDebug() << "Skipping already processed subgraph in precompute:" << subgraph->modelID();
#endif
            return;
        }
        processed->insert(subgraph);
#ifdef FLOW_LAYOUT_DEBUG
        qDebug() << "Layout subgraph, modelID:" << subgraph->modelID()
                 << "nodes count:" << subgraph->nodes().size() << "base pos:" << basePos;
        QString name = subgraph->modelID() + " BasePos";
        drawDebugPoint(m_scene, basePos, name, Qt::red);
#endif
        // Main layout
        auto layers = subgraph->calculateLayers();
        if (layers.isEmpty()) {
            qWarning() << "No layers in subgraph" << subgraph->modelID();
            return;
        }
#ifdef FLOW_LAYOUT_DEBUG
        qDebug() << "Layers in subgraph" << subgraph->modelID() << ":" << layers.size();
        for (int i = 0; i < layers.size(); ++i) {
            QStringList nodeIds;
            for (auto* node : layers[i]) {
                nodeIds << node->id;
            }
            qDebug() << "Layer" << i << ":" << nodeIds;
        }
#endif
        m_layerBounds.resize(layers.size());
        m_tempLayerBounds.resize(layers.size());

        if (subgraph->effectiveLayoutDirection() == LayoutDirection::HORIZONTAL) {
            // basePos.x() + m_layerPadding
            QPointF pos(basePos.x() + m_layerPadding, basePos.y());
            applyHorizontal(layers, pos, true);
        } else {
            // basePos.y() + m_layerPadding
            QPointF pos(basePos.x(), basePos.y() + m_layerPadding);
            applyVertical(layers, pos, true);
        }

        //
        auto* subgraphItem = m_scene->getSubgraphItem(subgraph);
        if (subgraphItem != nullptr) {
            adjustSubgraphChildren(subgraph, subgraphItem);
        }

        // Recursively process nested subgraphs
        for (auto* node : subgraph->nodes()) {
            if (node->type == NodeType::SUBGRAPH && node->subgraph != nullptr) {
                qDebug() << "Processing nested subgraph:" << node->subgraph->modelID();
                auto* nodeItem = m_scene->getSubgraphItem(node->subgraph);
                qDebug() << "Node currently pos:" << nodeItem->pos();
                QPointF basePos = nodeItem->pos();
                if (node->subgraph->effectiveLayoutDirection() == LayoutDirection::HORIZONTAL) {
                    if (nodeItem != nullptr) {
                        auto* rectItem = dynamic_cast<SubgraphShapeItemRect*>(nodeItem);
                        if (rectItem != nullptr) {
                            QSizeF size = rectItem->size();
                            qDebug() << "Nested subgraph size:" << size;
                            basePos = QPointF(basePos.x() - (size.width() / 2), basePos.y());
                            qDebug() << "Nested subgraph base pos:" << basePos;
                        }
                    }
                } else {
                    if (nodeItem != nullptr) {
                        auto* rectItem = dynamic_cast<SubgraphShapeItemRect*>(nodeItem);
                        if (rectItem != nullptr) {
                            QSizeF size = rectItem->size();
                            qDebug() << "Nested subgraph size:" << size;
                            basePos = QPointF(basePos.x(), basePos.y() - (size.height() / 2));
                            qDebug() << "Nested subgraph base pos:" << basePos;
                        }
                    }
                }
                layoutSubgraph(node->subgraph, basePos, processed);
            }
        }
    }

    void FlowChartLayouter::adjustSubgraphChildren(FlowChartModel* subgraph,
                                                   QGraphicsItem* subgraphItem) {
        if (subgraph == nullptr || subgraphItem == nullptr) {
            return;
        }

        QPointF subgraphPos = subgraphItem->pos();
        for (auto* node : subgraph->nodes()) {
            QGraphicsItem* item = nullptr;
            if (node->type == NodeType::REGULAR) {
                item = m_scene->getRegularNodeItem(node);
            } else if (node->type == NodeType::SUBGRAPH) {
                item = m_scene->getSubgraphNodeItem(node);
            }
            if (item != nullptr) {
                // Get the current scene coordinates of the node
                QPointF scenePos = item->pos();
                // Calculate the relative coordinates relative to subgraphItem
                QPointF relativePos = scenePos - subgraphPos;
                // Set up a father-son relationship
                item->setParentItem(subgraphItem);
                // Update node position as relative coordinates
                item->setPos(relativePos);
                qDebug() << "Node" << node->id << "parent:" << subgraph->modelID();
            }
        }
    }

    QSizeF FlowChartLayouter::computeLayerMaxSize(const QVector<FlowNode*>& layer) const {
        qreal maxWidth = 0;
        qreal maxHeight = 0;
        for (auto* node : layer) {
            if (node->type != NodeType::VIRTUAL) { // Regular nodes only
                if (auto* item = m_scene->getNodeItem(node)) {
                    maxWidth = qMax(maxWidth, item->boundingRect().width());
                    maxHeight = qMax(maxHeight, item->boundingRect().height());
                }
            }
        }
        return {maxWidth, maxHeight};
    }

    //Compute the total layer size (separate regular nodes and virtual nodes)
    QSizeF FlowChartLayouter::computeLayerTotalSize(const QVector<FlowNode*>& layer,
                                                    LayerSize& layerSize,
                                                    bool isHorizontal) const {
        qreal totalRegular = 0; // Total size of regular nodes
        qreal totalVirtual = 0; // Total virtual node spacing
        qreal usedSpacing = 0;
        int regularCount = 0;
        int virtualCount = 0;

        for (auto* node : layer) {
            if (auto* item = m_scene->getNodeItem(node)) {
                if (node->type == NodeType::VIRTUAL) {
                    totalVirtual += m_nodeSpacing; // Virtual nodes only contribute spacing
                    virtualCount++;
                } else {
                    totalRegular +=
                        isHorizontal ? item->boundingRect().height() : item->boundingRect().width();
                    totalRegular += m_nodeSpacing; // Regular node contribution size and spacing
                    regularCount++;
                }
            }
        }

        // Subtract the last spacing
        if (regularCount > 0) {
            totalRegular -= m_nodeSpacing;
        }
        if (virtualCount > 0) {
            totalVirtual -= m_nodeSpacing; // Optional: Virtual nodes may not require end spacing
        }

        layerSize.regularSize = totalRegular;
        layerSize.virtualSize = totalVirtual;

        // Total size = regular node + virtual node
        qreal totalSize = totalRegular + totalVirtual;
        return {isHorizontal ? totalRegular : totalSize,
                      isHorizontal ? totalSize : totalRegular};
    }

    // Positioning nodes in the layer (regular nodes first, virtual nodes behind)
    void FlowChartLayouter::positionNodesInLayer(const QVector<QVector<FlowNode*>>& layers, LayerInfo& info,  // NOLINT(readability-function-cognitive-complexity)
                                                 qreal startCoord, bool isHorizontal, qreal baseCoord, bool saveLayerInfo,
                                                 int layerIndex) {
        qreal regularCoord = startCoord; // Regular node start coordinates
        [[maybe_unused]] qreal lastCoord = startCoord;    // Used for debugging spacing
        LayerSize layerSize{0, 0}; // Regular node size and virtual node spacing

        // First calculate the size of the regular node and the virtual node
        computeLayerTotalSize(layers[layerIndex], layerSize, isHorizontal);

        //Collect the secondary axis coordinates of the previous layer of virtual nodes (horizontal: Y, vertical: X)
        QMap<QString, qreal> prevVirtualPos; // pathId -> Secondary axis coordinates
        if (layerIndex > 0 && saveLayerInfo && !m_layerBounds.isEmpty()) {
            const QVector<FlowNode*>& prevLayer = layers[layerIndex - 1];
            for (auto* node : prevLayer) {
                if (node->type == NodeType::VIRTUAL) {
                    if (auto* item = m_scene->getNodeItem(node)) {
                        prevVirtualPos[node->pathId] = isHorizontal ? item->pos().y() : item->pos().x();
                    }
                }
            }
        }

        // First time: locate regular nodes
        for (auto* node : layers[layerIndex]) {
            if (node->type != NodeType::VIRTUAL) {
                auto* item = m_scene->getNodeItem(node);
                if (item != nullptr) {
                    qreal size =
                        isHorizontal ? item->boundingRect().height() : item->boundingRect().width();
                    qreal centerMain =
                        isHorizontal ? (baseCoord + (info.maxWidth / 2)) : (regularCoord + (size / 2));
                    qreal centerCross =
                        isHorizontal ? (regularCoord + (size / 2)) : (baseCoord + (info.maxHeight / 2));
                    QPointF pos(centerMain, centerCross);

#ifdef FLOW_LAYOUT_DEBUG
                    if (saveLayerInfo) {
                        qDebug() << "Regular Node" << node->id << "pos: (" << pos.x() << ","
                                 << pos.y() << ") spacing:" << (regularCoord - lastCoord);
                        drawDebugPoint(m_scene, pos, node->id, Qt::red);
                    }
#endif

                    item->setPos(pos);

                    // Update the boundaries
                    QRectF rect = item->boundingRect();
                    rect.moveCenter(pos);
                    info.xMin = qMin(info.xMin, rect.left());
                    info.xMax = qMax(info.xMax, rect.right());
                    info.yMin = qMin(info.yMin, rect.top());
                    info.yMax = qMax(info.yMax, rect.bottom());

                    lastCoord = regularCoord;
                    regularCoord += size + m_nodeSpacing;
                }
            }
        }

        // Second pass: Position the virtual node (prioritize the previous layer)
        qreal virtualCoord = startCoord + layerSize.regularSize; // Default start position
        lastCoord = virtualCoord;
        for (auto* node : layers[layerIndex]) {
            if (node->type == NodeType::VIRTUAL) {
                auto* item = m_scene->getNodeItem(node);
                if (item != nullptr) {
                    qreal size = 0; // The virtual node size is 0
                    qreal centerMain;
                    qreal centerCross;
                    if (prevVirtualPos.contains(node->pathId)) {
                        // Use the secondary axis coordinates of the corresponding virtual nodes on the previous layer
                        if (isHorizontal) {
                            centerMain = baseCoord + info.maxWidth / 2;
                            centerCross = prevVirtualPos[node->pathId]; // Y Coordinate alignment
                        } else {
                            centerMain = prevVirtualPos[node->pathId]; // X Coordinate alignment
                            centerCross = baseCoord + info.maxHeight / 2;
                        }
                    } else {
                        // Default order
                        centerMain = isHorizontal ? (baseCoord + (info.maxWidth / 2)) : virtualCoord;
                        centerCross = isHorizontal ? virtualCoord : (baseCoord + (info.maxHeight / 2));
                    }
                    QPointF pos(centerMain, centerCross);

#ifdef FLOW_LAYOUT_DEBUG
                    if (saveLayerInfo) {
                        qDebug() << "Virtual Node" << node->id << "pos: (" << pos.x() << ","
                                 << pos.y() << ") spacing:" << (virtualCoord - lastCoord)
                                 << "pathId:" << node->pathId
                                 << "aligned:" << prevVirtualPos.contains(node->pathId);
                        drawDebugPoint(m_scene, pos, node->id, Qt::blue);
                        drawDebugRect(m_scene, QRectF(pos.x() - 2, pos.y() - 2, 4, 4));
                    }
#endif

                    item->setPos(pos);

                    // Update the boundaries
                    QRectF rect = item->boundingRect();
                    rect.moveCenter(pos);
                    info.xMin = qMin(info.xMin, rect.left());
                    info.xMax = qMax(info.xMax, rect.right());
                    info.yMin = qMin(info.yMin, rect.top());
                    info.yMax = qMax(info.yMax, rect.bottom());

                    //Update maxHeight (horizontal layout) or maxWidth (vertical layout)
                    if (isHorizontal) {
                        info.maxHeight = qMax(info.maxHeight, centerCross + m_nodeSpacing);
                    } else {
                        info.maxWidth = qMax(info.maxWidth, centerMain + m_nodeSpacing);
                    }

                    lastCoord = virtualCoord;
                    virtualCoord += m_nodeSpacing; // Keep the default increment
                }
            }
        }
    }

    void FlowChartLayouter::applyHorizontal(const QVector<QVector<FlowNode*>>& layers,
                                            const QPointF& basePos, bool saveLayerInfo) {
        qreal startx = basePos.x();
        for (int idx = 0; idx < layers.size(); ++idx) {
            LayerInfo info;
            // Calculate the maximum size (regular nodes only)
            QSizeF maxSize = computeLayerMaxSize(layers[idx]);
            LayerSize LayerSize{0, 0}; // Regular (height) node size and virtual node spacing
            QSizeF totalSize =
                computeLayerTotalSize(layers[idx], LayerSize, true);
            info.maxWidth = maxSize.width();
            info.maxHeight = totalSize.height(); // Initially include regular and virtual nodes

            // Vertical arrangement of nodes (regular nodes are on top and virtual nodes are on bottom)
            qreal starty = basePos.y() - (LayerSize.regularSize / 2); // Centered on regular nodes
            positionNodesInLayer(layers, info, starty, true, startx, saveLayerInfo, idx);

            startx += info.maxWidth + m_layerSpacing;
            if (saveLayerInfo) {
#ifdef FLOW_LAYOUT_DEBUG
                drawDebugRect(m_scene, QRectF(info.xMin, info.yMin, info.xMax - info.xMin,
                                              info.yMax - info.yMin));
                qDebug() << "Layer" << idx << ": xMin=" << info.xMin << "xMax=" << info.xMax
                         << "yMin=" << info.yMin << "yMax=" << info.yMax
                         << "maxWidth=" << info.maxWidth << "maxHeight=" << info.maxHeight;
#endif
                m_layerBounds[idx] = info;
            } else {
                m_tempLayerBounds[idx] = info;
            }
        }
    }

    void FlowChartLayouter::applyVertical(const QVector<QVector<FlowNode*>>& layers,
                                          const QPointF& basePos, bool saveLayerInfo) {
        qreal starty = basePos.y();
        for (int idx = 0; idx < layers.size(); ++idx) {
            LayerInfo info;
            // Calculate the maximum size (regular nodes only)
            QSizeF maxSize = computeLayerMaxSize(layers[idx]);
            LayerSize LayerSize{0, 0}; // Regular (width) node size and virtual node spacing
            QSizeF totalSize =
                computeLayerTotalSize(layers[idx], LayerSize, false);
            info.maxHeight = maxSize.height();
            info.maxWidth = totalSize.width(); // Initially include regular and virtual nodes

            // Horizontal arrangement of nodes (regular nodes are on the left and virtual nodes are on the right)
            qreal startx = basePos.x() - (LayerSize.regularSize / 2); // Centered on regular nodes
            positionNodesInLayer(layers, info, startx, false, starty, saveLayerInfo, idx);

            starty += info.maxHeight + m_layerSpacing;
            if (saveLayerInfo) {
#ifdef FLOW_LAYOUT_DEBUG
                drawDebugRect(m_scene, QRectF(info.xMin, info.yMin, info.xMax - info.xMin,
                                              info.yMax - info.yMin));
                qDebug() << "Layer" << idx << ": xMin=" << info.xMin << "xMax=" << info.xMax
                         << "yMin=" << info.yMin << "yMax=" << info.yMax
                         << "maxWidth=" << info.maxWidth << "maxHeight=" << info.maxHeight;
#endif
                m_layerBounds[idx] = info;
            } else {
                m_tempLayerBounds[idx] = info;
            }
        }
    }

    LayerInfo
    FlowChartLayouter::computeBoundsFromLayers(const QVector<LayerInfo>& layerBounds) const {
        LayerInfo bounds;
        for (const auto& info : layerBounds) {
            bounds.xMin = qMin(bounds.xMin, info.xMin);
            bounds.xMax = qMax(bounds.xMax, info.xMax);
            bounds.yMin = qMin(bounds.yMin, info.yMin);
            bounds.yMax = qMax(bounds.yMax, info.yMax);
        }
        bounds.maxWidth = qAbs(bounds.xMax - bounds.xMin) + 2 * m_layerPadding;
        bounds.maxHeight = qAbs(bounds.yMax - bounds.yMin) + 2 * m_layerPadding;
        return bounds;
    }

    void FlowChartLayouter::updateSceneBounds() {
        //
        m_wholeLayerInfo = computeBoundsFromLayers(m_layerBounds);

        QRectF innerRect(m_wholeLayerInfo.xMin - m_layerPadding,
                         m_wholeLayerInfo.yMin - m_layerPadding, m_wholeLayerInfo.maxWidth,
                         m_wholeLayerInfo.maxHeight);
        m_scene->setChartInnerRect(innerRect);

        QRectF titleRect;
        if (QGraphicsItem* titleItem = m_scene->getTitleItem()) {
            QRectF titleRect = titleItem->boundingRect();
            // Calculate the centered X position & top Y position
            qreal offsetX = innerRect.center().x() - titleRect.center().x();
            qreal titleY = innerRect.top() - (titleRect.height() / 2) - m_layerPadding;
            titleItem->setPos(offsetX, titleY);
            // Add the title to the innerRect scope (the position after using translated)
            QRectF titleBounds = titleRect.translated(titleItem->pos());
            innerRect = innerRect.united(titleBounds);
            // Update new innerRect
            m_scene->setChartInnerRect(innerRect);
        }

        QRectF outerRect =
            innerRect.adjusted(-m_scene->getSaftyMargin(), -m_scene->getSaftyMargin(),
                               m_scene->getSaftyMargin(), m_scene->getSaftyMargin());
        m_scene->setChartOuterRect(outerRect);

#ifdef FLOW_LAYOUT_DEBUG
        qDebug() << "WholeLayerInfo: xMin=" << m_wholeLayerInfo.xMin
                 << "xMax=" << m_wholeLayerInfo.xMax << "yMin=" << m_wholeLayerInfo.yMin
                 << "yMax=" << m_wholeLayerInfo.yMax << "maxWidth=" << m_wholeLayerInfo.maxWidth
                 << "maxHeight=" << m_wholeLayerInfo.maxHeight;
        qDebug() << "InnerRect:" << innerRect << "OuterRect:" << outerRect;
#endif
    }

    void FlowChartLayouter::adjustView() {
        if (m_wholeLayerInfo.maxWidth <= 0 || m_wholeLayerInfo.maxHeight <= 0 || m_scene == nullptr) {
            return;
        }

        constexpr QRectF defaultRect(0, 0, 1200, 800);

        // Get outerRect and window sizes
        QRectF outerRect = m_scene->chartOuterRect();
        QRectF visibleRect = m_scene->views().isEmpty()
                                 ? defaultRect
                                 : m_scene->views().first()->viewport()->rect();

        const qreal minWidth = 300.0;
        const qreal minHeight = 300.0;
        if (visibleRect.width() < minWidth || visibleRect.height() < minHeight) {
            visibleRect = defaultRect;
#ifdef FLOW_LAYOUT_DEBUG
            qDebug() << "AdjustView: Using default visibleRect (800x600) due to small size:"
                     << visibleRect;
#endif
        }

        // Calculate the scaling factor (adapt width or height, take the minimum)
        qreal wzoom = visibleRect.width() / (outerRect.width() + 2 * m_scene->getSaftyMargin());
        qreal hzoom = visibleRect.height() / (outerRect.height() + 2 * m_scene->getSaftyMargin());
        qreal scaleFactor = qMin(wzoom, hzoom);
        scaleFactor = qBound(DEFAULT_ZOOM_MIN, scaleFactor, DEFAULT_ZOOM_MAX);

        // Centered view
        QPointF graphCenter((m_wholeLayerInfo.xMin + m_wholeLayerInfo.xMax) / 2,
                            (m_wholeLayerInfo.yMin + m_wholeLayerInfo.yMax) / 2);

        // Notification scene application zoom
        m_scene->applyViewScale(scaleFactor, graphCenter);

        m_scene->update();

#ifdef FLOW_LAYOUT_DEBUG
        qDebug() << "AdjustView: scaleFactor=" << scaleFactor << "center=" << graphCenter
                 << "outerRect=" << outerRect;
#endif
    }

} // namespace hbchart