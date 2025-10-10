/**
 * File: GanttChartLayouter.h
 * Description: The declaration of GanttChartLayouter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTCHARTLAYOUTER_H
#define GANTTCHARTLAYOUTER_H

#include <QObject>
#include <QPair>
#include <QTimer>

#include "GanttChartModel.h"
#include "GanttChartScene.h"

namespace hbchart {

    /**
     * @brief The GanttChartLayouter class.
     * @details This class is responsible for the layout of the Gantt chart.
     */
    class GanttChartLayouter : public QObject {
        Q_OBJECT

      private:
        GanttChartModel* m_model = nullptr;
        GanttChartScene* m_scene = nullptr;
        bool m_updatePending = false;

        // Layout area
        QRectF m_innerRect;         ///< Tasks and milestones drawing areas
        QRectF m_boundingRect;      ///< Scene range
        QPointF m_innerBottomLeft;  ///< innerRect bottom left corner
        QPointF m_innerBottomRight; ///< innerRect bottom right corner
        qreal m_extraSpan;          ///< Extra span for the chart

      Q_SIGNALS:
        /**
         * @brief Layout changed signal.
         */
        void eventLayoutChanged();

      public:
        /**
         * @brief Constructor.
         */
        explicit GanttChartLayouter(GanttChartModel* model, GanttChartScene* scene,
                                    QObject* parent = nullptr);
        ~GanttChartLayouter() override = default; ///< Destructor

        /**
         * @brief Get the model base time.
         * @return The model base time
         */
        qreal getModelBaseTime();

        /**
         * @brief Get the inner rect of the chart.
         * @return The inner rect of the chart.
         */
        [[nodiscard]] QRectF getChartInnerRect() const;

        /**
         * @brief Get the outer rect of the chart.
         * @return The outer rect of the chart.
         */
        [[nodiscard]] QRectF getChartOuterRect() const;

        /**
         * @brief Get the scene bounding rect. bouding rect + safety margin.
         * @return The extended bounding rect.
         */
        [[nodiscard]] QRectF getChartSceneRect() const;

        /**
         * @brief Get the chart origin.
         * @return The chart origin.
         */
        [[nodiscard]] QPointF getChartOrigin() const;

        /**
         * @brief Get the chart end point.
         * @return The chart end point.
         */
        [[nodiscard]] QPointF getChartEnd() const;

        /**
         * @brief Set the extra span for the chart.
         * @param span The extra span for the chart.
         */
        [[nodiscard]] qreal getExtraSpan() const;

        /**
         * @brief Layout the chart. This method should be called after the model is set.
         */
        void apply();

      private:
        /**
         * @brief Update the inner rect.
         */
        void updateInnerRect();

        /**
         * @brief Update the bounding rect.
         */
        void updateBoundingRect();

        /**
         * @brief Layout the title.
         */
        void layoutTitle();

        /**
         * @brief Layout the section.
         */
        void layoutSection(const GanttSection* section);

        /**
         * @brief Layout the task.
         * @param task The task to layout.
         * @param currentRow The current row.
         */
        void layoutTask(const GanttTask* task, int& currentRow);

        /**
         * @brief Layout the milestone.
         * @param milestone The milestone to layout.
         */
        void layoutMilestone(const GanttMilestone* milestone);
    };

} // namespace hbchart

#endif // GANTTCHARTLAYOUTER_H
