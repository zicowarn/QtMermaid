/**
 * File: QtMermaidTypeDetector.cpp
 * Description: The implementation of the QtMermaidTypeDetector class.
 * 
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "QtMermaidTypeDetector.h"

namespace hbchart {
    QtMermaidType QtMermaidTypeDetector::detectType(const QString& input) {
        if (!input.isEmpty()) {
            QString trimmed = input.trimmed();
            /*
            if (trimmed.startsWith("graph")) return QtMermaidType::Flow;
            if (trimmed.startsWith("gantt")) return QtMermaidType::Gantt;
            */
        }
        return QtMermaidType::NONE;
    }
} // namespace hbchart
