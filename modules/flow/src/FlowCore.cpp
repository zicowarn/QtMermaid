/**
 * File: FlowCore.cpp
 * Description: The implementation of the FlowCore class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "FlowCore.h"

namespace hbchart {
    /// --- FlowCore ---
    FlowCore::FlowCore() {
        int dummy = 1;
    }

    FlowCore::~FlowCore() {
        int dummy = 1;
    };

    QGraphicsScene* FlowCore::getScene() {
        return m_scene.get();
    }

    QGraphicsView* FlowCore::getView() {
        return m_view.get();
    }

    void FlowCore::setData(const ChartGraph& graph) {
    }

    void FlowCore::updateScene() {

    }

    void FlowCore::applyLayout() {
    }

    void FlowCore::drawConnectors() {
    }

    void FlowCore::updateCoordinate() {
    }

}