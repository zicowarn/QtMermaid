/**
 * File: FlowHighlighter.h
 * Description: The declaration of the FlowHighlighter class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWHIGHLIGHTER_H
#define FLOWHIGHLIGHTER_H

#include <QRegularExpression>
#include <QTextCharFormat>
#include <QTextDocument>
#include <QVector>

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     * @brief The FlowHighlighter class. ihnerits from SyntaxHighlighter.
     */
    class FlowHighlighter : public SyntaxHighlighter {
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
         * @brief FlowHighlighter constructor
         */
        FlowHighlighter(QTextDocument* parent);
        ~FlowHighlighter() override = default;  ///< Destructor

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
    REGISTER_HIGHLIGHTER(FLOW, FlowHighlighter)
} // namespace hbchart

#endif // FLOWHIGHLIGHTER_H