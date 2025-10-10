/**
 * File: simple_demo.cpp
 * Description: A simple demo for FlowChartWidget
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include <QApplication>
#include "QtMermaidEditor.h"

int main(int argc, char* argv[]) {
  constexpr int WINDOW_WIDTH = 1200;
  constexpr int WINDOW_HEIGHT = 800;
  
  QApplication app(argc, argv);

  auto* gantt = new hbchart::QtMermaidEditor();
  gantt->resize(WINDOW_WIDTH, WINDOW_HEIGHT);
  gantt->show();

  return QApplication::exec();
}
