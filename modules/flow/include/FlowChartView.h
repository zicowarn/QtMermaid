/**
 * File: FlowChartView.h
 * Description: The declaration of the FlowChartView class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWCHARTVIEW_H
#define FLOWCHARTVIEW_H

#include <QDebug>
#include <QGraphicsView>
#include <QTimer>
#include <QTime>
#include <QWheelEvent>

namespace hbchart {

    constexpr qreal DEFAULT_ZOOM_MIN = 0.2;           ///< Default minimum zoom scale
    constexpr qreal DEFAULT_ZOOM_MAX = 3.0;           ///< Default maximum zoom scale

    /**
     * @brief Flowchart view
     * @details Inherits from QGraphicsView, used for drawing flowcharts
     * @note
     * Note: This class is only for flowchart drawing, not for node and connection interactions.
     * Mainly implements zooming, panning, dragging and other functions.
     */
    class FlowChartView : public QGraphicsView {
        Q_OBJECT

      private:
        QTimer* m_debouceTimer;
        qreal m_pendingScale = 1.0;
        QPointF m_pendingMouseScenePos;

      public:
        /**
         * @brief Constructor
         * @param scene Scene
         * @param parent Parent object
         */
        explicit FlowChartView(QGraphicsScene* scene, QWidget* parent = nullptr);
        ~FlowChartView() override = default; ///< Destructor

        /**
         * @brief Set zoom
         * @param wzoom Width zoom scale
         * @param hzoom Height zoom scale
         * @note Note: It's recommended to use this method for zooming rather than
         * directly calling QGraphicsView::scale()
         */
        void setZoom(qreal wzoom, qreal hzoom);

        protected:
        /**
         * @brief Handle mouse press event
         * @param event Event
         */
        void mousePressEvent(QMouseEvent* event) override;

        /**
         * @brief Handle mouse move event
         * @param event Event
         */
        void mouseReleaseEvent(QMouseEvent* event) override;

        /**
         * @brief Handle wheel event
         * @param event Event
         */
        void wheelEvent(QWheelEvent* event) override;

      private slots:
        /**
         * @brief Handle debounced zoom event
         */
        void onApplyPendingScale();
    };
} // namespace hbchart

#endif // FLOWCHARTVIEW_H
