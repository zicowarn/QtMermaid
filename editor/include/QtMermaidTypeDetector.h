/**
 * File: QtMermaidTypeDetector.h
 * Description: The declaration of the QtMermaidTypeDetector class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef QTMERMAIDTYPEDETECTOR_H
#define QTMERMAIDTYPEDETECTOR_H

#include "Interfaces.h"
namespace hbchart {

  class QtMermaidTypeDetector {
    public:
        /**
         * @brief Constructor.
         */
        QtMermaidTypeDetector() = default;
        ~QtMermaidTypeDetector() = default; ///< Destructor.

        /**
         * @brief Detects the type of the input string.
         * @param input The input string.
         * @return The chart type of the input string.
         */
        static QtMermaidType detectType(const QString& input);
    };
} // namespace hbchart

#endif // QTMERMAIDTYPEDETECTOR_H