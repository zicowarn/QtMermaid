/**
 * File: FlowChartModel.h
 * Description: The declaration of the FlowChartModel class. use DAG graph to represent the
 * flowchart. Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#ifndef FLOWCHART_MODEL_H
#define FLOWCHART_MODEL_H

#include <QColor>
#include <QFont>
#include <QMap>
#include <QObject>
#include <QRectF>
#include <QString>
#include <QVector>
#include <optional>

#define QTMERMAID_DEBUG

namespace hbchart {

    // Forward declarations
    class FlowChartModel;

    /**
     * @brief The template class StrongType.
     * @details This class is used to define strong types for task name, task description, task
     * start time, etc.
     * @tparam Tag The tag type.
     */
    template <typename Tag, typename ValueType> struct StrongType {
        // Tag type
        ValueType value;

        // Default constructor for empty value
        StrongType() = default;

        // Allow explicit construction (recommended to avoid implicit conversion)
        explicit StrongType(ValueType val) : value(std::move(val)) {}

        // Provides conversion to ValueType (optional)
        operator ValueType() const { return value; }
    };

    /**
     * @brief Layout direction
     */
    enum class LayoutDirection : std::uint8_t {
        HORIZONTAL, ///< Lay out nodes horizontally
        VERTICAL    ///< Lay out nodes vertically
    };

    /**
     * @brief The Events enum.
     */
    enum class EventFlowUpdateType : std::uint8_t {
        ALL,
        NODE_CHANGED,
        EDGE_CHANGED,
        SUBGRAPH_CHANGED
    };

    /**
     * @brief The NodeType enum.
     */
    enum class NodeType : std::uint8_t {
        NONE,    ///< None node type
        VIRTUAL, ///< Virtual node, used to represent subgraphs
        REGULAR, ///< Normal node
        SUBGRAPH ///< Subgraph node
    };

    /**
     * @brief The ShapeType enum.
     */
    enum class ShapeType : std::uint8_t {
        VIRTUAL,      ///< Virtual shape, used to represent subgraphs
        CIRCLE,       ///< Circle shape
        RECT,         ///< Rectangle shape
        ROUNDED_RECT, ///< Rounded rectangle shape
        DIAMOND       ///< Diamond shape
    };

    /**
     * @brief The EdgeStyle enum.
     */
    enum class EdgeStyle : std::uint8_t {
        INVISIBLE,             ///< Invisible line
        SOLID,                 ///< Solid line
        DASHED,                ///< Dashed line
        SOLIDARROW,            ///< Solid arrow
        DASHEDARROW,           ///< Dashed arrow
        SOLIDDOUBLEARROW,      ///< Solid double arrow
        DASHEDDOUBLEARROW,     ///< Dashed double arrow
        THICKSOLID,            ///< Thick solid line
        THICKDASHED,           ///< Thick dashed line
        THICKSOLIDARROW,       ///< Thick solid arrow
        THICKDASHEDARROW,      ///< Thick dashed arrow
        THICKSOLIDDOUBLEARROW, ///< Thick solid double arrow
        THICKDASHEDDOUBLEARROW ///< Thick dashed double arrow
    };

    /**
     * @brief GanttChart Title
     * @details Title is used to display text on the top of the chart,
     * it can be used to display some information or instructions.
     */
    struct FlowTitle {
        QString text;         ///< title text
        QFont font;           ///< font
        QColor color;         /// title color
        bool visible = false; ///< title visible
        FlowTitle() = default;

        FlowTitle(QString text_, const QFont& font_ = QFont(), QColor color_ = Qt::black)
            : text(std::move(text_)), font(font_), color(std::move(color_)) {}
    };

    /**
     * @brief The FlowNode struct.
     * @details This struct represents a node in the flowchart.
     * @sa FlowChartModel
     */
    struct FlowNode {
        QString id;
        NodeType type;
        ShapeType shape; ///< Only use REGULAR nodes
        QString label;
        FlowChartModel* subgraph = nullptr; ///< Only use SUBGRAPH nodes
        QString pathId = ""; ///< The path ID of the virtual node, such as "A_D" or "A_H"

        FlowNode() = default;

        FlowNode(QString id_, NodeType type_, ShapeType shape_, QString label_,
                 FlowChartModel* subgraph_)
            : id(std::move(id_)), type(type_), shape(shape_), label(std::move(label_)),
              subgraph(subgraph_) {}
    };

    /**
     * @brief The FlowEdge struct.
     * @details This struct represents an edge in the flowchart.
     * @sa FlowChartModel
     */
    struct FlowEdge {
        FlowNode* source; ///< Source node
        FlowNode* target; ///< Target node
        EdgeStyle style;  ///< Edge style
        QString label;    ///< Edge label
    };

    /**
     * @brief The VirtualNodeInfo struct.
     * @details This struct represents the information of a virtual node.
     */
    struct VirtualNodeInfo {
        QString sourceId;
        QString targetId;
        int span;
    };

    using NodeId = StrongType<struct NodeIdTag, QString>;       ///< Node ID type
    using SourceId = StrongType<struct SourceIdTag, QString>;   ///< Source ID type
    using TargetId = StrongType<struct TargetIdTag, QString>;   ///< Target ID type
    using NodeLabel = StrongType<struct NodeLabelTag, QString>; ///< Node label type
    using EdgeLabel = StrongType<struct EdgeLabelTag, QString>; ///< Edge label type

    /**
     * @brief The FlowChartModel class.
     * @details This class represents a flowchart model. It uses a DAG graph to represent the
     * flowchart.
     * @sa FlowNode, FlowEdge
     */
    class FlowChartModel : public QObject {
        Q_OBJECT

      private:
        QString m_modelID;                          ///< Model ID
        FlowTitle m_title;                          ///< Title
        FlowChartModel* m_parentModel;              ///< Parent model
        QMap<QString, FlowNode*> m_nodes;           ///< Node map
        QMap<QString, FlowNode*> m_virtualNodes;    ///< Virtual node map
        QVector<FlowEdge*> m_edges;                 ///< Edge list
        QMap<QString, FlowChartModel*> m_subgraphs; ///< Subgraph map
        int m_level; ///< Added: sub-graph level (root is 0, sub-graph increments)
        std::optional<LayoutDirection> m_layoutDirection; ///< Layout direction, default is vertical
        // Performance optimization: node index
        QMap<QString, FlowNode*> m_nodeIndex; ///< Format: Model_0::A, Sub1::C
        // Performance optimization: layer index
        bool m_isLayerValid;                       ///< Whether the layer is valid
        QVector<QVector<FlowNode*>> m_modelLayers; ///< Layers of the nodes
        QHash<QString, VirtualNodeInfo> m_virtualNodeInfo; ///< Node positions
        QHash<QString, QVector<FlowEdge*>> m_sourceEdges;  ///< Edge list
        QHash<QString, QVector<FlowEdge*>> m_targetEdges;  ///< Edge list

      signals:
        /**
         * @brief Signal emitted when data changes.
         * @param type The type of data change.
         */
        void dataChanged(EventFlowUpdateType type);

      public:
        /**
         * @brief Constructor.
         * @param parentModel Parent model.
         * @param parent Parent object.
         */
        explicit FlowChartModel(FlowChartModel* parentModel = nullptr, QObject* parent = nullptr);
        ~FlowChartModel() override = default; ///< Destructor

        /**
         * @brief Clear the model.
         */
        void clear();

        /**
         * @brief Set the model ID.
         */
        void setModelID(const QString& modelID);

        /**
         * @brief Get the model ID.
         * @return The model ID.
         */
        [[nodiscard]] QString modelID() const;

        /**
         * @brief Get the title.
         * @return The title.
         */
        void setTitle(const QString& title);

        /**
         * @brief Get the title.
         * @return The title.
         */
        FlowTitle& title();

        /**
         * @brief Get the parent model.
         * @return The parent model.
         */
        [[nodiscard]] const FlowTitle& getTitle() const;

        /**
         * @brief Invalidate the parent layers.
         * @details This function is called when the hierarchy of the nodes is changed. It
         * invalidates the parent layers so that they can be recalculated.
         */
        void invalidateParentLayers();

        /**
         * @brief Get the nodes.
         * @return The nodes.
         */
        [[nodiscard]] const QMap<QString, FlowNode*>& nodes() const { return m_nodes; }

        /**
         * @brief Get the virtual nodes.
         * @return The virtual nodes.
         */
        [[nodiscard]] const QMap<QString, FlowNode*>& virtualNodes() const { return m_virtualNodes; }

        /**
         * @brief Get the edges.
         * @return The edges.
         */
        [[nodiscard]] const QVector<FlowEdge*>& edges() const { return m_edges; }

        /**
         * @brief Get the subgraphs.
         * @return The subgraphs.
         */
        [[nodiscard]] const QMap<QString, FlowChartModel*>& subgraphs() const { return m_subgraphs; }

        /**
         * @brief Get the level.
         * @return The level.
         */
        [[nodiscard]] int level() const { return m_level; } // New: Get sub-layer level

        /**
         * @brief Add a node.
         * @param parentSubgraph The parent subgraph.
         * @param id The node ID.
         * @param label The node label.
         * @param shape The node shape.
         * @param type The node type.
         * @return True if the node is added successfully, false otherwise.
         * @details The node is added to the m_nodes of the parent subgraph.
         */
        bool addNode(FlowChartModel* parentSubgraph, const NodeId& nodeId,
                     const NodeLabel& label = NodeLabel(QString()),
                     ShapeType shape = ShapeType::RECT, NodeType type = NodeType::REGULAR);
        /**
         * @brief Add an edge.
         * @param sourceSubgraph The source subgraph.
         * @param sourceId The source node ID.
         * @param targetSubgraph The target subgraph.
         * @param targetId The target node ID.
         * @param style The edge style.
         * @param label The edge label.
         * @return True if the edge is added successfully, false otherwise.
         * @details Cross-sub-graph edges (main image to sub-graph, sub-graph to main image,
         * sub-graph to sub-graph) should be logically stored in the m_edges of the main image,
         * because：
         * 1. The main graph manages global topology (A, Sub1, Sub2 and their connections).
         * 2. The subgraph only manages internal nodes and edges (such as B -> C in Sub1).
         * 3. Cross subgraph edges affect the hierarchy calculation of the main graph (such as Sub1
         * -> A adjusts the hierarchy of A).
         */
        bool addEdge(FlowChartModel* sourceSubgraph, const SourceId& sourceId,
                     FlowChartModel* targetSubgraph, const TargetId& targetId,
                     EdgeStyle style = EdgeStyle::THICKSOLIDDOUBLEARROW,
                     const EdgeLabel& label = EdgeLabel(QString()));

        /**
         * @brief Add a subgraph.
         * @param parentSubgraph The parent subgraph.
         * @param name The subgraph name.
         * @return True if the subgraph is added successfully, false otherwise.
         * @details The subgraph is added to the m_subgraphs of the parent subgraph, and the parent
         * subgraph is added to the m_nodes of the subgraph.
         */
        bool addSubgraph(FlowChartModel* parentSubgraph, const QString& name);

        /**
         * @brief Find a node.
         * @param id The node ID.
         * @param subgraph The subgraph to search in.
         * @return The node if found, nullptr otherwise.
         */
        FlowNode* findNode(const QString& nodeId, FlowChartModel* subgraph = nullptr, bool recursive = false) const;

        /**
         * @brief Get a subgraph.
         * @param name The subgraph name.
         * @return The subgraph if found, nullptr otherwise.
         */
        FlowChartModel* getSubgraph(const QString& name);

        /**
         * @brief Set the layout direction.
         * @param direction The layout direction.
         */
        void setLayoutDirection(std::optional<LayoutDirection> direction);

        /**
         * @brief Get the effective layout direction.
         * @return The effective layout direction.
         */
        [[nodiscard]] LayoutDirection effectiveLayoutDirection() const;

        /**
         * @brief Topological sort (Kahn's algorithm).
         * @return The sorted node list.
         * @details The sorted node list is calculated based on the topological sort of the nodes.
         */
        [[nodiscard]] QVector<FlowNode*> topologicalSortKahn() const;

        /**
         * @brief Calculate the layers of the nodes.
         * @return The layers of the nodes.
         * @details The layers are calculated based on the topological sort of the nodes.
         */
        QVector<QVector<FlowNode*>> calculateLayers();

#ifdef QTMERMAID_DEBUG
        QList<FlowNode*> predecessors(FlowNode* node) const;
#endif // QTMERMAID_DEBUG

      private:
        
        /**
         * @brief Update the edge maps. sourcEdges and targetEdges are used to quickly find the
         * incoming and outgoing edges of a node.
         * @details Called after add virtual nodes.
         */
        void updateEdgeMaps();

        /**
         * @brief Find the layer index of a node.
         * @param node The node.
         * @param layers The layers of the nodes.
         * @return The layer index of the node.
         */
        static int findNodeLayer(const FlowNode* node, const QVector<QVector<FlowNode*>>& layers);

        /**
         * @brief Add virtual nodes.
         * @param layers The layers of the nodes.
         * @details Virtual nodes are added to the layers to ensure that the number of layers is
         * always the same.
         */
        void addVirtualNodes(QVector<QVector<FlowNode*>>& layers);

        static int findNodeIndex(FlowNode* node, const QVector<FlowNode*>& layer);

        [[nodiscard]] QVector<QPair<FlowNode*, double>> getRelatedNodes(const FlowNode* node, const VirtualNodeInfo& info,
                                                     const QVector<QVector<FlowNode*>>& layers) const;

        [[nodiscard]] std::optional<double> computeBarycenter(const FlowNode* node, int layerIndex,
                                            const QVector<QVector<FlowNode*>>& layers) const;

        [[nodiscard]] int countLayerCrossings(const QVector<QVector<FlowNode*>>& layers, int layerIndex) const;
        
        void adjustVirtualNodes(QVector<FlowNode*>& layer, int layerIndex,
                            const QVector<QVector<FlowNode*>>& layers, int& crossings) const;

        /**
         * @brief Optimize the layers of the nodes.
         * @param layers The layers of the nodes.
         * @details The layers are optimized to reduce the number of layers.
         */
        void optimizeByBarycenter(QVector<QVector<FlowNode*>>& layers);

        /**
         * @brief Calculate the layers of the nodes.
         * @return The layers of the nodes.
         * @details The layers are calculated based on the topological sort of the nodes.
         */
        [[nodiscard]] QVector<QVector<FlowNode*>> calculateLayersCore() const;

        /**
         * @brief Optimize the layers of the nodes.
         * @param layers The layers of the nodes.
         * @details The layers are optimized to reduce the number of layers.
         */
        void optimizeLayers(QVector<QVector<FlowNode*>>& layers);

        /**
         * @brief Get the layout direction.
         * @return The layout direction.
         */
        [[nodiscard]] std::optional<LayoutDirection> layoutDirection() const;

        /**
         * @brief Get the root model.
         * @return The root model.
         */
        FlowChartModel* getRootModel();

        /**
         * @brief Check if the edge is cross-sub-graph edge.
         * @param sourceGraph The source graph.
         * @param targetGraph The target graph.
         * @return True if the edge is cross-sub-graph edge, false otherwise.
         * @details Cross-sub-graph edges (main image to sub-graph, sub-graph to main image,
         * sub-graph to sub-graph) should be logically stored in the m_edges of the main image,
         * because：
         * 1. The main graph manages global topology (A, Sub1, Sub2 and their connections).
         * 2. The subgraph only manages internal nodes and edges (such as B -> C in Sub1).
         * 3. Cross subgraph edges affect the hierarchy calculation of the main graph (such as Sub1
         * -> A adjusts the hierarchy of A).
         */
        static bool isCrossSubgraphEdge(FlowChartModel* sourceGraph, FlowChartModel* targetGraph);

        /**
         * @brief Check if the edge would create a cycle.
         * @param source The source node.
         * @param target The target node.
         * @return True if the edge would create a cycle, false otherwise.
         */
        bool wouldCreateCycle(FlowNode* source, FlowNode* target) const;

        /**
         * @brief Topological sort (DFS).
         * @param node The current node.
         * @param visited The visited node set.
         * @param sorted The sorted node list.
         * @details The sorted node list is calculated based on the topological sort of the nodes.
         */
        void topologicalSortDFS(FlowNode* node, QHash<FlowNode*, int>& visited,
                                QVector<FlowNode*>& sorted) const;
#ifndef QTMERMAID_DEBUG
        /**
         * @brief Get the predecessors of a node.
         * @param node The node.
         * @return The predecessors of the node.
         */
        QList<FlowNode*> predecessors(FlowNode* node) const;
#endif // QTMERMAID_DEBUG
    };

} // namespace hbchart

#endif // FLOWCHART_MODEL_H