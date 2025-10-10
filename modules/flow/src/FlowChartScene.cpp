/**
 * File: FlowChartScene.cpp
 * Description: The implementation of the FlowChartScene class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "FlowChartScene.h"
#include "FlowChartView.h"
#include <QDebug>
#include <QGraphicsView>
#include <QPainter>
#include <QtMath>

#include "FlowChartShapeItem.h"

#define FLOW_SCENE_DEBUG

namespace hbchart {

    void drawDebugPoint(QGraphicsScene* scene, const QPointF& center, const QString& text,
                        const QColor& color) {
        constexpr qreal DEFAULT_SIZE = 2.0;
        constexpr qreal POS_OFFSET = DEFAULT_SIZE / 2;
        auto* label = scene->addText(text);
        label->setDefaultTextColor(color);
        label->setPos(center);
        scene->addEllipse(center.x() - POS_OFFSET, center.y() - POS_OFFSET, DEFAULT_SIZE, DEFAULT_SIZE, QPen(color, 1), QBrush(color));
    }

    void drawDebugRect(QGraphicsScene* scene, const QRectF& rect, const QColor& color) {
        scene->addRect(rect, QPen(color, 1));
    }

    /// --- FlowChartScene Impl ---
    FlowChartScene::FlowChartScene(FlowChartModel* model, QObject* parent)
        : m_model(model), QGraphicsScene(parent) {
        constexpr qreal DEFAULT_SCALE = 1.0;
        m_levelColors = {
            Qt::lightGray, // Level 0 (root image)
            Qt::blue,      // Level 1
            Qt::green,     // Level 2
            Qt::yellow     // Level 3
            // Expand more colors
        };
        m_nodeRegularItems.clear();
        m_virtualNodeItems.clear();
        m_subgraphNodeItems.clear();
        m_subgraphItems.clear();
        m_titleItem = nullptr;
        m_viewScale = DEFAULT_SCALE; // View is currently zoomed, 1.0=100%
        m_gridSize = DEFAULT_GRID_SIZE;
        m_minChartWidth = CHART_MAX_WIDTH;
        m_minChartHeight = CHART_MIN_HEIGHT;
        m_chartMargin = CHART_MARGIN;
        m_saftyMargin = CHART_SAFETY_MARGIN;
        m_showControlPoints = false;
        m_DrawChartBoundary = false;
#ifdef FLOW_SCENE_DEBUG
        m_showControlPoints = true;
        m_DrawChartBoundary = true;
#endif
    }

    qreal FlowChartScene::getGridSize() const { return m_gridSize; }

    void FlowChartScene::setChartMinWidth(qreal width) { m_minChartWidth = width; }

    qreal FlowChartScene::chartMinWidth() const { return m_minChartWidth; }

    void FlowChartScene::setChartMinHeight(qreal height) { m_minChartHeight = height; }

    qreal FlowChartScene::chartMinHeight() const { return m_minChartHeight; }

    void FlowChartScene::setChartMargin(qreal margin) { m_chartMargin = margin; }

    qreal FlowChartScene::chartMargin() const { return m_chartMargin; }

    void FlowChartScene::setSaftyMargin(qreal margin) { m_saftyMargin = margin; }

    qreal FlowChartScene::getSaftyMargin() const { return m_saftyMargin; }

    void FlowChartScene::setLevelColors(const QList<QColor>& colors) { m_levelColors = colors; }

    void FlowChartScene::updateViewScaleFactor(qreal scaleFactor) { m_viewScale = scaleFactor; }

    FlowChartLabel* FlowChartScene::getTitleItem() const { return m_titleItem; }

    void FlowChartScene::setDrawChartBoundary(bool draw) { m_DrawChartBoundary = draw; }

    QRectF FlowChartScene::chartInnerRect() const { return m_chartInnerRect; }

    void FlowChartScene::setChartInnerRect(const QRectF& rect) { m_chartInnerRect = rect; }

    QRectF FlowChartScene::chartOuterRect() const { return m_chartOuterRect; }

    void FlowChartScene::setChartOuterRect(const QRectF& rect) { m_chartOuterRect = rect; }

    QGraphicsItem* FlowChartScene::getNodeItem(const FlowNode* node) const {
        if (node == nullptr) {
            return nullptr;
        }
        if (m_nodeRegularItems.contains(node)) {
            return m_nodeRegularItems.value(node);
        }
        if (m_subgraphNodeItems.contains(node)) {
            return m_subgraphNodeItems.value(node);
        } 
        if (m_virtualNodeItems.contains(node)) {
            return m_virtualNodeItems.value(node);
        }
        return nullptr;
    }

    QGraphicsItem* FlowChartScene::getRegularNodeItem(const FlowNode* node) const {
        if (node == nullptr) {
            return nullptr;
        }
        return m_nodeRegularItems.contains(node) ? m_nodeRegularItems.value(node) : nullptr;
    }

    QGraphicsItem* FlowChartScene::getVirtualNodeItem(const FlowNode* node) const {
        if (node == nullptr) {
            return nullptr;
        }
        return m_virtualNodeItems.contains(node) ? m_virtualNodeItems.value(node) : nullptr;
    }

    QGraphicsItem* FlowChartScene::getSubgraphNodeItem(const FlowNode* subgraph) const {
        if (subgraph == nullptr) {
            return nullptr;
        }
        return m_subgraphNodeItems.contains(subgraph) ? m_subgraphNodeItems.value(subgraph) : nullptr;
    }

    QGraphicsItem* FlowChartScene::getSubgraphItem(const FlowChartModel* subgraph) const {
        if (subgraph == nullptr) {
            return nullptr;
        }
        return m_subgraphItems.contains(subgraph) ? m_subgraphItems.value(subgraph) : nullptr;
    }

    void FlowChartScene::applyViewScale(qreal scale, QPointF graphCenter) {
        if (views().isEmpty()) {
            return;
        }
        auto* view = qobject_cast<FlowChartView*>(views().first());
        if (view != nullptr) {
            view->setZoom(scale, scale);
            updateViewScaleFactor(scale);
            view->centerOn(graphCenter);
        }
    }

    void FlowChartScene::setShowControlPoints(bool show) {
        m_showControlPoints = show;
    }

    bool FlowChartScene::showControlPoints() const { return m_showControlPoints; }

    void FlowChartScene::updateScene() {
        // Delete the title
        if (m_titleItem != nullptr) {
            delete m_titleItem;
            m_titleItem = nullptr;
        }
        for (auto* nodeItem : m_nodeRegularItems) {
            delete nodeItem;
        }
        m_nodeRegularItems.clear();
        for (auto* subgraphItem : m_subgraphNodeItems) {
            delete subgraphItem;
        }
        m_subgraphNodeItems.clear();
        for (auto* virtualNode : m_virtualNodeItems) {
            delete virtualNode;
        }
        m_virtualNodeItems.clear();
        m_subgraphItems.clear();
        // Iterate over all nodes and remove them from the scene
        if (m_model == nullptr) {
            return;
        }

        // Add a title to the scene
        addTitle(m_model->getTitle());

        // Iterate through all nodes and subgraphs
        addNodesRecursively(m_model);
    }

    void FlowChartScene::drawBackground(QPainter* painter, const QRectF& rect) {
        //
        const bool isDrawLine = false;                       // Draw grid lines?
        constexpr QColor LINE_COLOR = QColor(220, 220, 220); // Light gray
        constexpr QColor GRID_COLOR = QColor(120, 120, 120); // White
        constexpr qreal POINT_SIZE = 0.5;                    // Size of grid points
        QPen pen;
        if (isDrawLine) {
            pen.setColor(LINE_COLOR); // Light gray
        } else {
            pen.setColor(GRID_COLOR); // dark gray
        }
        pen.setWidth(1);
        painter->setPen(pen);

        qreal adjustedGridSize = getGridSize();

        // Calculate area to draw
        qreal left = std::floor(rect.left() / adjustedGridSize) * adjustedGridSize;
        qreal right = std::ceil(rect.right() / adjustedGridSize) * adjustedGridSize;
        qreal top = std::floor(rect.top() / adjustedGridSize) * adjustedGridSize;
        qreal bottom = std::ceil(rect.bottom() / adjustedGridSize) * adjustedGridSize;

        if (isDrawLine) {
            // Draw vertical grid lines
            int iCount = qCeil((right - left) / adjustedGridSize);
            for (int i = 0; i < iCount; i++) {
                qreal xpos = left + (i * adjustedGridSize);
                painter->drawLine(QPointF(xpos, top), QPointF(xpos, bottom));
            }

            int jCount = qCeil((bottom - top) / adjustedGridSize);
            for (int j = 0; j < jCount; j++) {
                qreal ypos = top + (j * adjustedGridSize);
                painter->drawLine(QPointF(left, ypos), QPointF(right, ypos));
            }
        } else {
            int iCount = qCeil((right - left) / adjustedGridSize);
            int jCount = qCeil((bottom - top) / adjustedGridSize);
            for (int i = 0; i < iCount; i++) {
                qreal xpos = left + (i * adjustedGridSize);
                for (int j = 0; j < jCount; j++) {
                    qreal ypos = top + (j * adjustedGridSize);
                    painter->drawPoint(QPointF(xpos, ypos));
                }
            }
        }

        // debug
        if (m_DrawChartBoundary) {
            painter->setPen(QPen(Qt::red, 1));
            painter->drawRect(m_chartInnerRect);
            painter->drawRect(m_chartOuterRect);
            // draw a black circle at  0,0
            painter->setPen(QPen(Qt::black, 1));
            painter->drawEllipse(QPointF(0, 0), 2, 2);
        }

        // draw a black circle at  0,0
        painter->setPen(QPen(Qt::black, 1));
        painter->drawEllipse(QPointF(0, 0), 2, 2);
    }

    void FlowChartScene::addTitle(const FlowTitle& title) {
        if (title.text.isEmpty() || !title.visible) {
            return;
        }
        m_titleItem = new FlowChartLabel(title.text, nullptr);
        addItem(m_titleItem);
    }

    void FlowChartScene::addNodesRecursively(const FlowChartModel* model,
                                             QSet<const FlowChartModel*>* processed) {
        // Check if we have a valid model
        if (model == nullptr) {
            return;
        }

        // Avoid infinite recursion by checking if we've already processed this model
        QSet<const FlowChartModel*> localProcessed;
        if (processed == nullptr) {
            processed = &localProcessed;
        }
        if (processed->contains(model)) {
            return;
        }
        processed->insert(model);
#ifdef FLOW_SCENE_DEBUG
        qDebug() << "Adding nodes for model:" << model->modelID()
                 << "nodes count:" << model->nodes().size();
#endif
        // Add all virtual nodes to the scene
        for (auto* node : model->virtualNodes()) {
            addVirtualNode(node);
        }

        // Add all nodes to the scene
        for (auto* node : model->nodes()) {
            addNode(node);
            if (node->type == NodeType::SUBGRAPH && node->subgraph != nullptr) {
                addNodesRecursively(node->subgraph);
            }
        }
    }

    void FlowChartScene::addVirtualNode(const FlowNode* node) {
        if (node == nullptr) {
            return;
        }
        QGraphicsItem* item = nullptr;
        if (node->type == NodeType::VIRTUAL) {
            auto* virtualItem = new VirtualNodeItem(node->label);
#ifdef FLOW_SCENE_DEBUG
            qDebug() << "Adding virtual node:" << node->label;
#endif
            m_virtualNodeItems[node] = virtualItem;
            addItem(virtualItem);
        } 
    }

    void FlowChartScene::addNode(const FlowNode* node) {
        if (node == nullptr) {
            return;
        }
        QGraphicsItem* item = nullptr;
        if (node->type == NodeType::SUBGRAPH) {
            const QString label = node->label.isEmpty() ? node->id : node->label;
#ifdef FLOW_SCENE_DEBUG
            qDebug() << "Adding subgraph:" << label
                    << "to model:" << (node->subgraph != nullptr ? node->subgraph->modelID() : "null");
#endif
            auto* subgraphItem = new SubgraphShapeItemRect(label, nullptr);
            if (node->subgraph != nullptr) {
                int level = node->subgraph->level();
                QColor color = m_levelColors.value(level % m_levelColors.size(), Qt::lightGray);
                constexpr qreal alpha = 0.1;
                color.setAlphaF(alpha);
                subgraphItem->setFillColor(color);
                m_subgraphItems[node->subgraph] = subgraphItem;
            }
            subgraphItem->setBorderColor(Qt::black);
            m_subgraphNodeItems[node] = subgraphItem;
            addItem(subgraphItem);
        } else if (node->type == NodeType::REGULAR) {
            const QString label = node->label.isEmpty() ? node->id : node->label;
#ifdef FLOW_SCENE_DEBUG
            qDebug() << "Adding node:" << label;
#endif
            if (node->shape == ShapeType::RECT) {
                item = new ShapeItemRect(label, nullptr);
            } else if (node->shape == ShapeType::CIRCLE) {
                item = new ShapeItemCircle(label, nullptr);
            } else if (node->shape == ShapeType::ROUNDED_RECT) {
                item = new ShapeItemRoundedRect(label, nullptr);
            } else if (node->shape == ShapeType::DIAMOND) {
                item = new ShapeItemDiamond(label, nullptr);
            } else {
                qDebug() << "Unknown shape type:" << static_cast<int>(node->shape);
                return;
            }
            addItem(item);
            if (item != nullptr) {
                m_nodeRegularItems[node] = item;
            }
        }
        
    }
} // namespace hbchart
