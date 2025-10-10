/**
 * File: FlowChartShapeItem.h
 * Description: The declaration of the FlowChartShapeItem class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWCHARTSHAPEITEM_H
#define FLOWCHARTSHAPEITEM_H

#include <QColor>
#include <QFont>
#include <QGraphicsObject>

/**
 * +---------------------------+
 * |                           | <- extendedRect
 * |   +-------------------+   |
 * |   | outer(bounding)   |   |
 * |   |   +-----------+   |   |
 * |   |   | innerRect |   |   |
 * |   |   | "Text"    |   |   |
 * |   |   +-----------+   |   |
 * |   +-------------------+   |
 * |                           |
 * +---------------------------+
 */

namespace hbchart {

    /**
     * @brief FlowChart rounded rectangle shape
     * @details Inherits from QGraphicsObject, used for drawing label of FlowChart
     */
    class FlowChartLabel : public QGraphicsObject {
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
        explicit FlowChartLabel(QString label, QGraphicsItem* parent = nullptr);
        ~FlowChartLabel() override = default; ///<

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

      protected:
        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text
         * @note Called once in constructor
         */
        void sizeForLabel();

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
    };

    /**
     * @brief Flowchart shape
     * @details Inherits from QGraphicsObject, used for drawing flowchart shapes
     */
    class FlowChartShapeItem : public QGraphicsObject {
        Q_OBJECT

      protected:
        QString m_label;      ///< Text
        int m_layer;          ///< Layer
        qreal m_margin;       ///< Margin
        qreal m_safeSpacing;  ///< Safe spacing (not for drawing, only for layout)
        QFont m_font;         ///< Font
        QColor m_fontColor;   ///< Font color
        QColor m_fillColor;   ///< Fill color
        QColor m_borderColor; ///< Border color
        bool m_isVirtual;      ///< Virtual node flag
        int m_inDegree;       ///< In-degree
        int m_outDegree;      ///< Out-degree

      Q_SIGNALS:
        void positionChanged();

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit FlowChartShapeItem(QString label, QGraphicsItem* parent = nullptr);
        ~FlowChartShapeItem() override = default; ///< Destructor

        /**
         * @brief Set text
         * @param label Text
         */
        void setLabel(const QString& label);

        /**
         * @brief Get text
         * @return Text
         */
        [[nodiscard]] QString getLabel() const;

        /**
         * @brief Set layer
         * @param layer Layer
         */
        void setLayer(int layer);

        /**
         * @brief Get layer
         * @return Layer
         */
        [[nodiscard]] int getLayer() const;

        /**
         * @brief Set margin
         * @param margin Margin
         */
        void setMargin(qreal margin);

        /**
         * @brief Get margin
         * @return Margin
         */
        [[nodiscard]] qreal margin() const;

        /**
         * @brief Set safe spacing
         * @param spacing Safe spacing
         */
        void setSafeSpacing(qreal spacing);

        /**
         * @brief Get safe spacing
         * @return Safe spacing
         */
        [[nodiscard]] qreal safeSpacing() const;

        /**
         * @brief Set font
         * @param font Font
         */
        void setFont(const QFont& font);

        /**
         * @brief Get font
         * @return Font
         */
        [[nodiscard]] const QFont& font() const;

        /**
         * @brief Set font color
         * @param color Font color
         */
        void setFontColor(const QColor& color);

        /**
         * @brief Get font color
         * @return Font color
         */
        [[nodiscard]] const QColor& fontColor() const;

        /**
         * @brief Set fill color
         * @param color Fill color
         */
        void setFillColor(const QColor& color);

        /**
         * @brief Get fill color
         * @return Fill color
         */
        [[nodiscard]] const QColor& fillColor() const;

        /**
         * @brief Set border color
         * @param color Border color
         */
        void setBorderColor(const QColor& color);

        /**
         * @brief Get border color
         * @return Border color
         */
        [[nodiscard]] const QColor& borderColor() const;

        /**
         * @brief Set virtual node flag
         * @param isVirtual Virtual node flag
         */
        [[nodiscard]] bool isVirtual() const;

        /**
         * @brief Set in-degree
         * @param inDegree In-degree
         */
        void setInDegree(int inDegree);
        /**
         * @brief Get in-degree
         * @return In-degree
         */
        [[nodiscard]] int getInDegree() const;

        /**
         * @brief Set out-degree
         * @param outDegree Out-degree
         */
        void setOutDegree(int outDegree);

        /**
         * @brief Get out-degree
         * @return Out-degree
         */
        [[nodiscard]] int getOutDegree() const;
        /**
         * @brief Inner rectangle
         * @details Inner rectangle composed of anchor points and margins
         * @return Inner rectangle
         * @note Layer 0, implemented by subclasses, core shape area
         * Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] virtual QRectF innerRect() const = 0;

        /**
         * @brief Outer boundary
         * @return Outer boundary
         * @note Layer 3, outer margin plus one margin
         */
        [[nodiscard]] QRectF outerRect() const;

        /**
         * @brief Extended rectangle
         * @details Boundary with added safe space for layout reference
         * @return Extended rectangle
         * @note Layer 4, includes safeSpacing area for layout reference
         */
        [[nodiscard]] QRectF extendedRect() const;

        /**
         * @brief Bounding rectangle
         * @details Bounding rectangle composed of inner rectangle and margins
         * @return Layer 1, bounding rectangle including margin for drawing
         */
        [[nodiscard]] QRectF boundingRect() const override; ///< Bounding rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] virtual QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                          const QPointF& toPoint) const = 0;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] virtual QPointF getHorizontalEdge(bool isSource) const = 0;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] virtual QPointF getVerticalEdge(bool isSource) const = 0;

      protected:
        /**
         * @brief Calculate label size
         * @details Called once after subclass construction or setLabel() to update internal size
         * (e.g. stored in a QRectF m_innerRect) and update().
         * @return Label size
         */
        [[nodiscard]] QSizeF sizeForLabel() const;

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
    };

    /**
     * @brief Flowchart virtual node shape
     * @details Inherits from FlowChartShapeItem, used for drawing virtual node flowchart shapes
     */
    class VirtualNodeItem : public FlowChartShapeItem {
        Q_OBJECT

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        VirtualNodeItem(const QString& label, QGraphicsItem* parent = nullptr);
        ~VirtualNodeItem() override = default; ///< Destructor

        /**
         * @brief Bounding rectangle
         * @return Bounding rectangle
         * @note Layer 1, bounding rectangle including margin for drawing
         */
        [[nodiscard]] QRectF boundingRect() const override;

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override;

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

      private:
        /**
         * @brief Paint
         * @param painter
         * @param option Style options
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;
    };

    /**
     * @brief Flowchart subgraph shape
     * @details Inherits from FlowChartShapeItem, used for drawing rectangle flowchart shapes
     */
    class SubgraphShapeItemRect : public FlowChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle
        qreal m_width;      ///< Width
        qreal m_height;     ///< Height

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit SubgraphShapeItemRect(const QString& label, QGraphicsItem* parent = nullptr);
        ~SubgraphShapeItemRect() override = default; ///< Destructor

        /**
         *  @brief Get size
         *  @return Size
         */
        [[nodiscard]] QSizeF size() const;

        /**
         * @brief Set size
         * @param size Size
         */
        void setSize(QSizeF size);

        /**
         * @brief Set rectangle
         * @param rect Rectangle
         */
        void setRect(QRectF rect);

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

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
         * @brief Update inner rectangle
         * @details Update inner rectangle size and position based on anchor points and margins
         */
        void updateInnerRect();

        /**
         * @brief Adjust size to fit text
         * @details Adjust inner rectangle size to fit text and update
         * @note Called once in constructor
         */
        void adjustSizeToText();
    };

    /**
     * @brief Flowchart rectangle shape
     * @details Inherits from FlowChartShapeItem, used for drawing rectangle flowchart shapes
     */
    class ShapeItemRect : public FlowChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit ShapeItemRect(const QString& label, QGraphicsItem* parent = nullptr);
        ~ShapeItemRect() override = default; ///< Destructor

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

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
        void adjustSizeToText();
    };

    /**
     * @brief Flowchart rounded rectangle shape
     * @details Inherits from FlowChartShapeItem, used for drawing rounded rectangle flowchart
     * shapes
     */
    class ShapeItemRoundedRect : public FlowChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit ShapeItemRoundedRect(const QString& label, QGraphicsItem* parent = nullptr);
        ~ShapeItemRoundedRect() override = default; ///< Destructor

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

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
        void adjustSizeToText();
    };

    /**
     * @brief Flowchart circle shape
     * @details Inherits from FlowChartShapeItem, used for drawing circle flowchart shapes
     */
    class ShapeItemCircle : public FlowChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit ShapeItemCircle(const QString& label, QGraphicsItem* parent = nullptr);
        ~ShapeItemCircle() override = default; ///< Destructor

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

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
        void adjustSizeToText();
    };

    /**
     * @brief Flowchart diamond shape
     * @details Inherits from FlowChartShapeItem, used for drawing diamond flowchart shapes
     */
    class ShapeItemDiamond : public FlowChartShapeItem {
        Q_OBJECT

      private:
        QRectF m_innerRect; ///< Inner rectangle

      public:
        /**
         * @brief Constructor
         * @param label Text
         * @param parent Parent object
         */
        explicit ShapeItemDiamond(const QString& label, QGraphicsItem* parent = nullptr);
        ~ShapeItemDiamond() override = default; ///< Destructor

        /**
         * @brief Inner rectangle
         * @return Inner rectangle
         * @note Note: Origin at shape center, clockwise as positive direction
         */
        [[nodiscard]] QRectF innerRect() const override; ///< Inner rectangle

        /**
         * @brief Intersection point calculation method
         * @param fromPoint Start point
         * @param toPoint End point
         * @return Intersection point
         * @note Implemented by subclasses
         */
        [[nodiscard]] QPointF getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const override;

        /**
         * @brief Get horizontal boundary
         * @param isSource True if source, false if destination
         * @return point on horizontal boundary
         */
        [[nodiscard]] QPointF getHorizontalEdge(bool isSource) const override;

        /**
         * @brief Get vertical boundary
         * @param isSource True if source, false if destination
         * @return point on vertical boundary
         */
        [[nodiscard]] QPointF getVerticalEdge(bool isSource) const override;

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
        void adjustSizeToText();
    };

} // namespace hbchart

#endif // FLOWCHARTSHAPE_H
