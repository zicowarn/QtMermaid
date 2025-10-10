

#ifndef CHARTGRAPH_H
#define CHARTGRAPH_H


#include <vector>

namespace hbchart {
    
    /**
     * @brief The base struct for chart graph data.
     * @details The base struct for chart graph data.
     */
    struct ChartGraph {
        virtual ~ChartGraph() = default;
    };

    /**
     * @brief The struct for flowchart graph data.
     * @details The struct for flowchart graph data.
     */
    struct FlowGraph : ChartGraph {
      /*
        std::vector<Node> nodes;
        std::vector<Edge> edges;
        */
    };

    /**
     * @brief The struct for gantt chart graph data.
     * @details The struct for gantt chart graph data.
     */
    struct GanttGraph : ChartGraph {
      /*
        std::vector<Section> sections;
        std::vector<Task> tasks;
        std::vector<Milestone> milestones;
        */
    };
} // namespace hbchart

#endif // CHARTGRAPH_H