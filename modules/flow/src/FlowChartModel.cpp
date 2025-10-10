/**
 * File: FlowChartModel.cpp
 * Description: The implementation of the FlowChartModel class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */

#include "FlowChartModel.h"

#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QSet>
#include <QStack>
#include <qmath.h>

#define FLOW_MODEL_DEBUG

namespace hbchart {

    // 匿名命名空间，仅当前文件内可见
    namespace {
        constexpr int kMaxIterations = 10;
        constexpr double kSourceWeight = 1.0;   // 源后继权重
        constexpr double kNeighborWeight = 0.1; // 邻居权重
        constexpr double kMinCrossingImprovement = 1.0; // 交叉改进最小值
        constexpr double kTargetInfluenceWeight = 5.0; // 长边目标节点影响权重，这个权重决定了长边目标节点对虚拟节点位置的“拉力”


        enum class SegementType : std::uint8_t {
            FIRST_SEGMENT,
            INTERMEDIATE_SEGMENT,
            LAST_SEGMENT
        };

        const QMap<EdgeStyle, EdgeStyle> kFirstSegmentStyles = {
            {EdgeStyle::SOLIDARROW, EdgeStyle::SOLID},
            {EdgeStyle::DASHEDARROW, EdgeStyle::DASHED},
            {EdgeStyle::THICKSOLIDARROW, EdgeStyle::THICKSOLID},
            {EdgeStyle::THICKDASHEDARROW, EdgeStyle::THICKDASHED},
            {EdgeStyle::SOLIDDOUBLEARROW, EdgeStyle::SOLIDARROW},
            {EdgeStyle::DASHEDDOUBLEARROW, EdgeStyle::DASHEDARROW},
            {EdgeStyle::THICKSOLIDDOUBLEARROW, EdgeStyle::THICKSOLIDARROW},
            {EdgeStyle::THICKDASHEDDOUBLEARROW, EdgeStyle::THICKDASHEDARROW}};
        const QMap<EdgeStyle, EdgeStyle> kIntermediateSegmentStyles = {
            {EdgeStyle::SOLIDARROW, EdgeStyle::SOLID},
            {EdgeStyle::DASHEDARROW, EdgeStyle::DASHED},
            {EdgeStyle::THICKSOLIDARROW, EdgeStyle::THICKSOLID},
            {EdgeStyle::THICKDASHEDARROW, EdgeStyle::THICKDASHED},
            {EdgeStyle::SOLIDDOUBLEARROW, EdgeStyle::SOLID},
            {EdgeStyle::DASHEDDOUBLEARROW, EdgeStyle::DASHED},
            {EdgeStyle::THICKSOLIDDOUBLEARROW, EdgeStyle::THICKSOLID},
            {EdgeStyle::THICKDASHEDDOUBLEARROW, EdgeStyle::THICKDASHED}};
        const QMap<EdgeStyle, EdgeStyle> kLastSegmentStyles = {
            {EdgeStyle::SOLIDDOUBLEARROW, EdgeStyle::SOLIDARROW},
            {EdgeStyle::DASHEDDOUBLEARROW, EdgeStyle::DASHEDARROW},
            {EdgeStyle::THICKSOLIDDOUBLEARROW, EdgeStyle::THICKSOLIDARROW},
            {EdgeStyle::THICKDASHEDDOUBLEARROW, EdgeStyle::THICKDASHEDARROW}};

        void updateEdgeStyle(SegementType segType, EdgeStyle& setStyle) {
            static const QMap<SegementType, QMap<EdgeStyle, EdgeStyle>> styleMaps = {
                {SegementType::FIRST_SEGMENT, kFirstSegmentStyles},
                {SegementType::INTERMEDIATE_SEGMENT, kIntermediateSegmentStyles},
                {SegementType::LAST_SEGMENT, kLastSegmentStyles}};
            if (styleMaps[segType].contains(setStyle)) {
                setStyle = styleMaps[segType][setStyle];
            }
        }

        void mergeForInversionCounting(QVector<int>& arr, int left, int mid, int right,
                                       QVector<int>& temp, int& inversions) {
            int i = left;    // 左子数组的起始索引
            int j = mid + 1; // 右子数组的起始索引
            int k = left;    // 合并后数组的起始索引

            while (i <= mid && j <= right) {
                if (arr[i] <= arr[j]) {
                    temp[k++] = arr[i++];
                } else {
                    // 如果 arr[i] > arr[j]，则 arr[i...mid] 都大于 arr[j]
                    // 形成 (mid - i + 1) 个逆序对
                    temp[k++] = arr[j++];
                    inversions += (mid - i + 1);
                }
            }

            // 复制左子数组剩余的元素
            while (i <= mid) {
                temp[k++] = arr[i++];
            }

            // 复制右子数组剩余的元素
            while (j <= right) {
                temp[k++] = arr[j++];
            }

            // 将排序后的临时数组元素复制回原数组
            for (k = left; k <= right; ++k) {
                arr[k] = temp[k];
            }
        }

        void sortAndCountInversionsRecursive(QVector<int>& arr, int left, int right,
                                             QVector<int>& temp, int& inversions) {
            if (left >= right) {
                return;
            }
            int mid = left + (right - left) / 2;
            sortAndCountInversionsRecursive(arr, left, mid, temp, inversions);
            sortAndCountInversionsRecursive(arr, mid + 1, right, temp, inversions);
            mergeForInversionCounting(arr, left, mid, right, temp, inversions);
        }

        // 计算 QVector<int> 中的逆序对数量
        // 注意：这个函数会修改传入的 arr (使其排序)
        // 如果不希望修改原始数据，请传入副本：countInversions(QVector<int>(originalData))
        int countInversionsInList(QVector<int>& arr) {
            if (arr.size() < 2) {
                return 0;
            }
            int inversions = 0;
            QVector<int> temp(arr.size()); // 临时数组用于归并
            sortAndCountInversionsRecursive(arr, 0, arr.size() - 1, temp, inversions);
            return inversions;
        }
    } // namespace

    /// --- FlowChartModel Implementation ---
    FlowChartModel::FlowChartModel(FlowChartModel* parentModel, QObject* parent)
        : QObject(parent), m_parentModel(parentModel) {
        // Initialization level: The root image is 0, and the child image is based on the parent
        // image level + 1
        m_level = parentModel != nullptr ? parentModel->level() + 1 : 0;
        m_modelID = QString("Model_%1").arg(m_level); // initialization m_modelID
        // Initialize title
        QFont titlefont = QFont();
        titlefont.setBold(true);
        constexpr int TITLE_FONT_SIZE = 14;
        titlefont.setPointSize(TITLE_FONT_SIZE);
        m_title = FlowTitle("", titlefont, Qt::black);
        m_nodeIndex.clear();    // clear index
        m_nodes.clear();        // clear nodes
        m_virtualNodes.clear(); // clear virtual nodes
        m_edges.clear();        // clear edges
        m_subgraphs.clear();    // clear subgraphs
        m_isLayerValid = false;
        m_modelLayers.clear();     // clear model layers
        m_virtualNodeInfo.clear(); // clear virtual node info
        m_sourceEdges.clear();
        m_targetEdges.clear();
    }

    void FlowChartModel::clear() {
        qDeleteAll(m_nodes);
        m_nodes.clear();
        qDeleteAll(m_virtualNodes);
        m_virtualNodes.clear();
        qDeleteAll(m_edges);
        m_edges.clear();
        for (auto* subgraph : m_subgraphs.values()) {
            subgraph->clear();
            delete subgraph;
        }
        m_subgraphs.clear();
        m_nodeIndex.clear();
        m_isLayerValid = false;
        m_modelLayers.clear();
        m_virtualNodeInfo.clear();
        m_sourceEdges.clear();
        m_targetEdges.clear();
        emit dataChanged(EventFlowUpdateType::ALL);
    }

    void FlowChartModel::setModelID(const QString& modelID) {
        if (m_modelID == modelID) {
            return;
        }
        m_modelID = modelID;
    }

    QString FlowChartModel::modelID() const { return m_modelID; }

    void FlowChartModel::setTitle(const QString& title) {
        if (m_title.text == title || title.isEmpty()) {
            return;
        }
        m_title.text = title;
        m_title.visible = true;
    }

    FlowTitle& FlowChartModel::title() { return m_title; }

    const FlowTitle& FlowChartModel::getTitle() const { return m_title; }

    void FlowChartModel::invalidateParentLayers() {
        if (m_parentModel != nullptr) {
            m_parentModel->m_isLayerValid = false;
            m_parentModel->invalidateParentLayers();
        }
    }

    bool FlowChartModel::addNode(FlowChartModel* parentSubgraph, const NodeId& nodeId,
                                 const NodeLabel& label, ShapeType shape, NodeType type) {
        // If parentSubgraph is specified, add to the subgraph
        if (parentSubgraph != nullptr && parentSubgraph != this) {
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Adding node" << nodeId.value << "to subgraph" << parentSubgraph->m_modelID;
#endif
            bool success = parentSubgraph->addNode(nullptr, nodeId, label, shape, type);
            if (!success) {
                invalidateParentLayers();
            }
            return success;
        }

        // Check if the node exists
        if (m_nodes.contains(nodeId)) {
            qWarning() << "Node already exists:" << nodeId << "in model" << m_modelID;
            return false;
        }

        auto* node = new FlowNode{nodeId, type, shape, label, nullptr};
        QString fullId = m_modelID + "::" + nodeId;
        if (type == NodeType::SUBGRAPH) {
            node->subgraph = new FlowChartModel(
                this, nullptr); // The hierarchy is automatically set in the constructor
            node->subgraph->setObjectName(nodeId);
            node->subgraph->setModelID(nodeId);
            m_subgraphs[nodeId] = node->subgraph;
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Add subgraph:" << nodeId;
#endif
        }
        m_isLayerValid = false;
        m_nodes[nodeId] = node;
        m_nodeIndex[fullId] = node; // update index
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Add node:" << nodeId << "label:" << label;
#endif
        emit dataChanged(EventFlowUpdateType::NODE_CHANGED);
        return true;
    }

    bool FlowChartModel::addEdge(FlowChartModel* sourceSubgraph, const SourceId& sourceId,
                                 FlowChartModel* targetSubgraph, const TargetId& targetId,
                                 EdgeStyle style, const EdgeLabel& label) {
        if (isCrossSubgraphEdge(sourceSubgraph, targetSubgraph)) {
            FlowChartModel* rootModel = getRootModel();
            if (this != rootModel) {
                bool success = rootModel->addEdge(sourceSubgraph, sourceId, targetSubgraph,
                                                  targetId, style, label);
                if (success) {
                    invalidateParentLayers();
                }
                return success;
            }
        }
        FlowNode* source = findNode(sourceId, sourceSubgraph);
        FlowNode* target = findNode(targetId, targetSubgraph);
        // check if source and target exist
        if (source == nullptr || target == nullptr) {
            qWarning() << "Invalid source" << sourceId << "or target" << targetId;
            return false;
        }
        if (wouldCreateCycle(source, target)) {
            qWarning() << "Adding edge would create cycle";
            return false;
        }

        auto* edge = new FlowEdge{source, target, style, label};
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Add edge:" << source->id << "->" << target->id << "label:" << label;
#endif
        m_isLayerValid = false;
        m_edges.append(edge);
        emit dataChanged(EventFlowUpdateType::EDGE_CHANGED);
        return true;
    }

    bool FlowChartModel::addSubgraph(FlowChartModel* parentSubgraph, const QString& name) {
        if (parentSubgraph != nullptr && parentSubgraph != this) {
            qDebug() << "Adding subgraph" << name << "to parent" << parentSubgraph->m_modelID;
            bool success = parentSubgraph->addSubgraph(nullptr, name);
            if (success) {
                invalidateParentLayers();
            }
            return success;
        }

        if (m_subgraphs.contains(name)) {
            qWarning() << "Subgraph already exists:" << name << "in model" << m_modelID;
            return false;
        }

        bool success =
            addNode(nullptr, NodeId(name), NodeLabel(""), ShapeType::RECT, NodeType::SUBGRAPH);
        if (success) {
            m_isLayerValid = false;
            emit dataChanged(EventFlowUpdateType::SUBGRAPH_CHANGED);
        }
        return success;
    }

    FlowNode* FlowChartModel::findNode(const QString& nodeId, FlowChartModel* subgraph,
                                       bool recursive) const {
        if (!recursive) {
            if (subgraph != nullptr && subgraph != this) {
#ifdef FLOW_MODEL_DEBUG
                qDebug() << "Delegating search to subgraph:" << subgraph->m_modelID;
#endif
                return subgraph->findNode(nodeId, nullptr, false);
            }
            QString fullId = m_modelID + "::" + nodeId;
            qDebug() << "Searching node:" << fullId << "in model:" << m_modelID;
            FlowNode* node = m_nodeIndex.value(fullId, nullptr);
            if (node == nullptr) {
                qWarning() << "Node not found:" << fullId;
            }
            return node;
        }
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Recursive search for node:" << nodeId << "in model:" << m_modelID;
#endif
        if (subgraph != nullptr && subgraph != this) {
            return subgraph->findNode(nodeId, nullptr, true);
        }
        // Preferential check of global indexes
        QString fullId = m_modelID + "::" + nodeId;
        FlowNode* node = m_nodeIndex.value(fullId, nullptr);
        if (node != nullptr) {
            qDebug() << "Found node in index:" << fullId;
            return node;
        }
        // Check local nodes
        if (m_nodes.contains(nodeId)) {
            qDebug() << "Found node in local nodes:" << nodeId;
            return m_nodes[nodeId];
        }
        // Recursive search for sub-pictures
        for (auto* sub : m_subgraphs.values()) {
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Searching in subgraph:" << sub->m_modelID;
#endif
            node = sub->findNode(nodeId, nullptr, true);
            if (node != nullptr) {
                qDebug() << "Found node in subgraph:" << sub->m_modelID << ":" << nodeId;
                return node;
            }
        }
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Node not found recursively:" << nodeId;
#endif
        return nullptr;
    }

    FlowChartModel* FlowChartModel::getSubgraph(const QString& name) {
        FlowChartModel* subgraph = m_subgraphs.value(name, nullptr);
        if (subgraph == nullptr) {
            qWarning() << "Subgraph" << name << "not found in model" << m_modelID;
        } else {
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Found subgraph" << name << "with m_modelID" << subgraph->m_modelID;
            subgraph->m_parentModel = this;
#endif
        }

        return subgraph;
    }

    void FlowChartModel::setLayoutDirection(std::optional<LayoutDirection> direction) {
        m_layoutDirection = direction;
    }

    LayoutDirection FlowChartModel::effectiveLayoutDirection() const {
        if (m_layoutDirection) {
            return *m_layoutDirection;
        }
        if (m_parentModel != nullptr) {
            return m_parentModel->effectiveLayoutDirection();
        }
        return LayoutDirection::VERTICAL;
    }

    QVector<FlowNode*> FlowChartModel::topologicalSortKahn() const {
        QSet<FlowNode*> allNodes;
        QHash<FlowNode*, int> inDegree;

        // Collect only nodes of the current model
        for (auto* node : m_nodes.values()) {
            allNodes.insert(node);
        }

        // Initialize the entry level
        for (auto* node : allNodes) {
            inDegree[node] = 0;
        }

        // Collect only the edges of the current model
        for (auto* edge : m_edges) {
            if (allNodes.contains(edge->target)) {
                inDegree[edge->target]++;
            }
        }

        // Kahn algorithm
        QQueue<FlowNode*> queue;
        for (auto* node : allNodes) {
            if (inDegree[node] == 0) {
                queue.enqueue(node);
            }
        }

        QVector<FlowNode*> sorted;
        while (!queue.isEmpty()) {
            FlowNode* node = queue.dequeue();
            sorted.append(node);
            for (auto* edge : m_edges) {
                if (edge->source == node && allNodes.contains(edge->target)) {
                    if (--inDegree[edge->target] == 0) {
                        queue.enqueue(edge->target);
                    }
                }
            }
        }

        if (sorted.size() != allNodes.size()) {
            throw std::runtime_error("Graph contains a cycle");
        }

        return sorted;
    }

    QVector<QVector<FlowNode*>> FlowChartModel::calculateLayers() {
        if (m_isLayerValid) {
            return m_modelLayers;
        }
        // Initial layer allocation
        auto layers = calculateLayersCore();
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Core layers for" << modelID() << ":" << layers.size();
        for (int i = 0; i < layers.size(); ++i) {
            QStringList nodeIds;
            for (auto* node : layers[i]) {
                nodeIds << node->id;
            }
            qDebug() << "Layer" << i << ":" << nodeIds;
        }
#endif
        // Optimization (E and B are the same layer)
        optimizeLayers(layers);
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Optimized layers for" << modelID() << ":" << layers.size();
        for (int i = 0; i < layers.size(); ++i) {
            QStringList nodeIds;
            for (auto* node : layers[i]) {
                nodeIds << node->id;
            }
            qDebug() << "Layer" << i << ":" << nodeIds;
        }
#endif
        // Add virtual nodes (E and B are in the same layer)
        addVirtualNodes(layers);
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Added virtual layers for" << modelID() << ":" << layers.size();
        for (int i = 0; i < layers.size(); ++i) {
            QStringList nodeIds;
            for (auto* node : layers[i]) {
                nodeIds << node->id;
            }
            qDebug() << "Layer" << i << ":" << nodeIds;
        }
#endif
        // Update edge maps
        updateEdgeMaps();
        // Optimization (B same layer)
        optimizeByBarycenter(layers);
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Optimized by barycenter layers for" << modelID() << ":" << layers.size();
        for (int i = 0; i < layers.size(); ++i) {
            QStringList nodeIds;
            for (auto* node : layers[i]) {
                nodeIds << node->id;
            }
            qDebug() << "Layer" << i << ":" << nodeIds;
        }
#endif
        m_modelLayers.clear();
        m_modelLayers = std::move(layers);
        m_isLayerValid = true;
        return m_modelLayers;
    }

    void FlowChartModel::updateEdgeMaps() {
        m_sourceEdges.clear();
        m_targetEdges.clear();
        for (const auto& edge : m_edges) {
            m_sourceEdges[edge->source->id].append(edge);
            m_targetEdges[edge->target->id].append(edge);
        }
    }

    int FlowChartModel::findNodeLayer(const FlowNode* node,
                                      const QVector<QVector<FlowNode*>>& layers) {
        for (int i = 0; i < layers.size(); ++i) {
            if (layers[i].contains(const_cast<FlowNode*>(node))) {
                return i;
            }
        }
        return -1;
    }

    void FlowChartModel::addVirtualNodes(
        QVector<QVector<FlowNode*>>& layers) { // NOLINT(readability-function-cognitive-complexity)
        m_virtualNodes.clear();
        m_virtualNodeInfo.clear();
        QVector<FlowEdge*> newEdges;
        for (auto* edge : m_edges) {
            int sourceLayer = findNodeLayer(edge->source, layers);
            int targetLayer = findNodeLayer(edge->target, layers);
            if (qAbs(sourceLayer - targetLayer) <= 1) {
                newEdges.append(edge);
                continue;
            }
            FlowNode* prevNode = edge->source;
            bool firstEdge = true;
            QString pathId = QString("%1_%2").arg(edge->source->id, edge->target->id); // Path ID
            //
            for (int idx = sourceLayer + 1; idx < targetLayer; ++idx) {
                auto* virtualNode = new FlowNode;
                QString label = QString("virtual_%1_%2_%3")
                                    .arg(edge->source->id, edge->target->id, QString::number(idx));
                virtualNode->id = label;
                virtualNode->type = NodeType::VIRTUAL;
                virtualNode->shape = ShapeType::VIRTUAL;
                virtualNode->label = label;
                virtualNode->pathId = pathId; // Set path ID
                if (edge->source->subgraph != nullptr &&
                    edge->source->subgraph == edge->target->subgraph) {
                    virtualNode->subgraph = edge->source->subgraph;
                }
                layers[idx].append(virtualNode);
                m_virtualNodes[label] = virtualNode;
                m_virtualNodeInfo[label] =
                    VirtualNodeInfo{edge->source->id, edge->target->id, targetLayer - sourceLayer};

                // Reset edge style
                EdgeStyle setStyle = edge->style;
                if (firstEdge) {
                    updateEdgeStyle(SegementType::FIRST_SEGMENT, setStyle);
                    firstEdge = false;
                } else {
                    updateEdgeStyle(SegementType::INTERMEDIATE_SEGMENT, setStyle);
                }
                newEdges.append(new FlowEdge{prevNode, virtualNode, setStyle, edge->label});
                prevNode = virtualNode;
            }
            EdgeStyle setStyle = edge->style;
            updateEdgeStyle(SegementType::LAST_SEGMENT, setStyle);
            newEdges.append(new FlowEdge{prevNode, edge->target, setStyle, edge->label});
            delete edge; // Clean up original edge
        }
        m_edges = std::move(newEdges);
    }

    int FlowChartModel::findNodeIndex(FlowNode* node, const QVector<FlowNode*>& layer) {
        return layer.indexOf(node);
    }

    QVector<QPair<FlowNode*, double>>
    FlowChartModel::getRelatedNodes(const FlowNode* node, const VirtualNodeInfo& info,
                                    const QVector<QVector<FlowNode*>>& layers) const {
        QVector<QPair<FlowNode*, double>> relatedNodes;
        // 源后继
        if (m_sourceEdges.contains(info.sourceId)) {
            for (const auto* edge : m_sourceEdges[info.sourceId]) {
                relatedNodes.append({edge->target, kSourceWeight});
            }
        }
        // 目标前驱
        if (m_targetEdges.contains(info.targetId)) {
            for (const auto* edge : m_targetEdges[info.targetId]) {
                relatedNodes.append({edge->source, kSourceWeight});
            }
        }
        // 直接邻居（同一路径）
        if (node->type == NodeType::VIRTUAL && m_virtualNodeInfo.contains(node->id)) {
            for (const auto* edge : m_edges) {
                if (edge->source == node || edge->target == node) {
                    FlowNode* neighbor = (edge->source == node) ? edge->target : edge->source;
                    // 检查邻居是否属于同一路径
                    if (neighbor->type == NodeType::VIRTUAL &&
                        m_virtualNodeInfo.contains(neighbor->id)) {
                        const auto& neighborInfo = m_virtualNodeInfo[neighbor->id];
                        if (neighborInfo.sourceId == info.sourceId &&
                            neighborInfo.targetId == info.targetId) {
                            relatedNodes.append({neighbor, kNeighborWeight});
                        }
                    } else if (neighbor->id == info.sourceId || neighbor->id == info.targetId) {
                        relatedNodes.append({neighbor, kNeighborWeight});
                    }
                }
            }
        }
#ifdef FLOW_MODEL_DEBUG
        QStringList neighborIds;
        for (const auto& pair : relatedNodes) {
            neighborIds << pair.first->id;
        }
        qDebug() << "Node" << node->id << "related nodes:" << neighborIds;
#endif
        return relatedNodes;
    }

    std::optional<double>
    FlowChartModel::computeBarycenter( // NOLINT(readability-function-cognitive-complexity)
        const FlowNode* node, int layerIndex, const QVector<QVector<FlowNode*>>& layers) const {

        double barycenterSum = 0.0;
        double totalWeight = 0.0;

        if (node->type == NodeType::VIRTUAL) {
            // --- 虚拟节点处理：锚点法 ---
            FlowNode* pathPredecessor = nullptr;

            // 尝试从 m_targetEdges (入边缓存) 中找到路径前驱
            // 假设虚拟节点在路径上只有一条主要的结构性入边
            if (m_targetEdges.contains(node->id)) {
                const QVector<FlowEdge*> incomingEdges =
                    m_targetEdges.value(node->id);       // 注意原为 const FlowEdge*
                for (const auto* edge : incomingEdges) { // 遍历找到非回环的结构边
                    if (edge->source->type == NodeType::VIRTUAL ||
                        edge->source->type ==
                            NodeType::REGULAR) { // 简化判断，可以是任何类型的有效前驱
                                                 // 确保这条边确实是连接到上一层的结构边
                        int predLayerActual = findNodeLayer(edge->source, layers);
                        if (predLayerActual == layerIndex - 1) {
                            pathPredecessor = edge->source;
                            break;
                        }
                    }
                }
            }
            // 如果 m_targetEdges 中没有或不适用，可以作为 fallback 遍历 m_edges (效率较低)
            // if (!pathPredecessor) { /* ... fallback ... */ }

            if (pathPredecessor) {
                int predecessorLayerIndex = findNodeLayer(pathPredecessor, layers);
                // 确保前驱确实在上一层
                if (predecessorLayerIndex == layerIndex - 1) {
                    int predecessorPos =
                        findNodeIndex(pathPredecessor, layers[predecessorLayerIndex]);
                    if (predecessorPos != -1) {
#ifdef FLOW_MODEL_DEBUG
                        qDebug() << "Virtual Node" << node->id << "in Lyr" << layerIndex
                                 << "anchoring to predecessor" << pathPredecessor->id << "at Pos"
                                 << predecessorPos << "in Lyr" << predecessorLayerIndex;
#endif
                        return static_cast<double>(predecessorPos);
                    }
                } else {
#ifdef FLOW_MODEL_DEBUG
                    qDebug() << "Virtual Node" << node->id << "in Lyr" << layerIndex
                             << "found path predecessor" << pathPredecessor->id
                             << "but it's in unexpected layer" << predecessorLayerIndex;
#endif
                }
            } else {
#ifdef FLOW_MODEL_DEBUG
                qDebug() << "Virtual Node" << node->id << "in Lyr" << layerIndex
                         << "could not find valid path predecessor.";
#endif
            }
            // Fallback: 如果找不到有效的前驱，或者前驱不在上一层，则让它保持当前相对位置
            // 或者取层内节点数量的一半作为参照，尝试居中。
            // 这里返回当前位置，让它在排序时尽可能不移动（除非其他节点把它挤开）
            // 或者也可以不返回，让下面的 totalWeight == 0 逻辑处理
            int currentIndex = layers[layerIndex].indexOf(const_cast<FlowNode*>(node));
            return currentIndex != -1 ? std::optional<double>(static_cast<double>(currentIndex))
                                      : std::nullopt;

        } else { // --- 普通节点处理 (与您之前的逻辑类似) ---
            // 确保只考虑连接到相邻层的边
            // 使用 m_sourceEdges 和 m_targetEdges 提高效率
            if (m_sourceEdges.contains(node->id)) {
                for (const auto* edge : m_sourceEdges.value(node->id)) { // 注意原为 const FlowEdge*
                    if (findNodeLayer(edge->target, layers) == layerIndex + 1) {
                        int index = findNodeIndex(edge->target, layers[layerIndex + 1]);
                        if (index != -1) {
                            barycenterSum += index;
                            totalWeight += 1.0;
                        }
                    }
                }
            }
            if (m_targetEdges.contains(node->id)) {
                for (const auto* edge : m_targetEdges.value(node->id)) { // 注意原为 const FlowEdge*
                    if (findNodeLayer(edge->source, layers) == layerIndex - 1) {
                        int index = findNodeIndex(edge->source, layers[layerIndex - 1]);
                        if (index != -1) {
                            barycenterSum += index;
                            totalWeight += 1.0;
                        }
                    }
                }
            }
        }

        if (totalWeight > 0) {
            double result = barycenterSum / totalWeight;
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Node" << node->id << (node->type == NodeType::VIRTUAL ? "(V)" : "(N)")
                     << "barycenter:" << result << "totalWeight:" << totalWeight;
#endif
            return result;
        }

        // 如果没有连接的邻居，或者对于虚拟节点没有找到有效前驱且未返回，
        // 则返回其当前在层内的索引，使其尽可能保持原位。
        // 或者返回 std::nullopt，让调用者处理。
        // 之前的版本是返回当前索引，我们保持这个逻辑。
        int currentIndex = layers[layerIndex].indexOf(const_cast<FlowNode*>(node));
#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Node" << node->id << (node->type == NodeType::VIRTUAL ? "(V)" : "(N)")
                 << "barycenter: (no_weights_fallback_to_current_pos)" << currentIndex;
#endif
        return currentIndex != -1 ? std::optional<double>(static_cast<double>(currentIndex))
                                  : std::nullopt; // 或者总是返回一个值
    }

    int FlowChartModel::countLayerCrossings(const QVector<QVector<FlowNode*>>& layers,
                                            int layerIndex) const {
        if (layerIndex < 0 || layerIndex >= layers.size() - 1) {
            return 0; // 没有下一层或层索引无效
        }

        const QVector<FlowNode*>& currentLayerNodes = layers[layerIndex];
        const QVector<FlowNode*>& nextLayerNodes = layers[layerIndex + 1];

        if (currentLayerNodes.isEmpty() || nextLayerNodes.isEmpty()) {
            return 0;
        }

        QVector<QPair<int, int>> interLayerEdgePositions;
        interLayerEdgePositions.reserve(m_edges.size()); // 预估容量

        // 1. 收集跨层边的 (源节点层内索引, 目标节点层内索引)
        // 为了效率，这里直接遍历 m_edges。如果节点很多，可以优化为遍历当前层的节点再查其出边。
        for (const auto* edge : m_edges) {
            if (edge->source == nullptr || edge->target == nullptr)
                continue;

            // 检查边是否连接当前层和下一层
            // (findNodeLayer 和 findNodeIndex 可能有性能开销，但为了逻辑清晰先这样)
            // 优化点: 如果已经有 node -> layer_idx 的映射会更快
            int sourceNodeLayer = findNodeLayer(edge->source, layers);
            if (sourceNodeLayer != layerIndex)
                continue;

            int targetNodeLayer = findNodeLayer(edge->target, layers);
            if (targetNodeLayer != layerIndex + 1)
                continue;

            int sourceNodePos = findNodeIndex(edge->source, currentLayerNodes);
            int targetNodePos = findNodeIndex(edge->target, nextLayerNodes);

            if (sourceNodePos != -1 && targetNodePos != -1) {
                interLayerEdgePositions.append({sourceNodePos, targetNodePos});
            }
        }

        if (interLayerEdgePositions.isEmpty()) {
            return 0;
        }

        // 2. 按源节点位置排序边 (如果源位置相同，按目标位置排序)
        std::sort(interLayerEdgePositions.begin(), interLayerEdgePositions.end(),
                  [](const QPair<int, int>& a, const QPair<int, int>& b) {
                      if (a.first != b.first) {
                          return a.first < b.first;
                      }
                      return a.second < b.second;
                  });

        // 3. 提取目标节点位置序列
        QVector<int> targetPositions;
        targetPositions.reserve(interLayerEdgePositions.size());
        for (const auto& edgePosPair : interLayerEdgePositions) {
            targetPositions.append(edgePosPair.second);
        }

        // 4. 计算目标序列的逆序对数量
        // 创建副本进行计算，以防原始 targetPositions (如果还需要) 被修改
        QVector<int> targetPositionsForCounting = targetPositions;
        int crossings = countInversionsInList(targetPositionsForCounting);

#ifdef FLOW_MODEL_DEBUG
        qDebug() << "Layer" << layerIndex << "->" << (layerIndex + 1)
                 << "calculated crossings:" << crossings;
#endif
        return crossings;
    }

    void FlowChartModel::adjustVirtualNodes(QVector<FlowNode*>& layer, int layerIndex,
                                            const QVector<QVector<FlowNode*>>& layers,
                                            int& crossings) const {
        for (int jdx = 0; jdx < layer.size(); ++jdx) {
            if (layer[jdx]->type != NodeType::VIRTUAL) {
                continue;
            }
            // 扩展交换范围
            for (int kdx = -2; kdx <= 2; ++kdx) {
                if (kdx == 0)
                    continue;
                int newPos = jdx + kdx;
                if (newPos >= 0 && newPos < layer.size()) {
                    QVector<FlowNode*> testLayer = layer;
                    std::swap(testLayer[jdx], testLayer[newPos]);
                    QVector<QVector<FlowNode*>> testLayers = layers;
                    testLayers[layerIndex] = testLayer;
                    int newCrossings = countLayerCrossings(testLayers, layerIndex);
                    if (newCrossings < crossings) {
                        layer = testLayer;
                        crossings = newCrossings;
#ifdef FLOW_MODEL_DEBUG
                        qDebug() << "Layer" << layerIndex << "crossings reduced to:" << crossings;
#endif
                    }
                }
            }
        }
    }

    void FlowChartModel::optimizeByBarycenter(QVector<QVector<FlowNode*>>& layers) {
        if (layers.isEmpty())
            return;

        // (可选) 在迭代开始前，为稳定性，可以记录一下各层节点的初始相对顺序
        // QList<QHash<FlowNode*, int>> previousIterationOrder; ...

        int prevTotalCrossings = std::numeric_limits<int>::max();
        for (int iter = 0; iter < kMaxIterations; ++iter) {
            int currentTotalCrossings = 0;

            // --- (可选) 双向扫描 ---
            // bool sweepDown = (iter % 2 == 0); // 或者其他扫描策略
            // if (sweepDown) {
            //     for (int i = 0; i < layers.size(); ++i) { ... }
            // } else {
            //     for (int i = layers.size() - 1; i >= 0; --i) { ... }
            // }
            // 当前只实现单向从上到下 (i = 0 to layers.size() - 1)

            for (int i = 0; i < layers.size(); ++i) {
                if (layers[i].isEmpty())
                    continue;

                QVector<QPair<FlowNode*, QPair<double, int>>> nodeBarycentersWithInitialIndex;
                nodeBarycentersWithInitialIndex.reserve(layers[i].size());

                for (int nodeIdx = 0; nodeIdx < layers[i].size(); ++nodeIdx) {
                    FlowNode* node = layers[i][nodeIdx];
                    std::optional<double> barycenterOpt = computeBarycenter(node, i, layers);

                    // 如果 computeBarycenter 返回 std::nullopt，使用节点当前位置或层中心
                    double barycenterValue = barycenterOpt.value_or(static_cast<double>(nodeIdx));

                    // 次级排序键：使用节点在当前层迭代开始时的索引 nodeIdx
                    nodeBarycentersWithInitialIndex.append({node, {barycenterValue, nodeIdx}});
                }

                std::sort(nodeBarycentersWithInitialIndex.begin(),
                          nodeBarycentersWithInitialIndex.end(), [](const auto& a, const auto& b) {
                              // 优先按重心值排序
                              if (qAbs(a.second.first - b.second.first) > 1e-9) { // 比较double
                                  return a.second.first < b.second.first;
                              }
                              // 重心值相近或相等时，按初始索引排序 (保持稳定性)
                              return a.second.second < b.second.second;
                          });

                QVector<FlowNode*> newLayerOrder;
                newLayerOrder.reserve(layers[i].size());
                for (const auto& pair : nodeBarycentersWithInitialIndex) {
                    newLayerOrder.append(pair.first);
                }
                layers[i] = newLayerOrder;

                // 在排序后、调整虚拟节点前，或调整后计算交叉数
                // 这里我们在调整虚拟节点后统一计算当前层与下一层间的交叉
                // 注意: countLayerCrossings 计算的是 layer[i] 和 layer[i+1] 间的交叉
                // adjustVirtualNodes 只调整 layer[i] 内部虚拟节点，试图优化 layer[i-1]<->layer[i]
                // 和 layer[i]<->layer[i+1]

                int crossingsAfterSort = countLayerCrossings(layers, i); // Crossings i -> i+1
                // (可选) 如果也关心 i-1 -> i 的交叉，也需要计算
                // int crossingsFromPrev = (i > 0) ? countLayerCrossings(layers, i - 1) : 0;

                // adjustVirtualNodes 尝试通过局部交换虚拟节点来减少交叉
                // 它内部会调用 countLayerCrossings，传递的是调整后的 testLayers
                // 所以，crossingsAfterSort 只是一个参考值，实际交叉数由 adjustVirtualNodes
                // 内部的最终值为准
                int crossingsForThisLayerPair = crossingsAfterSort; // 初始化
                if (i < layers.size() - 1) { // 只对非最后一层有意义，因为它影响 i -> i+1
                    adjustVirtualNodes(layers[i], i, layers, crossingsForThisLayerPair);
                }

                if (i < layers.size() - 1) { // 只累加有效的层间交叉
                    currentTotalCrossings += crossingsForThisLayerPair;
                }

#ifdef FLOW_MODEL_DEBUG
                QStringList nodeIds;
                for (auto* n : layers[i])
                    nodeIds << n->id;
                qDebug() << "Iter" << iter << "Layer" << i << "order:" << nodeIds
                         << "crossings to next:"
                         << ((i < layers.size() - 1) ? crossingsForThisLayerPair : 0);
#endif
            }
#ifdef FLOW_MODEL_DEBUG
            qDebug() << "Iter" << iter << "Total Crossings:" << currentTotalCrossings;
#endif
            if (currentTotalCrossings >= prevTotalCrossings ||
                (prevTotalCrossings - currentTotalCrossings) < kMinCrossingImprovement &&
                    iter > 0) { // iter > 0 避免第一次就退出
                // (prevTotalCrossings - currentTotalCrossings) 对于int是安全的
                break;
            }
            prevTotalCrossings = currentTotalCrossings;
        }
    }

    QVector<QVector<FlowNode*>> FlowChartModel::calculateLayersCore() const {
        QVector<QVector<FlowNode*>> layers;
        try {
            auto sorted = topologicalSortKahn();
            QHash<FlowNode*, int> nodeLayers;
            for (auto* node : sorted) {
                int maxLayer = -1;
                for (auto* pred : predecessors(node)) {
                    if (nodeLayers.contains(pred)) {
                        maxLayer = qMax(maxLayer, nodeLayers[pred]);
                    }
                }
                int layer = maxLayer + 1;
                nodeLayers[node] = layer;
                if (layers.size() <= layer) {
                    layers.resize(layer + 1);
                }
                layers[layer].append(node);
            }
            for (const QVector<FlowNode*>& layer : layers) {
                QStringList nodeIds;
                for (const auto* node : layer) {
                    nodeIds << node->id;
                }
            }
        } catch (const std::runtime_error& e) {
            qWarning() << "Calculate layers failed:" << e.what();
        }
        return layers;
    }

    void FlowChartModel::optimizeLayers(
        QVector<QVector<FlowNode*>>& layers) { // NOLINT(readability-function-cognitive-complexity)
        // If layer 0 has only one node (for example, A of layer 9 topology), no optimization is
        // required
        if (layers.isEmpty() || layers[0].size() <= 1) {
            return;
        }

        // Step 1: Calculate the longest path, number of successors and minimum successor levels
        // of layer 0 nodes
        struct NodeInfo {
            FlowNode* node;
            int longestPath;       // Maximum path length
            int successorCount;    // Number of successors
            int minSuccessorLayer; // Minimum level of succession
        };
        QVector<NodeInfo> startNodes;
        QHash<FlowNode*, int> currentLayer;
        for (int i = 0; i < layers.size(); ++i) {
            for (auto* node : layers[i]) {
                currentLayer[node] = i;
            }
        }

        // Calculate the longest path (DFS)
        std::function<int(FlowNode*, QSet<FlowNode*>&)> calcLongestPath;
        calcLongestPath = [this, &calcLongestPath](FlowNode* node,
                                                   QSet<FlowNode*>& visited) -> int {
            if (visited.contains(node)) {
                return 0;
            }
            visited.insert(node);
            int maxPath = 0;
            for (auto* edge : m_edges) {
                if (edge->source == node) {
                    maxPath = qMax(maxPath, 1 + calcLongestPath(edge->target, visited));
                }
            }
            visited.remove(node);
            return maxPath;
        };

        // Collect layer 0 node information
        for (auto* node : layers[0]) {
            NodeInfo info;
            info.node = node;
            QSet<FlowNode*> visitedSet;
            info.longestPath = calcLongestPath(node, visitedSet);
            info.successorCount = 0;
            info.minSuccessorLayer = layers.size();
            for (auto* edge : m_edges) {
                if (edge->source == node) {
                    info.successorCount++;
                    if (currentLayer.contains(edge->target)) {
                        info.minSuccessorLayer =
                            qMin(info.minSuccessorLayer, currentLayer[edge->target]);
                    }
                }
            }
            startNodes.append(info);
        }

        // Step 2: Select the master node (layer 0)
        // Priority: Longest path -> Number of successors -> Minimum level of successors
        std::sort(startNodes.begin(), startNodes.end(),
                  [](const NodeInfo& fromNode, const NodeInfo& toNode) {
                      if (fromNode.longestPath != toNode.longestPath) {
                          return fromNode.longestPath > toNode.longestPath;
                      }
                      if (fromNode.successorCount != toNode.successorCount) {
                          return fromNode.successorCount > toNode.successorCount;
                      }
                      return fromNode.minSuccessorLayer < toNode.minSuccessorLayer;
                  });

        // The master node stays at layer 0, and other nodes move down
        FlowNode* mainNode = startNodes[0].node;
        QVector<FlowNode*> nodesToMove;
        for (const auto& info : startNodes) {
            if (info.node != mainNode) {
                nodesToMove.append(info.node);
            }
        }

        // Step 3: Move the node down to the target layer (successive minimum level -1)
        QVector<QVector<FlowNode*>> newLayers;
        newLayers.resize(layers.size());
        QHash<FlowNode*, int> newLayerMap;

        // Initialize a new level
        for (int i = 0; i < layers.size(); ++i) {
            for (auto* node : layers[i]) {
                if (node == mainNode) {
                    newLayers[0].append(node);
                    newLayerMap[node] = 0;
                } else if (!nodesToMove.contains(node)) {
                    newLayers[i].append(node);
                    newLayerMap[node] = i;
                }
            }
        }

        // Move down nodes (consider width constraints: Layer 1 up to 4 nodes)
        int layer1Count = newLayers[1].size();
        const int MAX_LAYER1_NODES = 4;
        for (auto* node : nodesToMove) {
            int minSuccLayer = layers.size();
            for (auto* edge : m_edges) {
                if (edge->source == node && currentLayer.contains(edge->target)) {
                    minSuccLayer = qMin(minSuccLayer, currentLayer[edge->target]);
                }
            }
            int targetLayer = qMax(1, minSuccLayer - 1); // At least layer 1
            if (layer1Count >= MAX_LAYER1_NODES && targetLayer == 1) {
                targetLayer = 2; // Layer 1 is full, move down to Layer 2
            }
            if (targetLayer >= newLayers.size()) {
                newLayers.resize(targetLayer + 1);
            }
            newLayers[targetLayer].append(node);
            newLayerMap[node] = targetLayer;
            if (targetLayer == 1) {
                layer1Count++;
            }
        }

        // Step 4: Verify and adjust subsequent nodes
        // Ensure the dependency is correct: successor level > front-drive level
        bool adjusted;
        do {
            adjusted = false;
            for (int i = 0; i < newLayers.size(); ++i) {
                for (auto* node : newLayers[i]) {
                    for (auto* edge : m_edges) {
                        if (edge->source == node && newLayerMap.contains(edge->target)) {
                            int succLayer = newLayerMap[edge->target];
                            if (succLayer <= i) {
                                // The successor level is too shallow, move to i+1
                                newLayers[succLayer].removeOne(edge->target);
                                int newSuccLayer = i + 1;
                                if (newSuccLayer >= newLayers.size()) {
                                    newLayers.resize(newSuccLayer + 1);
                                }
                                newLayers[newSuccLayer].append(edge->target);
                                newLayerMap[edge->target] = newSuccLayer;
                                adjusted = true;
                            }
                        }
                    }
                }
            }
        } while (adjusted);

        // Step 5: Clean up the empty layer
        layers.clear();
        for (const auto& layer : newLayers) {
            if (!layer.isEmpty()) {
                layers.append(layer);
            }
        }
    }

    std::optional<LayoutDirection> FlowChartModel::layoutDirection() const {
        return m_layoutDirection;
    }

    bool FlowChartModel::isCrossSubgraphEdge(FlowChartModel* sourceGraph,
                                             FlowChartModel* targetGraph) {
        // Cross-subgraph edges: The source and target are not in the same model, or involve the
        // main image
        return (sourceGraph != targetGraph) || (sourceGraph == nullptr) || (targetGraph == nullptr);
    }

    FlowChartModel* FlowChartModel::getRootModel() {
        FlowChartModel* current = this;
        while (current->m_parentModel != nullptr) {
            current = current->m_parentModel;
        }
        return current;
    }

    bool FlowChartModel::wouldCreateCycle(FlowNode* source, FlowNode* target) const {
        if (source == target) {
            return false; // Self-ring allows
        }

        // Collect all relevant models (main and sub-pictures)
        QSet<const FlowChartModel*> models;
        FlowChartModel* root = const_cast<FlowChartModel*>(this)->getRootModel();
        models.insert(root);
        for (auto* sub : root->m_subgraphs.values()) {
            models.insert(sub);
        }
        models.insert(this); // Make sure the current model is included

        QSet<FlowNode*> visiting;
        QSet<FlowNode*> visited;
        QStack<FlowNode*> stack;
        stack.push(target);

        while (!stack.empty()) {
            FlowNode* node = stack.top();
            if (visiting.contains(node)) {
                stack.pop();
                visiting.remove(node);
                visited.insert(node);
                continue;
            }
            if (visited.contains(node)) {
                stack.pop();
                continue;
            }

            visiting.insert(node);
            // Iterate through all models' edges
            for (const FlowChartModel* model : models) {
                for (const FlowEdge* edge : model->m_edges) {
                    if (edge->source == node) {
                        if (edge->target == source) {
#ifdef FLOW_MODEL_DEBUG
                            qDebug() << "Cycle detected: " << edge->target->id << " -> "
                                     << edge->source->id;
#endif
                            return true;
                        }
                        if (!visited.contains(edge->target)) {
                            stack.push(edge->target);
                        }
                    }
                }
            }
        }
        return false;
    }

    void FlowChartModel::topologicalSortDFS(FlowNode* node, QHash<FlowNode*, int>& visited,
                                            QVector<FlowNode*>& sorted) const {
        visited[node] = 1; // Visiting
        for (auto* edge : m_edges) {
            if (edge->source == node) {
                if (visited.value(edge->target, 0) == 0) {
                    topologicalSortDFS(edge->target, visited, sorted);
                } else if (visited[edge->target] == 1) {
                    throw std::runtime_error("Cycle detected");
                }
            }
        }
        visited[node] = 2;    // Access is completed
        sorted.prepend(node); // Add later
    }

    QList<FlowNode*> FlowChartModel::predecessors(FlowNode* node) const {
        QList<FlowNode*> preds;
        for (auto* edge : m_edges) {
            if (edge->target == node) {
                preds.append(edge->source);
            }
        }
        return preds;
    }
} // namespace hbchart