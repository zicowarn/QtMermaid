/**
 * File: GanttChartLayouter.cpp
 * Description: The implementation of GanttChartLayouter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "GanttChartLayouter.h"

#include <QDebug>

#include "GanttChartShapeItem.h"

// #define HBCHART_DEBUG

namespace hbchart {

    GanttChartLayouter::GanttChartLayouter(GanttChartModel* model, GanttChartScene* scene,
                                           QObject* parent)
        : QObject(parent), m_model(model), m_scene(scene) {
        constexpr qreal TIMER_INTERVAL = 1000; // 1s
        // Initialize layout parameters
        m_extraSpan = 0.0;
    }

    qreal GanttChartLayouter::getModelBaseTime() { return m_model->getBaseTime(); }

    QRectF GanttChartLayouter::getChartInnerRect() const { return m_innerRect; }

    QRectF GanttChartLayouter::getChartOuterRect() const { return m_boundingRect; }

    QRectF GanttChartLayouter::getChartSceneRect() const {
        // Calculate the extended boundingRect size
        qreal saftyMargin = m_scene->getSaftyMargin();
        QRectF extendedRect = m_boundingRect;
        extendedRect.adjust(-saftyMargin, -saftyMargin, saftyMargin, saftyMargin);
        return extendedRect;
    }

    QPointF GanttChartLayouter::getChartOrigin() const { return m_innerBottomLeft; };

    QPointF GanttChartLayouter::getChartEnd() const { return m_innerBottomRight; }

    qreal GanttChartLayouter::getExtraSpan() const { return m_extraSpan; }

    void GanttChartLayouter::apply() {
        // update innerRect
        updateInnerRect();
        // update boundingRect
        updateBoundingRect();
        // layout title
        layoutTitle();
        // layout sections
        for (const GanttSection* section : m_model->getSections()) {
            layoutSection(section);
        }
        // layout tasks
        int currentRow = 0;
        //
        for (const GanttTask* task : m_model->getRootTasks()) {
            layoutTask(task, currentRow);
        }
        // layout milestones
        for (const GanttMilestone* milestone : m_model->getMilestones()) {
            layoutMilestone(milestone);
        }

#ifdef HBCHART_DEBUG
        m_scene->setDrawChartBoundary(true);
        m_scene->setChartInnerRect(m_innerRect);
        m_scene->setChartOuterRect(m_boundingRect);
#endif

        // set scene rect
        QRectF sceneRect = getChartSceneRect();
        m_scene->setSceneRect(sceneRect);
        //
        m_scene->update();
        // send layout changed signal
        emit eventLayoutChanged();
    }

    void GanttChartLayouter::updateInnerRect() {
        // 1. Calculate the number of tasks
        int taskCount = m_model->getTaskRowsNumber();

        // 2. Calculate the minimum and maximum time
        // auto [minTime, maxTime] = m_model->getModelTimeRange();

        // 3. Get base time from the scene
        qreal baseTime = m_scene->getSceneBaseTime(); 
        qreal spanTime = m_scene->getSceneMaxDuration(); // Usually some extra stretching distance

        // 4. Calculate scene span
        TimeSceneRatio timeSceneRatio = m_scene->timeSceneRatio();
        m_extraSpan = timeSceneRatio.toScene(spanTime);

        // 5. Get Scene time range
        auto [sceneMinTime, sceneMaxTime] = m_scene->getSceneTimeRange();
        qreal sceneDeltaTime = sceneMaxTime + spanTime - baseTime;
        sceneDeltaTime = std::max(sceneDeltaTime, 0.0); // Prevent negative numbers

        // 6. Get scene parameters
        qreal rowMargin = m_scene->rowMargin();
        qreal rowHeight = m_scene->rowHeight();

        // 7. Calculate inner size
        qreal innerWidth = timeSceneRatio.toScene(sceneDeltaTime);
        qreal innerHeight = (taskCount * (rowHeight + rowMargin)) + rowMargin;

        // 8. Apply minimum size
        innerWidth = std::max(innerWidth, m_scene->chartMinWidth());
        innerHeight = std::max(innerHeight, m_scene->chartMinHeight());

        // 9. Set innerRect (midpoint layout)
        m_innerRect = QRectF(-innerWidth / 2, -innerHeight / 2, innerWidth, innerHeight);
        m_innerBottomLeft = QPointF(-innerWidth / 2, innerHeight / 2);
        m_innerBottomRight = QPointF(innerWidth / 2, innerHeight / 2);

        // 10. Grid alignment (gridSize fixed, such as 20px, not bound to widthPerUnit)
        qreal gridSize = m_scene->getGridSize(); // For example 20.0
        qreal originX = m_innerBottomLeft.x();
        qreal alignedOriginX =
            std::floor(originX / gridSize) * gridSize; // Align to the nearest grid on the left
        qreal delta = alignedOriginX - originX;        // Offset

        // 11. Apply offsets
        m_innerRect.translate(delta, 0);
        m_innerBottomLeft.setX(alignedOriginX);
        m_innerBottomRight.setX(alignedOriginX + innerWidth);
    }

    void GanttChartLayouter::updateBoundingRect() {
        // Calculate boundingRect size
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        constexpr qreal SIZE_FACTOR_SIXFOLD = 6.0;
        qreal totalWidth =
            m_innerRect.width() + (SIZE_FACTOR_SIXFOLD * m_scene->chartMargin()) + (SIZE_FACTOR_DOUBLE * m_scene->yAxisWidth());
        qreal totalHeight =
            m_innerRect.height() + (SIZE_FACTOR_DOUBLE * m_scene->chartMargin()) + m_scene->xAxisHeight();

        // Set boundingRect (midpoint layout)
        m_boundingRect = QRectF(-totalWidth / SIZE_FACTOR_DOUBLE, -totalHeight / SIZE_FACTOR_DOUBLE, totalWidth, totalHeight);
    }

    void GanttChartLayouter::layoutTitle() {
        GanttChartLabel* item = m_scene->getTitleItem();
        if (item == nullptr) {
            // When untitled, use updateBoundingRect's m_boundingRect
            return;
        }

        // Get the title size (trust GanttChartLabel handled scaling)
        QRectF titleBounding = item->boundingRect(); // Include margins
        qreal titleHeight = titleBounding.height();
        // qreal titleWidth = titleBounding.width();

        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0; // 2.0 times the size

        // Determine the top margin (including the title and additional chartMargin)
        qreal topMargin =
            titleHeight + (SIZE_FACTOR_DOUBLE * m_scene->chartMargin()); // Match updateBoundingRect's 2 *chartMargin

        // Adjust m_boundingRect (based on updateBoundingRect)
        qreal totalWidth = m_boundingRect.width(); // from updateBoundingRect
        qreal baseHeight = m_innerRect.height() + m_scene->xAxisHeight() + m_scene->chartMargin();
        qreal totalHeight = baseHeight + topMargin; // Replace the top chartMargin
        m_boundingRect = QRectF(-totalWidth / SIZE_FACTOR_DOUBLE, (-totalHeight / SIZE_FACTOR_DOUBLE) - (m_scene->chartMargin() / SIZE_FACTOR_DOUBLE),
                                totalWidth, totalHeight);

        // Set the title position
        qreal xpos = 0.0; // Center
        qreal ypos = m_innerRect.top() - (titleHeight / SIZE_FACTOR_DOUBLE) - m_scene->chartMargin();
        item->setPos(xpos, ypos);
    }

    void GanttChartLayouter::layoutSection(const GanttSection* section) {
        GanttChartShapeItem* item = m_scene->findSectionItem(section);
        if (item != nullptr) {
            constexpr qreal SIZE_FACTOR_DOUBLE = 2.0; // 2.0 times the size
            constexpr qreal SIZE_FACTOR_TRIPLE = 3.0;
            if (section->taskIndex == -1 && section->taskCount == 0) {
                item->setVisible(false);
                return;
            }
            // Calculate position and size
            qreal x_left = m_innerRect.left() - (SIZE_FACTOR_TRIPLE * m_scene->chartMargin()) - m_scene->yAxisWidth();
            int rowIndex = section->taskIndex;
            int rowCount = section->taskCount;
            //
            qreal rowMargin = m_scene->rowMargin();
            qreal rowHeight = m_scene->rowHeight();
            qreal y_top = m_innerRect.top() + (rowIndex * (rowHeight + rowMargin));
            qreal itemWidth =
                m_innerRect.width() + (SIZE_FACTOR_TRIPLE * m_scene->chartMargin()) + m_scene->yAxisWidth();
            qreal itemHeight = rowCount * (rowHeight + rowMargin);
            // update size
            item->setRect(RowWidth{itemWidth}, RowHeight{itemHeight});
            // update position
            qreal xpos = x_left + (itemWidth / SIZE_FACTOR_DOUBLE);
            qreal ypos = y_top + (itemHeight / SIZE_FACTOR_DOUBLE);
            // set new position
            item->setPos(xpos, ypos);
        } else {
            qWarning() << "Section item not found for" << section->name;
        }
    }

    void GanttChartLayouter::layoutTask(const GanttTask* task, int& currentRow) {
        GanttChartShapeItem* item = m_scene->findTaskItem(task);
        if (item != nullptr) {
            constexpr qreal SIZE_FACTOR_DOUBLE = 2.0; // 2.0 times the size
            // Calculate position and size
            qreal timeDeltaSec = task->start - m_model->getBaseTime();
            TimeUnitType unitType = m_scene->getCoreTimeUnitType();
            qreal timeDeltaUnit = unitType.fromSeconds(timeDeltaSec);
            TimeSceneRatio timeSceneRatio = m_scene->timeSceneRatio();
            qreal rowMargin = m_scene->rowMargin();
            qreal rowHeight = m_scene->rowHeight();
            //
            qreal x_left = m_innerRect.left() + timeSceneRatio.toScene(timeDeltaUnit);
            qreal ypos = m_innerRect.top() + rowMargin + (currentRow * (rowHeight + rowMargin)) +
                      (rowHeight / SIZE_FACTOR_DOUBLE);
            qreal xpos = x_left + (item->width() / SIZE_FACTOR_DOUBLE);
            // update limits
            item->setXLimits(m_innerRect.left(), m_innerRect.right() - m_extraSpan);
            // set new position
            item->setPos(xpos, ypos);
        } else {
            qWarning() << "Task item not found for" << task->name;
        }

        // Incremental row number
        currentRow++;

        // Recursively layout subtasks
        if (task->isExpanded) {
            for (const GanttTask* child : task->children) {
                layoutTask(child, currentRow);
            }
        }
    }

    void GanttChartLayouter::layoutMilestone(const GanttMilestone* milestone) {
        GanttChartShapeItem* item = m_scene->findMilestoneItem(milestone);
        if (item != nullptr) {
            constexpr qreal SIZE_FACTOR_DOUBLE = 2.0; // 2.0 times the size
            constexpr qreal SIZE_FACTOR_QUADRUPLE = 4.0;
            // Find the line number of the associated task
            const GanttTask* parentTask = milestone->parent;
            int parentRow = -1;
            std::function<bool(const GanttTask*, int&)> findRow = [&](const GanttTask* task,
                                                                      int& row) {
                row++;
                if (task == parentTask) {
                    parentRow = row;
                    return true;
                }
                if (task->isExpanded) {
                    for (const GanttTask* child : task->children) {
                        if (findRow(child, row)) {
                            return true;
                        }
                    }
                }
                return false;
            };
            int row = -1;
            for (const GanttTask* task : m_model->getRootTasks()) {
                if (findRow(task, row)) {
                    break;
                }
            }

            // If the parent task is found, layout the milestone
            if (parentRow >= 0) {
                qreal timeDeltaSec = milestone->time - m_model->getBaseTime();
                TimeUnitType unitType = m_scene->getCoreTimeUnitType();
                qreal timeDeltaUnit = unitType.fromSeconds(timeDeltaSec);
                TimeSceneRatio timeSceneRatio = m_scene->timeSceneRatio();
                qreal rowMargin = m_scene->rowMargin();
                qreal rowHeight = m_scene->rowHeight();

                qreal x_left = m_innerRect.left() + timeSceneRatio.toScene(timeDeltaUnit);
                qreal ypos = m_innerRect.top() + rowMargin + (parentRow * (rowHeight + (SIZE_FACTOR_DOUBLE * rowMargin))) +
                          (rowHeight / SIZE_FACTOR_DOUBLE) + (rowHeight / SIZE_FACTOR_QUADRUPLE);

                qreal xpos = x_left + (item->width() / SIZE_FACTOR_DOUBLE);
                // update limits
                item->setXLimits(m_innerRect.left(), m_innerRect.right() - m_extraSpan);
                // set new position
                item->setPos(xpos, ypos);
            }
        } else {
            qWarning() << "Milestone item not found for time" << milestone->time;
        }
    }

} // namespace hbchart
