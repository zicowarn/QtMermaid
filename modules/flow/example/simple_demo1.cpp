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

    Q_ASSERT(model->addNode(nullptr, NodeId("A"), NodeLabel("Node A description")));
    Q_ASSERT(model->addNode(nullptr, NodeId("B"), NodeLabel("Node B description")));
    Q_ASSERT(model->addNode(nullptr, NodeId("C"), NodeLabel("Node C description")));
    Q_ASSERT(model->addNode(nullptr, NodeId("D")));
    Q_ASSERT(model->addNode(nullptr, NodeId("E"), NodeLabel("Node E description")));
    Q_ASSERT(model->addNode(nullptr, NodeId("F")));
    Q_ASSERT(model->addNode(nullptr, NodeId("G"), NodeLabel("Node G description")));
    Q_ASSERT(model->addNode(nullptr, NodeId("H")));

    // Add edges
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("B")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B"), nullptr, TargetId("C")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("C"), nullptr, TargetId("D")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("E"), nullptr, TargetId("F")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("F"), nullptr, TargetId("G")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("G"), nullptr, TargetId("H")));
    Q_ASSERT(model->addEdge(nullptr, SourceId("B"), nullptr, TargetId("F"))); // Cross-path edge
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("D"))); // Cross 2 levels
    Q_ASSERT(model->addEdge(nullptr, SourceId("A"), nullptr, TargetId("H"))); // 跨多层

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