/**
 * File: FlowChartShapeItem.cpp
 * Description: The implementation of the FlowChartShapeItem class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "FlowChartShapeItem.h"

#include <QFont>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <qDebug>

// #define FLOWCHART_SHAPE_DEBUG

namespace hbchart {

    /// --- FlowChartLabel ---
    FlowChartLabel::FlowChartLabel(QString label, QGraphicsItem* parent)
        : m_label(std::move(label)), QGraphicsObject(parent) {
        constexpr qreal DEFAULT_MARGIN = 5.0;
        constexpr int DEFAULT_FONT_SIZE = 14;
        constexpr QColor DEFAULT_FILL_COLOR = QColor(236, 236, 255);
        constexpr QColor DEFAULT_BORDER_COLOR = QColor(171, 145, 227);
        m_width = 0.0;
        m_height = 0.0;
        m_margin = DEFAULT_MARGIN;
        m_fontColor = Qt::black;
        m_fillColor = DEFAULT_FILL_COLOR;
        m_borderColor = DEFAULT_BORDER_COLOR;
        m_font = QFont();
        m_font.setBold(true);
        int fontSize = DEFAULT_FONT_SIZE;
        m_font.setPointSize(fontSize);
        setZValue(1); // behind axis lines
        setFlag(QGraphicsItem::ItemIsMovable, false);
#ifdef FLOWCHART_SHAPE_DEBUG
        setFlag(QGraphicsItem::ItemIsMovable, true);
#endif
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        updateInnerRect();
    }

    QRectF FlowChartLabel::innerRect() const { return m_innerRect; }

    QRectF FlowChartLabel::boundingRect() const {
        // Derived classes provide innerRect()
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        QRectF inner = innerRect();
        return {inner.x() - m_margin, inner.y() - m_margin,
                inner.width() + (SIZE_FACTOR_DOUBLE * m_margin),
                inner.height() + (SIZE_FACTOR_DOUBLE * m_margin)};
    }

    qreal FlowChartLabel::width() const { return m_width; }

    qreal FlowChartLabel::height() const { return m_height; }

    QString FlowChartLabel::getLabel() const { return m_label; }

    void FlowChartLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                               QWidget* widget) {
        QRectF rect = innerRect();
        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, 0, 0);
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignHCenter, getLabel());
    }

    void FlowChartLabel::updateInnerRect() {
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        sizeForLabel();
        m_innerRect = QRectF(
            (-m_width / SIZE_FACTOR_DOUBLE) - m_margin, (-m_height / SIZE_FACTOR_DOUBLE) - m_margin,
            m_width + (SIZE_FACTOR_DOUBLE * m_margin), m_height + (SIZE_FACTOR_DOUBLE * m_margin));
        prepareGeometryChange();
        update();
    }

    void FlowChartLabel::sizeForLabel() {
        // Return default size if text is empty
        if (getLabel().isEmpty()) {
            return;
        }
        QFontMetricsF fmt(m_font);
        QRectF textRect = fmt.boundingRect(getLabel());
        m_width = textRect.width();
        m_height = textRect.height();
    }

    /// --- FlowChartShapeItem ---
    FlowChartShapeItem::FlowChartShapeItem(QString label, QGraphicsItem* parent)
        : m_label(std::move(label)), QGraphicsObject(parent) {
        setFlag(QGraphicsItem::ItemIsMovable, false);
#ifdef FLOWCHART_SHAPE_DEBUG
        setFlag(QGraphicsItem::ItemIsMovable, true);
#endif
        setFlag(QGraphicsItem::ItemIsSelectable);
        const qreal DEFAULT_MARGIN = 5.0;
        const qreal DEFAULT_SAFE_SPACING = 10.0;
        m_margin = DEFAULT_MARGIN;
        m_safeSpacing = DEFAULT_SAFE_SPACING;
        m_font = QFont();
        m_fontColor = Qt::black;
        constexpr QColor DEFAULT_FILL_COLOR = QColor(236, 236, 255);
        constexpr QColor DEFAULT_BORDER_COLOR = QColor(171, 145, 227);
        m_fillColor = DEFAULT_FILL_COLOR;
        m_borderColor = DEFAULT_FILL_COLOR;
        m_isVirtual = false;
        m_inDegree = 0;
        m_outDegree = 0;
        //
        connect(this, &QGraphicsObject::xChanged, this, [=]() { emit positionChanged(); });
        connect(this, &QGraphicsObject::yChanged, this, [=]() { emit positionChanged(); });
    }

    void FlowChartShapeItem::setLabel(const QString& label) {
        m_label = label;
    }

    QString FlowChartShapeItem::getLabel() const { return m_label; }

    void FlowChartShapeItem::setLayer(int layer) { m_layer = layer; }

    int FlowChartShapeItem::getLayer() const { return m_layer; }

    void FlowChartShapeItem::setMargin(qreal margin) { m_margin = margin; }

    qreal FlowChartShapeItem::margin() const { return m_margin; }

    void FlowChartShapeItem::setSafeSpacing(qreal spacing) { m_safeSpacing = spacing; }

    qreal FlowChartShapeItem::safeSpacing() const { return m_safeSpacing; }

    void FlowChartShapeItem::setFont(const QFont& font) { 
        m_font = font; 
        update();
    }

    const QFont& FlowChartShapeItem::font() const { return m_font; }

    void FlowChartShapeItem::setFontColor(const QColor& color) {
        m_fontColor = color;
        update();
    }

    const QColor& FlowChartShapeItem::fontColor() const { return m_fontColor; }

    void FlowChartShapeItem::setFillColor(const QColor& color) {
        m_fillColor = color;
        update();
    }

    const QColor& FlowChartShapeItem::fillColor() const { return m_fillColor; }

    void FlowChartShapeItem::setBorderColor(const QColor& color) {
        m_borderColor = color;
        update();
    }

    const QColor& FlowChartShapeItem::borderColor() const { return m_borderColor; }

    bool FlowChartShapeItem::isVirtual() const { return m_isVirtual; }

    void FlowChartShapeItem::setInDegree(int inDegree) { m_inDegree = inDegree; }

    int FlowChartShapeItem::getInDegree() const { return m_inDegree; }

    void FlowChartShapeItem::setOutDegree(int outDegree) { m_outDegree = outDegree; }

    int FlowChartShapeItem::getOutDegree() const { return m_outDegree; }

    QRectF FlowChartShapeItem::outerRect() const {
        QRectF rect = boundingRect();
        rect.adjust(-m_margin, -m_margin, m_margin, m_margin);
        return rect;
    }

    QRectF FlowChartShapeItem::extendedRect() const {
        /*
         * Use shape1.extendedRect().intersects(shape2.extendedRect()) to avoid overlaps
         */
        QRectF base = outerRect(); // outerRect already includes margin
        return base.adjusted(-m_safeSpacing, -m_safeSpacing, m_safeSpacing, m_safeSpacing);
    }

    QRectF FlowChartShapeItem::boundingRect() const {
        // Derived classes provide innerRect()
        QRectF inner = innerRect();
        return {inner.x() - m_margin, inner.y() - m_margin, inner.width() + (2 * m_margin),
                inner.height() + (2 * m_margin)};
    }

    QSizeF FlowChartShapeItem::sizeForLabel() const {

        // Return default size if text is empty
        if (m_label.isEmpty()) {
            constexpr qreal DEFAULT_WIDTH = 20.0;
            constexpr qreal DEFAULT_HEIGHT = 10.0;
            return {DEFAULT_WIDTH, DEFAULT_HEIGHT};
        }
        QFontMetricsF fmt(m_font);
        QRectF textRect = fmt.boundingRect(m_label);
        // Add margins
        return {textRect.width() + (2 * m_margin), textRect.height() + (2 * m_margin)};
    }

    /// --- Virtual Node item ---
    VirtualNodeItem::VirtualNodeItem(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        setZValue(-2);
        setVisible(false);
        m_isVirtual = true;
    }

    QRectF VirtualNodeItem::boundingRect() const { return {0, 0, 0, 0}; }

    QRectF VirtualNodeItem::innerRect() const { return {0, 0, 0, 0}; }

    QPointF VirtualNodeItem::getEdgeIntersection(const QPointF& fromPoint,
                                                 const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        return pos();
    }

    QPointF VirtualNodeItem::getHorizontalEdge(bool isSource) const {
        return pos();
    }

    QPointF VirtualNodeItem::getVerticalEdge(bool isSource) const {
        return pos();
    }

    void VirtualNodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
        // Virtual nodes are only used for logical calculations and do not participate in actual drawing.
    }

    /// --- SubgraphShapeItemRect ---
    SubgraphShapeItemRect::SubgraphShapeItemRect(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        constexpr qreal DEFAULT_WIDTH = 20.0;
        constexpr qreal DEFAULT_HEIGHT = 10.0;
        m_width = DEFAULT_WIDTH;
        m_height = DEFAULT_HEIGHT;
        adjustSizeToText();
    }

    QSizeF SubgraphShapeItemRect::size() const { return {m_width, m_height}; }

    void SubgraphShapeItemRect::setSize(QSizeF size) {
        m_width = size.width();
        m_height = size.height();
        updateInnerRect();
    }

    void SubgraphShapeItemRect::setRect(QRectF rect) { m_innerRect = rect; }

    QRectF SubgraphShapeItemRect::innerRect() const { return m_innerRect; }

    QPointF SubgraphShapeItemRect::getEdgeIntersection(const QPointF& fromPoint,
                                                       const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        QRectF rect = innerRect();
        QLineF line(mapFromScene(fromPoint), mapFromScene(toPoint));

        // Calculate the intersection point between a ray and a rectangle side
        QLineF top(rect.topLeft(), rect.topRight());
        QLineF bottom(rect.bottomLeft(), rect.bottomRight());
        QLineF left(rect.topLeft(), rect.bottomLeft());
        QLineF right(rect.topRight(), rect.bottomRight());

        QPointF intersection;
        QVector<QLineF> edges = {top, bottom, left, right};
        for (const QLineF& edge : edges) {
            if (line.intersects(edge, &intersection) == QLineF::BoundedIntersection) {
                return mapToScene(intersection);
            }
        }

        // If there is no intersection point, return to the center (alternative)
        return mapToScene(rect.center());
    }

    QPointF SubgraphShapeItemRect::getHorizontalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        qreal radius = rect.width() / 2;
        return isSource ? QPointF(center.x() + radius, center.y())
                        : QPointF(center.x() - radius, center.y());
    }

    QPointF SubgraphShapeItemRect::getVerticalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        qreal radius = rect.height() / 2;
        return isSource ? QPointF(center.x(), center.y() + radius)
                        : QPointF(center.x(), center.y() - radius);
    }

    void SubgraphShapeItemRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                      QWidget* widget) {
        QRectF rect = innerRect();
        // Draw background (transparent for testing)
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);
        // Draw border
        painter->setPen(m_borderColor);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignTop | Qt::AlignHCenter, m_label);
    }

    void SubgraphShapeItemRect::updateInnerRect() {
        m_innerRect = QRectF(-m_width / 2, -m_height / 2, m_width, m_height);
        prepareGeometryChange();
        update();
    }

    void SubgraphShapeItemRect::adjustSizeToText() {
        QSizeF size = sizeForLabel();
        // Add extra width for rounded rectangle
        size += QSizeF(m_width, m_height);
        m_width = size.width();
        m_height = size.height();
        updateInnerRect();
    }

    /// --- ShapeItemRect ---
    ShapeItemRect::ShapeItemRect(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        adjustSizeToText();
    }

    QRectF ShapeItemRect::innerRect() const { return m_innerRect; }

    QPointF ShapeItemRect::getEdgeIntersection(const QPointF& fromPoint,
                                               const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        QRectF rect = innerRect();
        QLineF line(mapFromScene(fromPoint), mapFromScene(toPoint));

        // Calculate the intersection point between a ray and a rectangle side
        QLineF top(rect.topLeft(), rect.topRight());
        QLineF bottom(rect.bottomLeft(), rect.bottomRight());
        QLineF left(rect.topLeft(), rect.bottomLeft());
        QLineF right(rect.topRight(), rect.bottomRight());

        QPointF intersection;
        QVector<QLineF> edges = {top, bottom, left, right};
        for (const QLineF& edge : edges) {
            if (line.intersects(edge, &intersection) == QLineF::BoundedIntersection) {
                return mapToScene(intersection);
            }
        }

        // If there is no intersection point, return to the center (alternative)
        return mapToScene(rect.center());
    }

    QPointF ShapeItemRect::getHorizontalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        return isSource ? QPointF(rect.right(), center.y()) : QPointF(rect.left(), center.y());
    }

    QPointF ShapeItemRect::getVerticalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        return isSource ? QPointF(center.x(), rect.bottom()) : QPointF(center.x(), rect.top());
    }

    void ShapeItemRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                              QWidget* widget) {
        QRectF rect = innerRect();
        // Draw background (transparent for testing)
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect);
        // Draw border
        painter->setPen(m_borderColor);
        painter->drawRect(rect.adjusted(0, 0, -1, -1));
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, m_label);
    }

    void ShapeItemRect::adjustSizeToText() {
        constexpr qreal DEFAULT_WIDTH = 20.0;
        constexpr qreal DEFAULT_HEIGHT = 10.0;
        QSizeF size = sizeForLabel();
        // Add extra width for rounded rectangle
        size += QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_innerRect = QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height());
        prepareGeometryChange();
        update();
    }

    /// --- ShapeItemRoundedRect ---
    ShapeItemRoundedRect::ShapeItemRoundedRect(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        adjustSizeToText();
    }

    QRectF ShapeItemRoundedRect::innerRect() const { return m_innerRect; }

    QPointF ShapeItemRoundedRect::getEdgeIntersection(const QPointF& fromPoint,
                                                      const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        QRectF rect = innerRect();
        QLineF line(mapFromScene(fromPoint), mapFromScene(toPoint));

        // Calculate the intersection point between a ray and a rectangle side
        QLineF top(rect.topLeft(), rect.topRight());
        QLineF bottom(rect.bottomLeft(), rect.bottomRight());
        QLineF left(rect.topLeft(), rect.bottomLeft());
        QLineF right(rect.topRight(), rect.bottomRight());

        QPointF intersection;
        QVector<QLineF> edges = {top, bottom, left, right};
        for (const QLineF& edge : edges) {
            if (line.intersects(edge, &intersection) == QLineF::BoundedIntersection) {
                return mapToScene(intersection);
            }
        }

        // If there is no intersection point, return to the center (alternative)
        return mapToScene(rect.center());
    }

    QPointF ShapeItemRoundedRect::getHorizontalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        return isSource ? QPointF(rect.right(), center.y()) : QPointF(rect.left(), center.y());
    }

    QPointF ShapeItemRoundedRect::getVerticalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        return isSource ? QPointF(center.x(), rect.bottom()) : QPointF(center.x(), rect.top());
    }

    void ShapeItemRoundedRect::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                     QWidget* widget) {
        QRectF rect = innerRect();
        //
        constexpr qreal CORNER_RADIUS = 10.0;
        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, CORNER_RADIUS, CORNER_RADIUS);
        // Draw border
        painter->setPen(m_borderColor);
        painter->drawRoundedRect(rect.adjusted(0, 0, -1, -1), CORNER_RADIUS, CORNER_RADIUS);
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, m_label);
    }

    void ShapeItemRoundedRect::adjustSizeToText() {
        constexpr qreal DEFAULT_WIDTH = 20.0;
        constexpr qreal DEFAULT_HEIGHT = 10.0;
        QSizeF size = sizeForLabel();
        // Add extra width for rounded rectangle
        size += QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_innerRect = QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height());
        prepareGeometryChange();
        update();
    }

    /// --- ShapeItemCircle ---
    ShapeItemCircle::ShapeItemCircle(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        adjustSizeToText();
    }

    QRectF ShapeItemCircle::innerRect() const { return m_innerRect; }

    QPointF ShapeItemCircle::getEdgeIntersection(const QPointF& fromPoint,
                                                 const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        QRectF rect = innerRect();
        QPointF center = mapToScene(rect.center());
        qreal radius = rect.width() / 2;

        QLineF line(fromPoint, toPoint);
        qreal dxpos = toPoint.x() - fromPoint.x();
        qreal dypos = toPoint.y() - fromPoint.y();
        qreal length = QLineF(fromPoint, toPoint).length();
        if (length == 0) {
            return center;
        }

        // Calculate the intersection point between a ray and a circle
        qreal ratio = radius / length;
        QPointF intersection = center + QPointF(dxpos * ratio, dypos * ratio);
        return intersection;
    }

    QPointF ShapeItemCircle::getHorizontalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        qreal radius = rect.width() / 2;
        return isSource ? QPointF(center.x() + radius, center.y())
                        : QPointF(center.x() - radius, center.y());
    }

    QPointF ShapeItemCircle::getVerticalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        QPointF center = rect.center();
        qreal radius = rect.height() / 2;
        return isSource ? QPointF(center.x(), center.y() + radius)
                        : QPointF(center.x(), center.y() - radius);
    }

    void ShapeItemCircle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                QWidget* widget) {
        QRectF rect = innerRect();
        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawEllipse(rect);
        // Draw border
        painter->setPen(m_borderColor);
        painter->drawEllipse(rect.adjusted(0, 0, -1, -1));
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, m_label);
    }

    void ShapeItemCircle::adjustSizeToText() {
        constexpr qreal DEFAULT_WIDTH = 10.0;
        constexpr qreal DEFAULT_HEIGHT = 10.0;
        QSizeF size = sizeForLabel();
        // Get maximum of width and height
        qreal max = qMax(size.width(), size.height());
        size = QSizeF(max, max);
        // Add extra width for circle
        size += QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_innerRect = QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height());
        prepareGeometryChange();
        update();
    }

    /// --- ShapeItemDiamond ---
    ShapeItemDiamond::ShapeItemDiamond(const QString& label, QGraphicsItem* parent)
        : FlowChartShapeItem(label, parent) {
        adjustSizeToText();
    }

    QRectF ShapeItemDiamond::innerRect() const { return m_innerRect; }

    QPointF ShapeItemDiamond::getEdgeIntersection(const QPointF& fromPoint,
                                                  const QPointF& toPoint) const {
        assert(fromPoint != toPoint && "Start and end points must be different");
        QRectF rect = innerRect();
        QPointF center = mapToScene(rect.center());
        QPolygonF polygon;
        polygon << mapToScene(QPointF(0, -rect.height() / 2)) // Top
                << mapToScene(QPointF(rect.width() / 2, 0))   // Right
                << mapToScene(QPointF(0, rect.height() / 2))  // Bottom
                << mapToScene(QPointF(-rect.width() / 2, 0)); // Left

        QLineF line(fromPoint, toPoint);
        QPointF intersection;
        for (int i = 0; i < polygon.size(); ++i) {
            QLineF edge(polygon[i], polygon[(i + 1) % polygon.size()]);
            if (line.intersects(edge, &intersection) == QLineF::BoundedIntersection) {
                return intersection;
            }
        }

        return center;
    }

    QPointF ShapeItemDiamond::getHorizontalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        qreal width = rect.width();
        return isSource ? QPointF(rect.center().x() + (width / 2), rect.center().y())
                        : QPointF(rect.center().x() - (width / 2), rect.center().y());
    }

    QPointF ShapeItemDiamond::getVerticalEdge(bool isSource) const {
        QRectF rect = innerRect().translated(scenePos());
        qreal height = rect.height();
        return isSource ? QPointF(rect.center().x(), rect.center().y() + (height / 2))
                        : QPointF(rect.center().x(), rect.center().y() - (height / 2));
    }

    void ShapeItemDiamond::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                 QWidget* widget) {
        QRectF rect = innerRect();

        QPolygonF polygon;

        polygon << QPointF(0, -rect.height() / 2) // Top
                << QPointF(rect.width() / 2, 0)   // Right
                << QPointF(0, rect.height() / 2)  // Bottom
                << QPointF(-rect.width() / 2, 0); // Left

        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(polygon);
        // Draw border
        painter->setPen(m_borderColor);
        painter->drawPolygon(polygon);
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, m_label);
    }

    void ShapeItemDiamond::adjustSizeToText() {
        constexpr qreal DEFAULT_WIDTH = 40.0;
        constexpr qreal DEFAULT_HEIGHT = 20.0;
        QSizeF size = sizeForLabel();
        // Add extra width for diamond
        size += QSizeF(DEFAULT_WIDTH, DEFAULT_HEIGHT);
        m_innerRect = QRectF(-size.width() / 2, -size.height() / 2, size.width(), size.height());
        prepareGeometryChange();
        update();
    }

} // namespace hbchart
