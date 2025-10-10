/**
 * File: GanttChartShapeItem.cpp
 * Description: The implementation of the GanttChartShapeItem class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "GanttChartShapeItem.h"

#include <QFont>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QtMath>
#include <qDebug>

#include "GanttChartScene.h"

namespace hbchart {

    /// --- GanttChartLabel ---
    GanttChartLabel::GanttChartLabel(QString label, QGraphicsItem* parent)
        : m_label(std::move(label)), QGraphicsObject(parent) {
        constexpr qreal DEFAULT_MARGIN = 10.0;
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
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        updateInnerRect();
    }

    QRectF GanttChartLabel::innerRect() const { return m_innerRect; }

    QRectF GanttChartLabel::boundingRect() const {
        // Derived classes provide innerRect()
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        QRectF inner = innerRect();
        return {inner.x() - m_margin, inner.y() - m_margin,
                      inner.width() + (SIZE_FACTOR_DOUBLE * m_margin),
                      inner.height() + (SIZE_FACTOR_DOUBLE * m_margin)};
    }

    qreal GanttChartLabel::width() const { return m_width; }

    qreal GanttChartLabel::height() const { return m_height; }

    QString GanttChartLabel::getLabel() const { return m_label; }

    void GanttChartLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
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

    void GanttChartLabel::updateInnerRect() {
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        sizeForLabel();
        m_innerRect = QRectF(
            (-m_width / SIZE_FACTOR_DOUBLE) - m_margin, (-m_height / SIZE_FACTOR_DOUBLE) - m_margin,
            m_width + (SIZE_FACTOR_DOUBLE * m_margin), m_height + (SIZE_FACTOR_DOUBLE * m_margin));
        prepareGeometryChange();
        update();
    }

    void GanttChartLabel::sizeForLabel() {
        // Return default size if text is empty
        if (getLabel().isEmpty()) {
            return;
        }
        QFontMetricsF fmt(m_font);
        QRectF textRect = fmt.boundingRect(getLabel());
        m_width = textRect.width();
        m_height = textRect.height();
    }

    /// --- GanttChartShapeItem ---
    GanttChartShapeItem::GanttChartShapeItem(QGraphicsItem* parent) : QGraphicsObject(parent) {
        setFlag(QGraphicsItem::ItemIsMovable);
        setFlag(QGraphicsItem::ItemIsSelectable);
        constexpr qreal DEFAULT_MIN_X = 0.0;
        constexpr qreal DEFAULT_MAX_X = 10.0;
        constexpr qreal DEFAULT_ROW_MARGIN = 10.0;
        constexpr QColor DEFAULT_FILL_COLOR = QColor(236, 236, 255);
        constexpr QColor DEFAULT_BORDER_COLOR = QColor(171, 145, 227);
        m_width = DEFAULT_ROW_HEIGHT;
        m_rowHeight = DEFAULT_ROW_HEIGHT;
        m_margin = DEFAULT_ROW_MARGIN;
        m_label = "";
        m_font = QFont();
        m_fontColor = Qt::black;
        m_fillColor = DEFAULT_FILL_COLOR;
        m_borderColor = DEFAULT_BORDER_COLOR;
        m_pendingDeltaX = 0.0;
        m_minX = DEFAULT_MIN_X;
        m_maxX = DEFAULT_MAX_X;
    }

    void GanttChartShapeItem::setXLimits(qreal minX, qreal maxX) {
        if (minX >= maxX) {
            qWarning() << "Invalid X limits";
            return;
        }
        // Limited range
        m_minX = minX + width() / 2;
        m_maxX = maxX + width() / 2;
    };

    qreal GanttChartShapeItem::height() const { return m_rowHeight; }

    void GanttChartShapeItem::setRect(RowWidth width, RowHeight height) {
        m_width = width;
        m_rowHeight = height;
        updateInnerRect();
    }

    QRectF GanttChartShapeItem::boundingRect() const {
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        // Derived classes provide innerRect()
        QRectF inner = innerRect();
        return {inner.x() - m_margin, inner.y() - m_margin, inner.width() + (SIZE_FACTOR_DOUBLE * m_margin),
                      inner.height() + (SIZE_FACTOR_DOUBLE * m_margin)};
    }

    void GanttChartShapeItem::mousePressEvent(QGraphicsSceneMouseEvent* event) {
        m_pendingDeltaX = 0;
        // emit eventDragStarted();
        QGraphicsObject::mousePressEvent(event);
    }

    void GanttChartShapeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
        if (event->buttons() == Qt::LeftButton) {
            // Save the old location
            QPointF oldPos = pos();

            // calculate delta
            QPointF delta = event->scenePos() - oldPos;
            qreal newX = oldPos.x() + delta.x();

            // Limited range
            qreal x_center = qBound(m_minX, newX, m_maxX);

            // Update location
            setPos(x_center, oldPos.y());

            // renew pending delta
            qreal deltaX = x_center - oldPos.x();
            m_pendingDeltaX += deltaX;

            event->accept();
        }
    }

    void GanttChartShapeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
        if (m_pendingDeltaX != 0) {
            emit eventPositionChanged(m_pendingDeltaX); // deltaX
            m_pendingDeltaX = 0;
        }
    }

    /// --- GanttChartItemSection ---
    GanttChartItemSection::GanttChartItemSection(const GanttSection* section, QGraphicsItem* parent)
        : m_section(section), GanttChartShapeItem(parent) {
        constexpr int DEFAULT_FONT_SIZE = 14;
        m_font.setBold(true);
        int fontSize = DEFAULT_FONT_SIZE;
        m_font.setPointSize(fontSize);
        setZValue(0); // behind axis lines
        setFlag(QGraphicsItem::ItemIsMovable, false);
        setFlag(QGraphicsItem::ItemIsSelectable, false);
        GanttChartItemSection::updateInnerRect();
    }

    qreal GanttChartItemSection::width() const { return m_width; }

    QRectF GanttChartItemSection::innerRect() const { return m_innerRect; }

    QString GanttChartItemSection::getLabel() const { return m_section->name; }

    void GanttChartItemSection::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                      QWidget* widget) {
        QRectF rect = innerRect();
        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, 0, 0);
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignVCenter | Qt::AlignLeft, getLabel());
    }

    void GanttChartItemSection::updateInnerRect() {
        m_innerRect = QRectF(-m_width / 2, -m_rowHeight / 2, m_width, m_rowHeight);
        prepareGeometryChange();
        update();
    }

    /// --- GanttChartItemTask ---
    GanttChartItemTask::GanttChartItemTask(const GanttTask* task, QGraphicsItem* parent)
        : m_task(task), GanttChartShapeItem(parent) {
        setZValue(3);
        GanttChartItemTask::updateInnerRect();
    }

    qreal GanttChartItemTask::width() const { return m_width; }

    QRectF GanttChartItemTask::innerRect() const { return m_innerRect; }

    QString GanttChartItemTask::getLabel() const { return m_task->name; }

    void GanttChartItemTask::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                   QWidget* widget) {
        constexpr qreal DEFAULT_BORDER_RADIUS = 5.0;
        QRectF rect = innerRect();
        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawRoundedRect(rect, DEFAULT_BORDER_RADIUS, DEFAULT_BORDER_RADIUS);
        // Draw border
        QPen borderPen(m_borderColor);
        borderPen.setWidth(2);
        painter->setPen(borderPen);
        painter->drawRoundedRect(rect.adjusted(0, 0, -1, -1), DEFAULT_BORDER_RADIUS, DEFAULT_BORDER_RADIUS);
        // Draw text
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(rect, Qt::AlignCenter, getLabel());
    }

    void GanttChartItemTask::updateInnerRect() {
        m_innerRect = QRectF(-m_width / 2, -m_rowHeight / 2, m_width, m_rowHeight);
        prepareGeometryChange();
        update();
    }

    /// --- GanttChartItemMilestone ---
    GanttChartItemMilestone::GanttChartItemMilestone(const GanttMilestone* milestone,
                                                     QGraphicsItem* parent)
        : m_milestone(milestone), GanttChartShapeItem(parent) {
        m_width = 2 * DEFAULT_ROW_MARGIN;
        m_rowHeight = 2 * DEFAULT_ROW_MARGIN;
        setZValue(4);
        GanttChartItemMilestone::updateInnerRect();
    }

    void GanttChartItemMilestone::setXLimits(qreal minX, qreal maxX) {
        if (minX >= maxX) {
            qWarning() << "Invalid X limits";
            return;
        }
        // Limited range
        m_minX = minX;
        m_maxX = maxX;
    };

    qreal GanttChartItemMilestone::width() const { return m_width; }

    QRectF GanttChartItemMilestone::innerRect() const { return m_innerRect; }

    QRectF GanttChartItemMilestone::boundingRect() const {
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        QRectF inner = innerRect();
        QSizeF textSize = sizeForLabel();
        QRectF textRect(inner.right() + margin(), -textSize.height() / 2, textSize.width(),
                        textSize.height());
        QRectF totalRect = inner.united(textRect);
        return {totalRect.x() - m_margin, totalRect.y() - m_margin,
                      totalRect.width() + (SIZE_FACTOR_DOUBLE * m_margin), totalRect.height() + (SIZE_FACTOR_DOUBLE * m_margin)};
    }

    QString GanttChartItemMilestone::getLabel() const { return m_milestone->name; }

    void GanttChartItemMilestone::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                        QWidget* widget) {
        QRectF rect = innerRect();

        QPolygonF polygon;
        // Draw diamond shape, rect 0 0 width height
        polygon << QPointF(0, -rect.height() / 2) // Top
                << QPointF(rect.width() / 2, 0)   // Right
                << QPointF(0, rect.height() / 2)  // Bottom
                << QPointF(-rect.width() / 2, 0); // Left

        // Draw background
        painter->setBrush(m_fillColor);
        painter->setPen(Qt::NoPen);
        painter->drawPolygon(polygon);
        // Draw border
        QPen borderPen(m_borderColor);
        borderPen.setWidth(2);
        painter->setPen(borderPen);
        painter->drawPolygon(polygon);
        // Draw text center with right offset is width
        QSizeF textSize = sizeForLabel();
        QRectF textPos(rect.right() + margin(), -textSize.height() / 2, textSize.width(),
                       textSize.height());
        painter->setPen(m_fontColor);
        painter->setFont(m_font);
        painter->drawText(textPos, Qt::AlignCenter, getLabel());
    }

    QSizeF GanttChartItemMilestone::sizeForLabel() const {
        constexpr qreal SIZE_FACTOR_DOUBLE = 2.0;
        // Return default size if text is empty
        if (getLabel().isEmpty()) {
            constexpr qreal DEFAULT_WIDTH = 40.0;
            constexpr qreal DEFAULT_HEIGHT = 20.0;
            return {DEFAULT_WIDTH, DEFAULT_HEIGHT};
        }
        QFontMetricsF fmt(m_font);
        QRectF textRect = fmt.boundingRect(getLabel());
        // Add margins
        return {textRect.width() + (SIZE_FACTOR_DOUBLE * m_margin), textRect.height() + (SIZE_FACTOR_DOUBLE * m_margin)};
    }

    void GanttChartItemMilestone::updateInnerRect() {
        m_innerRect = QRectF(-m_width / 2, -m_rowHeight / 2, m_width, m_rowHeight);
        prepareGeometryChange();
        update();
    }

} // namespace hbchart
