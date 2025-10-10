/**
 * File: FlowRenderer.h
 * Description: The declaration of the FlowRenderer class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWRENDERER_H
#define FLOWRENDERER_H

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     * @brief The FlowRenderer class. ihnerits from Renderer.
     */
    class FlowRenderer : public Renderer {
      private:
        std::unique_ptr<Core> m_core;    ///< Core object

      public:
        /**
         * @brief FlowRenderer constructor
         */
        FlowRenderer();
        ~FlowRenderer() override = default;   ///< Destructor

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
    REGISTER_RENDERER(FLOW, FlowRenderer)
} // namespace hbchart



#endif // FLOWRENDERER_H