/**
 * File: simple_demo.cpp
 * Description: A simple demo for FlowChartWidget
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
// NOLINTBEGIN

#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>

#include "FlowChartLayouter.h"
#include "FlowChartModel.h"
#include "FlowChartScene.h"
#include "FlowChartShapeItem.h"
#include "FlowChartView.h"
#include "FlowChartConnector.h"

using namespace hbchart;

int main(int argc, char* argv[]) {
    constexpr int SCENE_WIDTH = 1200;
    constexpr int SCENE_HEIGHT = 800;

    QApplication app(argc, argv);
    // Create window
    auto* window = new QWidget();
    auto* layout = new QVBoxLayout(window);
    // Create FlowChart
    auto* model = new FlowChartModel(nullptr);
    auto* scene = new FlowChartScene(model, nullptr);
    auto* view = new FlowChartView(scene, window);
    auto* layouter = new FlowChartLayouter(scene, model);
    auto* connector = new FlowChartConnector(scene, model);

    // model->setTitle("Simple Flow Chart Demo");

    // Set main icon title
    model->setTitle("Complex Multi-Layer Flow Chart Demo");
    model->setLayoutDirection(LayoutDirection::VERTICAL);

    Q_ASSERT(model->addNode(nullptr, NodeId("A"))); // 层 1: Input Data
    Q_ASSERT(model->addNode(nullptr, NodeId("B1"))); // 层 2: Clean Data
    Q_ASSERT(model->addNode(nullptr, NodeId("B2"))); // Normalize
    Q_ASSERT(model->addNode(nullptr, NodeId("B3"))); // Filter Noise
    Q_ASSERT(model->addNode(nullptr, NodeId("C1"))); // 层 3: Feature Extract 1
    Q_ASSERT(model->addNode(nullptr, NodeId("C2"))); // Feature Extract 2
    Q_ASSERT(model->addNode(nullptr, NodeId("C3"))); // Feature Extract 3
    Q_ASSERT(model->addNode(nullptr, NodeId("C4"))); // Feature Extract 4
    Q_ASSERT(model->addNode(nullptr, NodeId("D1"))); // 层 4: Model Train 1
    Q_ASSERT(model->addNode(nullptr, NodeId("D2"))); // Model Train 2
    Q_ASSERT(model->addNode(nullptr, NodeId("D3"))); // Model Train 3
    Q_ASSERT(model->addNode(nullptr, NodeId("D4"))); // Model Train 4
    Q_ASSERT(model->addNode(nullptr, NodeId("E1"))); // 层 5: Evaluate 1
    Q_ASSERT(model->addNode(nullptr, NodeId("E2"))); // Evaluate 2
    Q_ASSERT(model->addNode(nullptr, NodeId("E3"))); // Evaluate 3
    Q_ASSERT(model->addNode(nullptr, NodeId("E4"))); // Evaluate 4
    Q_ASSERT(model->addNode(nullptr, NodeId("F1"))); // 层 6: Optimize 1
    Q_ASSERT(model->addNode(nullptr, NodeId("F2"))); // Optimize 2
    Q_ASSERT(model->addNode(nullptr, NodeId("F3"))); // Optimize 3
    Q_ASSERT(model->addNode(nullptr, NodeId("F4"))); // Optimize 4
    Q_ASSERT(model->addNode(nullptr, NodeId("G1"))); // 层 7: Validate 1
    Q_ASSERT(model->addNode(nullptr, NodeId("G2"))); // Validate 2
    Q_ASSERT(model->addNode(nullptr, NodeId("G3"))); // Validate 3
    Q_ASSERT(model->addNode(nullptr, NodeId("G4"))); // Validate 4
    Q_ASSERT(model->addNode(nullptr, NodeId("H1"))); // 层 8: Output 1
    Q_ASSERT(model->addNode(nullptr, NodeId("H2"))); // Output 2
    Q_ASSERT(model->addNode(nullptr, NodeId("H3"))); // Output 3
    Q_ASSERT(model->addNode(nullptr, NodeId("H4"))); // Output 4
    Q_ASSERT(model->addNode(nullptr, NodeId("I"))); // 层 9: Final Report

    // 添加边
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("B1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("B2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("B3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B1"), nullptr, TargetId("C1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B1"), nullptr, TargetId("C2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B2"), nullptr, TargetId("C3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B3"), nullptr, TargetId("C4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("C1"), nullptr, TargetId("D1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("C2"), nullptr, TargetId("D2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("C3"), nullptr, TargetId("D3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("C4"), nullptr, TargetId("D4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("D1"), nullptr, TargetId("E1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("D2"), nullptr, TargetId("E2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("D3"), nullptr, TargetId("E3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("D4"), nullptr, TargetId("E4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("E1"), nullptr, TargetId("F1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("E2"), nullptr, TargetId("F2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("E3"), nullptr, TargetId("F3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("E4"), nullptr, TargetId("F4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("F1"), nullptr, TargetId("G1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("F2"), nullptr, TargetId("G2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("F3"), nullptr, TargetId("G3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("F4"), nullptr, TargetId("G4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("G1"), nullptr, TargetId("H1")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("G2"), nullptr, TargetId("H2")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("G3"), nullptr, TargetId("H3")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("G4"), nullptr, TargetId("H4")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("H1"), nullptr, TargetId("I")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("H2"), nullptr, TargetId("I")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("H3"), nullptr, TargetId("I")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("H4"), nullptr, TargetId("I")));
    // 长边
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("I"))); // A -> I
    Q_ASSERT(model->addEdge(nullptr, SourceId("B1"), nullptr, TargetId("F1"))); // B1 -> F1

    // Check the level
    auto layers = model->calculateLayers();
    qDebug() << "Main graph layers:" << layers.size();
    for (int i = 0; i < layers.size(); ++i) {
        QStringList node_ids;
        for (auto* node : layers[i]) {
            node_ids << node->id;
        }
        qDebug() << "Layer" << i << ":" << node_ids.join(", ");
    }

    // 调试输出
    qDebug() << "-----------------------------------------";
    qDebug() << "Root nodes:" << model->nodes().size();
    qDebug() << "Main graph edges:";
    for (auto* edge : model->edges()) {
        qDebug() << "Edge:" << edge->source->id << "->" << edge->target->id;
    }

    // 更新场景和布局
    scene->updateScene();
    layouter->apply();
    connector->updateAll();

    // 验证节点位置
    qDebug() << "Node positions after layout:";
    for (auto* node : model->nodes()) {
        if (auto* item = scene->getRegularNodeItem(node)) {
            qDebug() << "Node" << node->id << "pos:" << item->pos();
        }
        if (auto* item = scene->getSubgraphNodeItem(node)) {
            qDebug() << "Subgraph" << node->id << "pos:" << item->pos();
        }
    }

    // 验证场景
    QRectF innerRect = scene->chartInnerRect();
    QRectF outerRect = scene->chartOuterRect();
    qDebug() << "InnerRect:" << innerRect << "OuterRect:" << outerRect;
    Q_ASSERT(!innerRect.isEmpty());
    Q_ASSERT(outerRect.contains(innerRect));

    // Disable clang-tidy without warning
    QTransform tfm = view->transform();
    [[maybe_unused]] qreal scaleX = tfm.m11();
    [[maybe_unused]] qreal scaleY = tfm.m22();

    // Adjust the view
    layout->addWidget(view);
    window->setLayout(layout);

    window->resize(SCENE_WIDTH, SCENE_HEIGHT);
    window->show();
    // Create a scene
    QCoreApplication::setApplicationName("Simple Demo");
    QCoreApplication::exec();

    return 0;
}

// NOLINTEND