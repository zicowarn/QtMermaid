/**
 * File: QtMermaidHighlighter.h
 * Description: The declaration of the QtMermaidHighlighter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */
#ifndef QTMERMAIDHIGHLIGHTER_H
#define QTMERMAIDHIGHLIGHTER_H

#include <QRegularExpression>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QVector>

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     *@brief QtMermaidHighlighter class. Inherits from SyntaxHighlighter.
     */
    class QtMermaidHighlighter : public SyntaxHighlighter {
        Q_OBJECT

      private:
        /**
         *@brief HighlightingRule structure, used to define syntax highlighting rules
         *@details contains two member variables: QRegularExpression and QTextCharFormat
         */
        struct HighlightingRule {
            QRegularExpression pattern; ///< Regular expression
            QTextCharFormat format;     ///< Format
        };
        QVector<HighlightingRule> m_highlightingRules; ///< Syntax highlighting rules collection
        int m_errorLine = -1;                          ///< Error line number
        QTextCharFormat m_errorFormat;                 ///< Error line format

      public:
        /**
         *@brief QtMermaidHighlighter constructor
         */
        QtMermaidHighlighter(QTextDocument* parent);
        ~QtMermaidHighlighter() override = default; ///< Destructor

        /**
         * @brief Set error line
         * @param line Error line number
         */
        void setErrorLine(int line) override;

      protected:
        /**
         * @brief highlightBlock Rewrites virtual function to syntax highlighting of the current
         *block
         */
        void highlightBlock(const QString& text) override;
    };
    REGISTER_HIGHLIGHTER(NONE, QtMermaidHighlighter)
} // namespace hbchart

#endif // QTMERMAIDHIGHLIGHTER_H