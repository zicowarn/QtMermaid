/**
 * File: FlowChartView.cpp
 * Description: The implementation of the FlowChartView class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "FlowChartView.h"
#include "FlowChartScene.h"
#include <QGraphicsItem>

namespace hbchart {

    FlowChartView::FlowChartView(QGraphicsScene* scene, QWidget* parent)
        : QGraphicsView(scene, parent) {
        // Set transformation anchor to mouse position
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        // Enable smooth scaling
        setRenderHint(QPainter::Antialiasing);
        // Default is no drag mode
        setDragMode(QGraphicsView::NoDrag);
        // Optional: Enable mouse click selection
        setInteractive(true);
        // Debounce timer to apply zoom
        m_debouceTimer = new QTimer(this);
        m_debouceTimer->setSingleShot(true);
        connect(m_debouceTimer, &QTimer::timeout, this, &FlowChartView::onApplyPendingScale);
    }

    void FlowChartView::setZoom(qreal wzoom, qreal hzoom) {
        // Use the minimum of wzoom and hzoom
        qreal zoom = qMax(wzoom, hzoom);
        zoom = qBound(DEFAULT_ZOOM_MIN, zoom, DEFAULT_ZOOM_MAX);
        // Apply zoom
        scale(zoom, zoom);
        //
        auto* flowScene = qobject_cast<FlowChartScene*>(scene());
        if (flowScene != nullptr) {
            flowScene->updateViewScaleFactor(zoom);
        }
    }

    void FlowChartView::mousePressEvent(QMouseEvent* event) {
        QGraphicsItem* item = itemAt(event->pos());
        if (item != nullptr && (item->flags() & QGraphicsItem::ItemIsMovable) != 0) {
            // Moveable graphics, disable view dragging
            setDragMode(QGraphicsView::NoDrag);
        } else {
            // Blank area, enable scrolling
            setDragMode(QGraphicsView::ScrollHandDrag);
        }
        QGraphicsView::mousePressEvent(event);
    }

    void FlowChartView::mouseReleaseEvent(QMouseEvent* event) {
        setDragMode(QGraphicsView::NoDrag); // Reset drag mode
        QGraphicsView::mouseReleaseEvent(event);
    }

    void FlowChartView::wheelEvent(QWheelEvent* event) {
        static QTime lastEvent = QTime::currentTime();
        constexpr qreal TIME_INTERVAL = 50; // milliseconds
        if (lastEvent.msecsTo(QTime::currentTime()) < TIME_INTERVAL) {
            event->ignore();
            return;
        }
        lastEvent = QTime::currentTime();
        // Get wheel direction and magnitude
        // Get mouse position in scene coordinates
        QPointF mouseScenePos = mapToScene(event->position().toPoint());

        // Wheel direction and scale factor
        qreal angle = event->angleDelta().y();
        constexpr qreal SCALE_FACTOR_MAX = 1.1;
        constexpr qreal SCALE_FACTOR_MIN = 0.9;
        qreal scaleFactor = (angle > 0) ? SCALE_FACTOR_MAX : SCALE_FACTOR_MIN;

        // Limit zoom range
        qreal currentScale = transform().m11();
        qreal newScale = currentScale * scaleFactor;
        if (newScale < DEFAULT_ZOOM_MIN || newScale > DEFAULT_ZOOM_MAX) {
            event->ignore();
            return;
        }
        // Apply scaling
        scale(scaleFactor, scaleFactor);

        // Store pending scale and mouse position
        m_pendingScale = newScale;
        m_pendingMouseScenePos = mouseScenePos;
        // Debounce timer to apply zoom
        m_debouceTimer->start(TIME_INTERVAL);

        event->accept();
    }

    void FlowChartView::onApplyPendingScale() {
        // Apply delay scaling
        auto* flowScene = qobject_cast<FlowChartScene*>(scene());
        if (flowScene != nullptr) {
            flowScene->updateViewScaleFactor(m_pendingScale);
        }

        // Adjust offset to maintain mouse position
        QPointF newMouseScenePos = mapToScene(mapFromScene(m_pendingMouseScenePos));
        QPointF delta = m_pendingMouseScenePos - newMouseScenePos;
        translate(delta.x(), delta.y());

        // Reset viewport
        QRectF sceneBounds = scene()->sceneRect();
        QRectF viewportRect = mapToScene(viewport()->rect()).boundingRect();
        if (!sceneBounds.contains(viewportRect)) {
            centerOn(sceneBounds.center());
        }
    }

} // namespace hbchart
