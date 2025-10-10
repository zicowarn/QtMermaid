/**
 * File: QtMermaidEditor.h
 * Description: The declaration of the QtMermaidEditor class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef QTMERMAIDEDITOR_H
#define QTMERMAIDEDITOR_H

#include "Interfaces.h"
#include "QtMermaidTextEdit.h"
#include "QtMermaidTypeDetector.h"


#include <QTextEdit>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>

namespace hbchart {

  class QtMermaidEditor : public QWidget {
    Q_OBJECT

    private:
        QtMermaidType m_chartType;
        QtMermaidTextEdit* m_editor;
        QWidget* m_preview;
        QSplitter* m_splitter;
        QVBoxLayout* m_layout;
        std::unique_ptr<Parser> m_parser;
        std::unique_ptr<Renderer> m_renderer;
        std::unique_ptr<SyntaxHighlighter> m_highlighter;
        QtMermaidTypeDetector* m_typeDetector;
        QGraphicsView* m_view = nullptr;

    public:
        QtMermaidEditor(QWidget* parent = nullptr);
        ~QtMermaidEditor() override = default;

    private:
        void updateContent();
    };
} // namespace hbchart

#endif // QTMERMAIDEDITOR_H
