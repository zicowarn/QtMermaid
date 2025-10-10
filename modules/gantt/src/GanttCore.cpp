/**
 * File: FlowCore.cpp
 * Description: The implementation of the FlowCore class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "GanttCore.h"

namespace hbchart {
  /// --- GanttCore ---
  GanttCore::GanttCore() {
    int dummy = 1;
  }

  GanttCore::~GanttCore() {
    int dummy = 1;
  }

  QGraphicsScene* GanttCore::getScene() {
    return m_scene.get();
  }

  QGraphicsView* GanttCore::getView() {
    return m_view.get();
  }

  void GanttCore::setData(const ChartGraph& graph) {
  }

  void GanttCore::updateScene() {
  }

  void GanttCore::applyLayout() {
  }

  void GanttCore::drawConnectors() {
  }

  void GanttCore::updateCoordinate() {
  }
}