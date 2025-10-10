/**
 * File: GanttRenderer.h
 * Description: The declaration of the GanttRenderer class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/20
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTRENDERER_H
#define GANTTRENDERER_H

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     * @brief The GanttRenderer class. ihnerits from Renderer.
     */
    class GanttRenderer : public Renderer {
      private:
        std::unique_ptr<Core> m_core;    ///< Core object

      public:
        /**
         * @brief GanttRenderer constructor
         */
        GanttRenderer();
        ~GanttRenderer() override = default;   ///< Destructor

        /**
         * @brief Render the chart graph data.
         * @param graph The chart graph data.
         */
        void render(const ChartGraph& graph) override;

        /**
         * @brief Get the graphics scene.
         * @return The graphics scene.
         */
        QGraphicsView* getView() override;
    };
    REGISTER_RENDERER(GANTT, GanttRenderer)
} // namespace hbchart



#endif // GANTTRENDERER_H