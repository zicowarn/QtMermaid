/**
 * File: QtMermaidEditor.cpp
 * Description: The implementation of the QtMermaidEditor class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */

#include "QtMermaidEditor.h"
#include "RegistryInitializer.h"
#include <QLabel>
#include <QDebug>

// #define QTMERMAID_DEBUG

namespace hbchart {
    QtMermaidEditor::QtMermaidEditor(QWidget* parent) : QWidget(parent) {
        // Initialize registries
        initializeRegistries();
        // Define const
        constexpr int totalWidth = 1000; // Assume the initial total width
        constexpr int lefSpan = 3;
        constexpr int rightSpan = 7;
        constexpr int totalSpan = 10;
        m_chartType = QtMermaidType::NONE;
        m_typeDetector = new QtMermaidTypeDetector();
        m_editor = new QtMermaidTextEdit(this);
        m_splitter = new QSplitter(Qt::Horizontal);
        m_splitter->addWidget(m_editor);
        m_preview = new QWidget(this);
        m_preview->setStyleSheet("background-color: white;");
        // add "Welcome to QtMermaid" text in center preview, font size 20 and blod，vertical center alignment
        auto* welcomeText = new QLabel(m_preview);
        welcomeText->setStyleSheet("font-size: 20px; font-weight: bold;");
        welcomeText->setText("Welcome to QtMermaid!");
        welcomeText->setAlignment(Qt::AlignCenter);
        // Set layout for preview
        auto* previewLayout = new QVBoxLayout();
        previewLayout->addStretch(1);
        previewLayout->addWidget(welcomeText);
        previewLayout->addStretch(1);
        // Set layout for editor and preview
        m_preview->setLayout(previewLayout);
        // Set layout for splitter
        m_splitter->addWidget(m_preview);
        // Key modification: Set the stretching factor (proportion)
        m_splitter->setSizes({lefSpan * totalWidth / totalSpan,
                              rightSpan * totalWidth / totalSpan}); // Initial pixel ratio
        m_splitter->setStretchFactor(0, lefSpan);   // The first component accounts for 3
        m_splitter->setStretchFactor(1, rightSpan); // The second component accounts for 7
        m_layout = new QVBoxLayout(this);
        m_layout->addWidget(m_splitter);
        connect(m_editor, &QtMermaidTextEdit::textChanged, this, &QtMermaidEditor::updateContent);

        // Apply default highlighter
        m_highlighter = HighlighterFactory::create(m_chartType, m_editor->document());
#ifdef QTMERMAID_DEBUG
        qDebug() << "CoreFactoryRegistry size:" << hbchart::CoreFactoryRegistry.size();
        qDebug() << "RendererFactoryRegistry size:" << hbchart::RendererFactoryRegistry.size();
        qDebug() << "ParserFactoryRegistry size:" << hbchart::ParserFactoryRegistry.size();
        qDebug() << "HighlighterFactoryRegistry size:"
                 << hbchart::HighlighterFactoryRegistry.size();
#endif
    }

    void QtMermaidEditor::updateContent() {
        QString input = m_editor->toPlainText();
        // QtMermaidType type = m_typeDetector->detectType(input);
        QtMermaidType type = QtMermaidType::FLOW;
        if (type == QtMermaidType::NONE) {
            if (m_highlighter == nullptr && m_chartType!= type) {
              m_highlighter = HighlighterFactory::create(QtMermaidType::NONE, m_editor->document());
            }
            return;
        }

        if (m_view != nullptr) {
            m_splitter->replaceWidget(1, m_preview);
            delete m_view;
            m_view = nullptr;
        }

        m_parser = ParserFactory::create(type);
        m_renderer = RendererFactory::create(type);
        if (m_parser == nullptr || m_renderer == nullptr || m_highlighter == nullptr) {
            qDebug() << "Factory creation failed:"
                     << "Parser:" << (m_parser == nullptr) << "Renderer:" << (m_renderer == nullptr)
                     << "Highlighter:" << (m_highlighter == nullptr);
            m_highlighter = HighlighterFactory::create(QtMermaidType::NONE);
            return;
        }

        // Parse and check for errors
        int errorLine = 0;
        /*
        std::unique_ptr<ChartGraph> graph(m_parser->parse(input, &errorLine));
        if (!graph && errorLine > 0) {
            m_highlighter->setErrorLine(errorLine);
        } else {
            m_highlighter->setErrorLine(-1); // Clear error
        }
     

        m_view = m_renderer->getView();
        m_splitter->addWidget(m_view);
        if (graph) {
            m_renderer->render(*graph);
        }       */
    }

} // namespace hbchart
