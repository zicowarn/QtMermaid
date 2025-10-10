/**
 * File: simple_demo.cpp
 * Description: A simple demo for GanttChartWidget
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

#include "GanttChartCoordinate.h"
#include "GanttChartLayouter.h"
#include "GanttChartModel.h"
#include "GanttChartScene.h"
#include "GanttChartView.h"

using namespace hbchart;

int main(int argc, char* argv[]) {
    constexpr qreal SCENE_WIDTH = 1000.0;
    constexpr qreal SCENE_HEIGHT = 800.0;

    // Create an application
    QApplication app(argc, argv);
    // Create a window
    auto* window = new QWidget();
    auto* layout = new QVBoxLayout(window);
    constexpr qreal TIME_RATIO = 120.0; // 120 seconds per unit, 2 minutes per unit

    TimeUnitType timeUnit =
        makeTimeUnit(TimeUnit::MINUTE, TIME_RATIO); // 120 seconds per unit, 2 minutes per unit
    auto* model = new GanttChartModel(timeUnit);
    auto* scene = new GanttChartScene(model);
    auto* view = new GanttChartView(scene, window);
    // scene->updateViewScaleFactor(1);
    auto* layouter = new GanttChartLayouter(model, scene);
    auto* coordinate = new GanttChartCoordinate(layouter, scene);

    model->setBaseTime("0h");

    model->setTitle("Simple Gantt Chart Demo");

    // Add Section
    model->addSection(SectionName{"Section 1"});
    // Add a task
    model->addTask(nullptr, TaskName{"Task 1"}, TaskDescription{"Task 1 description"}, TaskStart{"1.2h"}, TaskEndOrDuration{"5.0h"}); // Root task: 0-2Hour
    // GanttTask* task1 = model->getRootTasks()[0];
    model->addTask(nullptr, TaskName{"Task 2"}, TaskDescription{"Task 2 description"}, TaskStart{"1.5h"}, TaskEndOrDuration{"3.5h"}); // Root task：0.5-1.5Hour"Task 2", "Task 2 description", "1.5h", "3.5h"); // Subtask: 0.5-1.5Hour
    // Add Section
    model->addSection(SectionName{"Section 2"});
    // Add a task
    model->addTask(nullptr, TaskName{"Task 3"}, TaskDescription{"Task 3 description"}, TaskStart{"2.0h"}, TaskEndOrDuration{"12.0h"}); // Root task：1-2Hour"Task 3", "Task 3 description", "2.0h", "12.0h"); // Root task：1-2Hour
    // Add subtasks
    GanttTask* task2 = model->getRootTasks()[1];
    //
    model->addTask(task2, TaskName{"Task 4"}, TaskDescription{"Task 4 description"}, TaskStart{"2.5h"}, TaskEndOrDuration{"6.5h"}); // Subtask: 0.5-1.5Hour"Task 4", "Task 4 description", "2.5h", "6.5h"); // Subtask: 0.5-1.5Hour
    // Add milestones
    model->addMilestone(task2, MilestoneName{"Milestone 1"}, MilestoneDescription{""}, MilestoneTime{"5.0h"}); // Milestone: 1Hour"Milestone 1", "", "5.0h"); // Milestone: 1Hour
    // Add Section
    model->addSection(SectionName{"Section 1"});
    // Add a task
    model->addTask(nullptr, TaskName{"Task 5"}, TaskDescription{"Task 5 description"}, TaskStart{"3.0h"}, TaskEndOrDuration{"13.0h"}); // Root task：1-2Hour"Task 5", "Task 5 description", "3.0h", "13.0h"); // Root task：1-2Hour
    // Add a task
    model->addTask(nullptr, TaskName{"Task 6"}, TaskDescription{"Task 6 description"}, TaskStart{"4.0h"}, TaskEndOrDuration{"14.0h"}); // Root task：1-2Hour"Task 5", "Task 5 description", "3.0h", "13.0h"); // Root task：1-
    
    // Update scenarios
    scene->updateScene();
    // Update layout
    layouter->apply();
    // Update coordinates
    coordinate->updateAll();

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
