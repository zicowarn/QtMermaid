/**
 * File: GanttChartShapeItem.h
 * Description: The declaration of the GanttChartShapeItem class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTCHARTSHAPEITEM_H
#define GANTTCHARTSHAPEITEM_H

#include <QColor>
#include <QFont>
#include <QGraphicsObject>

#include "GanttChartModel.h"

/**
 * +-------------------+
 * | outer(bounding)   |
 * |   +-----------+   |
 * |   | innerRect |   |
 * |   | "Text"    |   |
 * |   +-----------+   |
 * +-------------------+
 */

namespace hbchart {

    using RowWidth = StrongType<struct WidthTag, qreal>;
    using RowHeight = StrongType<struct HeightTag, qreal>;

    /**
     * @brief GanttChart rounded rectangle shape
     * @details Inherits from QGraphicsObject, used for drawing label of GanttChart
     */
    class GanttChartLabel : public QGraphicsObject {
        Q_OBJECT

      private:
        QRectF m_innerRect;   ///< Inner rectangle
        qreal m_width;        ///< Width
        qreal m_height;       ///< Height
        qreal m_margin;       ///< Margin
        QString m_label;      ///< Text
        QFont m_font;         ///< Font
        QColor m_fontColor;   ///< Font color
        QColor m_fillColor;   ///< Fill color
        QColor m_borderColor; ///< Border color

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit GanttChartLabel(QString label, QGraphicsItem* parent = nullptr);
        ~GanttChartLabel() override = default; ///<

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const; ///< Inner rectangle

        /**
         * @brief Bounding rectangle
         * @return Bounding rectangle
         * @note Layer 1, bounding rectangle including margin for drawing
         */
        [[nodiscard]] QRectF boundingRect() const override; ///< Bounding rectangle

        /**
         * @brief Get width
         * @return Width
         */
        [[nodiscard]] qreal width() const;

        /**
         * @brief Get heigth
         * @return Height
         */
        [[nodiscard]] qreal height() const;

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] QString getLabel() const;

      private:
        /**
         * @brief Paint
         * @param painter Painter
         * @param option Style options
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text
         * @note Called once in constructor
         */
        void updateInnerRect();

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text
         * @note Called once in constructor
         */
        void sizeForLabel();
    };

    /**
     * @brief GanttChart shape
     * @details Inherits from QGraphicsObject, used for drawing GanttChart shapes
     */
    class GanttChartShapeItem : public QGraphicsObject {
        Q_OBJECT

      protected:
        qreal m_width;      ///< Row Width
        qreal m_rowHeight; ///< Row height
        QString m_label;       ///< Text
        qreal m_margin;        ///< Margin
        QFont m_font;          ///< Font
        QColor m_fontColor;    ///< Font color
        QColor m_fillColor;    ///< Fill color
        QColor m_borderColor;  ///< Border color
        qreal m_pendingDeltaX; ///< Pending delta X
        qreal m_minX;          ///< Minimum X
        qreal m_maxX;          ///< Maximum X

      Q_SIGNALS:
        /**
         * @brief Position changed signal
         * @details Emitted when position changes (e.g. when moved or resized)
         */
        void eventPositionChanged(qreal deltaX);

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit GanttChartShapeItem(QGraphicsItem* parent = nullptr);
        ~GanttChartShapeItem() override = default; ///< Destructor

        /**
         * @brief Set X limits
         * @param minX Minimum X
         * @param maxX Maximum X
         */
        virtual void setXLimits(qreal minX, qreal maxX);

        /**
         * @brief Get height
         * @return Height
         */
        [[nodiscard]] qreal height() const;

        /**
         * @brief Get width
         * @return Width
         */
        [[nodiscard]] virtual qreal width() const = 0;

        /**
         * @brief Set Rect
         * @param width Width
         * @param height Height
         */
        void setRect(RowWidth width, RowHeight height);

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] virtual QString getLabel() const = 0;

        /**
         * @brief Set margin
         * @param margin Margin
         */
        void setMargin(qreal margin) { m_margin = margin; }

        /**
         * @brief Get margin
         * @return Margin
         */
        [[nodiscard]] qreal margin() const { return m_margin; }

        /**
         * @brief Set font
         * @param font Font
         */
        void setFont(const QFont& font) {
            m_font = font;
        }

        /**
         * @brief Get font
         * @return Font
         */
        [[nodiscard]] const QFont& font() const { return m_font; }

        /**
         * @brief Set font color
         * @param color Font color
         */
        void setFontColor(const QColor& color) {
            m_fontColor = color;
        }

        /**
         * @brief Get font color
         * @return Font color
         */
        [[nodiscard]] const QColor& fontColor() const { return m_fontColor; }

        /**
         * @brief Set fill color
         * @param color Fill color
         */
        void setFillColor(const QColor& color) {
            m_fillColor = color;
        }

        /**
         * @brief Get fill color
         * @return Fill color
         */
        [[nodiscard]] const QColor& fillColor() const { return m_fillColor; }

        /**
         * @brief Set border color
         * @param color Border color
         */
        void setBorderColor(const QColor& color) {
            m_borderColor = color;
        }

        /**
         * @brief Get border color
         * @return Border color
         */
        [[nodiscard]] const QColor& borderColor() const { return m_borderColor; }

        /**
         * @brief Inner rectangle
         * @details Inner rectangle composed of anchor points and margins
         * @return Inner rectangle
         * @note Layer 0, implemented by subclasses, core shape area
         * Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] virtual QRectF innerRect() const = 0;

        /**
         * @brief Bounding rectangle
         * @details Bounding rectangle composed of inner rectangle and margins
         * @return Layer 1, bounding rectangle including margin for drawing
         */
        [[nodiscard]] QRectF boundingRect() const override; ///< Bounding rectangle

      protected:
        /**
         * @brief Mouse press event
         * @param event Mouse event
         */
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

        /**
         * @brief Mouse move event
         * @param event Mouse event
         */
        void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

        /**
         * @brief Mouse release event
         * @param event Mouse event
         */
        void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

      private:
        /**
         * @brief Paint
         * @param painter Painter
         * @param option Style options
         * @param widget Widget
         * @note Implemented by subclasses
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                           QWidget* widget) override = 0;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        virtual void updateInnerRect() = 0;
    };

    /**
     * @brief GanttChart half-transparent rectangle shape
     * @details Inherits from GanttChartShapeItem, used for drawing rectangle GanttChart as section
     * shapes
     */
    class GanttChartItemSection : public GanttChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle
        const GanttSection* m_section;

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit GanttChartItemSection(const GanttSection* section,
                                       QGraphicsItem* parent = nullptr);
        ~GanttChartItemSection() override = default; ///< Destructor

        /**
         * @brief Get width
         * @return Width
         */
        [[nodiscard]] qreal width() const override;

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] QString getLabel() const override;

      private:
        /**
         * @brief Paint
         * @param painter Painter
         * @param option Style options
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        void updateInnerRect() override;
    };

    /**
     * @brief GanttChart rounded rectangle shape
     * @details Inherits from GanttChartShapeItem, used for drawing rounded rectangle GanttChart
     * shapes
     */
    class GanttChartItemTask : public GanttChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle
        const GanttTask* m_task;

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit GanttChartItemTask(const GanttTask* task, QGraphicsItem* parent = nullptr);
        ~GanttChartItemTask() override = default; ///< Destructor

        /**
         * @brief Get width
         * @return Width
         */
        [[nodiscard]] qreal width() const override;

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] QString getLabel() const override;

      private:
        /**
         * @brief Paint
         * @param painter Painter
         * @param option Style options
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        void updateInnerRect() override;
    };

    /**
     * @brief GanttChart diamond shape
     * @details Inherits from GanttChartShapeItem, used for drawing diamond GanttChart shapes
     */
    class GanttChartItemMilestone : public GanttChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle
        const GanttMilestone* m_milestone;

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit GanttChartItemMilestone(const GanttMilestone* milestone,
                                         QGraphicsItem* parent = nullptr);
        ~GanttChartItemMilestone() override = default; ///< Destructor

        /**
         * @brief Set X limits
         * @param minX Minimum X
         * @param maxX Maximum X
         */
        void setXLimits(qreal minX, qreal maxX) override;

        /**
         * @brief Get width
         * @return Width
         */
        [[nodiscard]] qreal width() const override;

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Bounding rectangle
         * @details Bounding rectangle composed of inner rectangle and margins
         * @return Layer 1, bounding rectangle including margin for drawing
         */
        [[nodiscard]] QRectF boundingRect() const override;

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] QString getLabel() const override;

      private:
        /**
         * @brief Paint
         * @param painter Painter
         * @param option Style options
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        [[nodiscard]] QSizeF sizeForLabel() const;

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        void updateInnerRect() override;
    };

} // namespace hbchart

#endif // GANTTCHARTSHAPEITEM_H
