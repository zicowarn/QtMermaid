/**
 * File: FlowCore.h
 * Description: The declaration of the FlowCore class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/20
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */
#ifndef FLOWCORE_H
#define FLOWCORE_H

#include "Factory.h"
#include "ChartGraph.h"
#include "FlowChartConnector.h"
#include "FlowChartLayouter.h"
#include "FlowChartModel.h"
#include "Interfaces.h"

namespace hbchart {

    /**
     * @brief Flowchart core
     */
    class FlowCore : public Core {
      private:
        std::unique_ptr<FlowChartModel> m_model;
        std::unique_ptr<FlowChartLayouter> m_layouter;
        std::unique_ptr<FlowChartConnector> m_connector;
        std::unique_ptr<QGraphicsScene> m_scene;
        std::unique_ptr<QGraphicsView> m_view;

      public:
        FlowCore();
        ~FlowCore() override;
        QGraphicsScene* getScene() override;
        QGraphicsView* getView() override;
        void setData(const ChartGraph& graph) override;
        void updateScene() override;
        void applyLayout() override;
        void drawConnectors() override;
        void updateCoordinate() override;
    };
    REGISTER_CORE(FLOW, FlowCore)
} // namespace hbchart

#endif // FLOWCORE_H