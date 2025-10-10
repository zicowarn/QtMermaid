/**
 * File: GanttRenderer.cpp
 * Description: The implementation of the GanttRenderer class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */

#include "GanttRenderer.h"

namespace hbchart {
    // -- Implementation of GanttRenderer --
    GanttRenderer::GanttRenderer() {
        m_core = CoreFactory::create(QtMermaidType::FLOW);
        if (!m_core) {
            // throw std::runtime_error("Failed to create GanttCore");
        }
    }

    void GanttRenderer::render(const ChartGraph& graph) {
      // TODO: 实现渲染逻辑
    }

    QGraphicsView* GanttRenderer::getView() {
        if (m_core != nullptr) {
            return m_core->getView();
        }
        return nullptr;
    }
} // namespace hbchart