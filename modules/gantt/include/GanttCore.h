/**
 * File: GanttCore.h
 * Description: The declaration of the GanttCore class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/20
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#ifndef GANTTCORE_H
#define GANTTCORE_H

#include "Factory.h"
#include "ChartGraph.h"
#include "Interfaces.h"
#include "GanttChartModel.h"
#include "GanttChartLayouter.h"
#include "GanttChartCoordinate.h"

namespace hbchart {
  /**
     * @brief Ganttchart core 
     */
    class GanttCore : public Core {
      private:
        std::unique_ptr<GanttChartModel> m_model;
        std::unique_ptr<GanttChartLayouter> m_layouter;
        std::unique_ptr<GanttChartCoordinate> m_coordinate;
        std::unique_ptr<QGraphicsScene> m_scene;
        std::unique_ptr<QGraphicsView> m_view;

      public:
        GanttCore();
        ~GanttCore() override;
        QGraphicsScene* getScene() override;
        QGraphicsView* getView() override;
        void setData(const ChartGraph& graph) override;
        void updateScene() override;
        void applyLayout() override;
        void drawConnectors() override;
        void updateCoordinate() override;
    };
    REGISTER_CORE(GANTT, GanttCore)
}

#endif // GANTTCORE_H