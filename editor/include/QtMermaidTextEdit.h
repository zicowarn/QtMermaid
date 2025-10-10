/**
 * File: QtMermaidTextEdit.h
 * Description: The declaration of the QtMermaidTextEdit class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/07
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef QTMERMAIDTEXTEDIT_H
#define QTMERMAIDTEXTEDIT_H

#include <QPlainTextEdit>
#include <QSize>
#include <QWidget>
namespace hbchart {
    // Forward declaration
    class QtMermaidTextEdit;
    class QtMermaidLineNumArea;

    /**
     * @brief Flowchart text editor
     * @details Inherits from QPlainTextEdit, used for displaying mermaid text
     * support syntax highlighting, auto-completion und auto syntax parsing.
     */
    class QtMermaidTextEdit : public QPlainTextEdit {
        Q_OBJECT

      private:
        QtMermaidLineNumArea* m_lineNumberArea = nullptr; ///< Line number area

      public:
        /**
         * @brief Constructor
         * @param parent Parent widget
         */
        explicit QtMermaidTextEdit(QWidget* parent = nullptr);
        ~QtMermaidTextEdit() override = default; ///< Destructor

        /**
         * @brief Event handling function to draw line number area
         * @param event Point to the event object of QPaintEvent, providing information about the
         * drawing area
         * @details Triggered when the paintEvent of LineNumberArea is called and is used to draw
         * the contents of the line number area.
         */
        void onLineNumberAreaPaintEvent(QPaintEvent* event);

        /**
         * @brief Calculate the width of the line number area
         * @return Returns the pixel width of the line number area
         * @details Calculate the width required for the line number area based on the current
         * number of rows and font size
         */
        int getLineNumberAreaWidth();

      protected:
        /**
         * @brief The keyboard presses the event handler function
         * @details Fired when the keyPressEvent of CustomPythonEditor is called and is used to
         * handle keypress events.
         */
        void keyPressEvent(QKeyEvent* event) override;

        /**
         * @brief Rewrite virtual functions to handle size change events
         * @param event Point to the event object of QResizeEvent, providing window size information
         */
        void resizeEvent(QResizeEvent* event) override;

      private slots:
        /**
         * @brief Update the width of the line number area
         * @param newBlockCount New block count
         */
        void onUpdateLineNumberAreaWidth(int newBlockCount);

        /**
         * @brief Highlight the current line
         * @details When the Cursor position changes, call the function to highlight the line number
         * of the current line
         */
        void onHighlightCurrentLine();

        /**
         * @brief Update line number area
         * @param rect Area rectangle
         * @param dy Vertical offset
         * @details When QtMermaidTextEdit
         When the content of *changes, that is, when the updateRequest signal is issued, the
         function is called to update the display of the line number area.
         */
        void onUpdateLineNumberArea(const QRect& rect, int deletY);
    };

    /**
     *@brief QtMermaidLineNumArea class, used to display line numbers
     *@details inherits from the QWidget class and is used to display line numbers
     *@note Used to display line numbers.
     *@see QWidget
     */
    class QtMermaidLineNumArea : public QWidget {
        Q_OBJECT

      private:
        QtMermaidTextEdit* m_CodeEditor; ///< Pointer to FlowChartTextEditor

      public:
        /**
         *@brief QtMermaidLineNumArea constructor
         *@param editor Pointer to FlowChartTextEditor to obtain line number information
         *@details constructor, used to initialize the display of line number areas
         */
        QtMermaidLineNumArea(QtMermaidTextEdit* editor);
        ~QtMermaidLineNumArea() override = default; ///< Destructor

        /**
         *@brief sizeHint Rewrites the virtual function to obtain the size information of the line
         *number area
         *@return Return the dimension information of the line number area
         */
        [[nodiscard]] QSize sizeHint() const override;

      protected:
        /**
         *@brief paintEvent Rewrite the virtual function to draw the contents of the line number
         *area
         *@param event pointing to the event object of QPaintEvent, providing information about the
         *drawing area
         */
        void paintEvent(QPaintEvent* event) override;
    };
} // namespace hbchart

#endif // QTMERMAIDTEXTEDIT_H