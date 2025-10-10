/**
 * File: QtMermaidTextEdit.cpp
 * Description: The implementation of the QtMermaidTextEdit class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "QtMermaidTextEdit.h"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>

namespace hbchart {
    /// --- QtMermaidTextEdit ---
    QtMermaidTextEdit::QtMermaidTextEdit(QWidget* parent) : QPlainTextEdit(parent) {
        // Set the width
        constexpr int FONT_SIZE = 12;
        setFont(QFont("Courier New", FONT_SIZE));
        // Set eye protection mode: light yellow background, dark gray text
        QPalette palette = this->palette();
        constexpr QColor BACKGROUND_COLOR = QColor(245, 245, 220);
        palette.setColor(QPalette::Base, BACKGROUND_COLOR); // Beige
        constexpr QColor TEXT_COLOR = QColor(50, 50, 50);
        palette.setColor(QPalette::Text, TEXT_COLOR); // DarkGray
        setPalette(palette);

        // Set default options
        QTextOption option = document()->defaultTextOption();
        option.setFlags(option.flags() | QTextOption::ShowTabsAndSpaces);
        document()->setDefaultTextOption(option);

        // Initialize the line number part
        m_lineNumberArea = new QtMermaidLineNumArea(this);

        // Connect signals and slots
        connect(this, &QtMermaidTextEdit::blockCountChanged, this,
                &QtMermaidTextEdit::onUpdateLineNumberAreaWidth);
        connect(this, &QtMermaidTextEdit::updateRequest, this,
                &QtMermaidTextEdit::onUpdateLineNumberArea);
        connect(this, &QtMermaidTextEdit::cursorPositionChanged, this,
                &QtMermaidTextEdit::onHighlightCurrentLine);

        // Set initial line number area width
        onUpdateLineNumberAreaWidth(0);
        onHighlightCurrentLine();
    }

    void QtMermaidTextEdit::onLineNumberAreaPaintEvent(QPaintEvent* event) {
        QPainter painter(m_lineNumberArea);
        painter.fillRect(event->rect(), Qt::lightGray);

        QTextBlock block = firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
        int bottom = top + qRound(blockBoundingRect(block).height());

        while (block.isValid() && top <= event->rect().bottom()) {
            if (block.isVisible() && bottom >= event->rect().top()) {
                QString number = QString::number(blockNumber + 1);
                painter.setPen(Qt::black);
                painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
                                 Qt::AlignRight, number);
            }

            block = block.next();
            top = bottom;
            bottom = top + qRound(blockBoundingRect(block).height());
            ++blockNumber;
        }
    }

    int QtMermaidTextEdit::getLineNumberAreaWidth() {
        int digits = 1;
        int max = qMax(1, blockCount());
        constexpr int MINIMUM_TEXT_LENGTH = 10;
        while (max >= MINIMUM_TEXT_LENGTH) {
            max /= MINIMUM_TEXT_LENGTH;
            ++digits;
        }

        int space = 3 + (fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits);

        return space;
    }

    void QtMermaidTextEdit::keyPressEvent(QKeyEvent* event) {
        if (event->key() == Qt::Key_Tab) {
            // Insert 2 spaces instead of Tab
            insertPlainText("  "); // 2 spaces
            return;                // Block default Tab behavior
        }
        QPlainTextEdit::keyPressEvent(event); // Leave other keys to the base class to process
    }

    void QtMermaidTextEdit::resizeEvent(QResizeEvent* event) {
        QPlainTextEdit::resizeEvent(event);

        QRect contRect = contentsRect();
        m_lineNumberArea->setGeometry(
            QRect(contRect.left(), contRect.top(), getLineNumberAreaWidth(), contRect.height()));
    }

    void QtMermaidTextEdit::onUpdateLineNumberAreaWidth(int /* newBlockCount */) {
        setViewportMargins(getLineNumberAreaWidth(), 0, 0, 0);
    }

    void QtMermaidTextEdit::onHighlightCurrentLine() {
        QList<QTextEdit::ExtraSelection> extraSelections;

        if (!isReadOnly()) {
            QTextEdit::ExtraSelection selection;
            constexpr int COLOR_ALPHA = 160;
            QColor lineColor = QColor(Qt::yellow).lighter(COLOR_ALPHA);

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            selection.cursor = textCursor();
            selection.cursor.clearSelection();
            extraSelections.append(selection);
        }

        setExtraSelections(extraSelections);
    }

    void QtMermaidTextEdit::onUpdateLineNumberArea(const QRect& rect, int deletY) {
        if (deletY != 0) {
            m_lineNumberArea->scroll(0, deletY);
        } else {
            m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
        }

        if (rect.contains(viewport()->rect())) {
            onUpdateLineNumberAreaWidth(0);
        }
    }

    // --- QtMermaidLineNumArea  ---
    QtMermaidLineNumArea::QtMermaidLineNumArea(QtMermaidTextEdit* editor)
        : QWidget(editor), m_CodeEditor(editor) {}

    QSize QtMermaidLineNumArea::sizeHint() const {
        return {m_CodeEditor->getLineNumberAreaWidth(), 0};
    }

    void QtMermaidLineNumArea::paintEvent(QPaintEvent* event) {
        m_CodeEditor->onLineNumberAreaPaintEvent(event);
    }
} // namespace hbchart