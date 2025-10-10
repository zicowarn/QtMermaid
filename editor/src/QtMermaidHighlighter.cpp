/**
 * File: QtMermaidHighlighter.cpp
 * Description: The implementation of the QtMermaidHighlighter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "QtMermaidHighlighter.h"
#include <QDebug>

namespace hbchart {
    QtMermaidHighlighter::QtMermaidHighlighter(QTextDocument* parent) : SyntaxHighlighter(parent) {
        HighlightingRule rule;
        // Keywords: dark blue
        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(Qt::darkBlue);
        keywordFormat.setFontWeight(QFont::Bold);
        QStringList keywordPatterns = {
          "\\bgraph\\b", 
          "\\bflowchart\\b", 
          "\\bgantt\\b",
          "\\bsequence\\b",
          "\\bclassDiagram\\b",
          "\\bstateDiagram\\b",
          "\\bpie\\b",
          "\\bradar-beta\\b",
          "\\bsequenceDiagram\\b"
        };
        for (const QString& pattern : keywordPatterns) {
            rule.pattern = QRegularExpression(pattern);
            rule.format = keywordFormat;
            m_highlightingRules.append(rule);
        }

        // Comments
        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::gray);
        rule.pattern = QRegularExpression("%%.*$");
        rule.format = commentFormat;
        m_highlightingRules.append(rule);

        // Error line
        m_errorFormat.setBackground(Qt::red);
        m_errorFormat.setProperty(QTextFormat::FullWidthSelection, true);
    }

    void QtMermaidHighlighter::setErrorLine(int line) {
        m_errorLine = line;
        bool oldState = document()->blockSignals(true); // Block QTextDocument signal
        rehighlight();
        document()->blockSignals(oldState); // Recovery signal
    }

    void QtMermaidHighlighter::highlightBlock(const QString& text) {
        // Check text length to avoid unnecessary processing
        constexpr int minTextLength = 3;
        if (text.length() < minTextLength) {
            return;
        }
        // Normal text hightlighting
        for (const HighlightingRule& rule : m_highlightingRules) {
            if (!rule.pattern.isValid()) {
                qDebug() << "Invalid pattern:" << rule.pattern;
                qDebug() << "Invalid pattern reason:" << rule.pattern.errorString();
                qDebug() << "Skipping invalid pattern in highlightBlock";
                continue;
            }
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }

        // Error line highlighting
        if (currentBlock().blockNumber() == m_errorLine - 1) {
            qDebug() << "Error line: " << text;
            setFormat(0, text.length(), m_errorFormat);
        }
    }
} // namespace hbchart