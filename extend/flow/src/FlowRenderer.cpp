/**
 * File: FlowRenderer.cpp
 * Description: The implementation of the FlowRenderer class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */

#include "FlowRenderer.h"

namespace hbchart {
    // -- Implementation of FlowRenderer --
    FlowRenderer::FlowRenderer() {
        m_core = CoreFactory::create(QtMermaidType::FLOW);
        if (!m_core) {
            // throw std::runtime_error("Failed to create FlowCore");
        }
    }

    void FlowRenderer::render(const ChartGraph& graph) {
      // TODO: Implement rendering logic
    }

    QGraphicsView* FlowRenderer::getView() {
        if (m_core != nullptr) {
            return m_core->getView();
        }
        return nullptr;
    }
} // namespace hbchart