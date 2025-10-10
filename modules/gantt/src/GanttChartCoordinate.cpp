/**
 * File: GanttChartCoordinate.cpp
 * Description: The implementation of GanttChartCoordinate class for Gantt chart axis.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "GanttChartCoordinate.h"

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <qmath.h>

namespace hbchart {

    // --- GanttChartCoordPath Implementation ---
    GanttChartCoordPath::GanttChartCoordPath(QString label, QGraphicsItem* parent)
        : m_label(std::move(label)), QGraphicsPathItem(parent) {
        constexpr qreal DEFAULT_LINE_WIDTH = 2.0;
        constexpr qreal DEFAULT_FONT_SIZE = 14;
        constexpr qreal DEFAULT_LABEL_MARGIN = 5.0;
        constexpr qreal DEFAULT_LABEL_OFFSET = 10.0;
        constexpr qreal DEFAULT_EXTEND_SIZE = 5.0;
        constexpr qreal DEFAULT_START_X = -10.0;
        constexpr qreal DEFAULT_END_X = 10.0;
        constexpr QRectF DEFAULT_BOUNDING_RECT = QRectF(-15, -15, 20, 20);

        m_labelPosition = CoordLabelPosition::RIGHT;
        m_lineWidth = DEFAULT_LINE_WIDTH;
        m_lineColor = Qt::black;
        m_lineStyle = Qt::SolidLine;
        m_labelFont = QFont("Arial", DEFAULT_FONT_SIZE);
        m_labelFont.setBold(true);
        m_labelMargin = DEFAULT_LABEL_MARGIN;
        m_labelOffset = DEFAULT_LABEL_OFFSET;
        m_start = QPointF(DEFAULT_START_X, 0.0);
        m_end = QPointF(DEFAULT_END_X, 0.0);
        m_extendSize = DEFAULT_EXTEND_SIZE;
        m_boundingRect = DEFAULT_BOUNDING_RECT;
        m_showLabel = true;
        m_axisArrowVisible = true;
        setZValue(1); // Above sections (z=0), below tasks (z=3)
        setFlags(flags() & ~(ItemIsSelectable | ItemIsMovable)); // Non-selectable, non-movable
        updateLabelRect();                                       // Initialize m_labelRect
    }

    void GanttChartCoordPath::setLineWidth(qreal width) {
        m_lineWidth = width;
        emit eventPathPropertyChanged();
    }

    qreal GanttChartCoordPath::getLineWidth() const { return m_lineWidth; }

    void GanttChartCoordPath::setLineColor(const QColor& color) {
        if (m_lineColor == color) {
            return;
        }
        m_lineColor = color;
        emit eventPathPropertyChanged();
    }

    QColor GanttChartCoordPath::getLineColor() const { return m_lineColor; }

    void GanttChartCoordPath::setLineStyle(Qt::PenStyle style) {
        if (m_lineStyle == style) {
            return;
        }
        m_lineStyle = style;
        emit eventPathPropertyChanged();
    }

    Qt::PenStyle GanttChartCoordPath::getLineStyle() const { return m_lineStyle; }

    void GanttChartCoordPath::setLabelFont(const QFont& font) {
        if (m_labelFont == font) {
            return;
        }
        m_labelFont = font;
        updateLabelRect();
        emit eventPathPropertyChanged();
    }

    QFont GanttChartCoordPath::getLabelFont() const { return m_labelFont; }

    void GanttChartCoordPath::setLabel(const QString& label) {
        if (m_label == label) {
            return;
        }
        m_label = label;
        updateLabelRect();
        emit eventPathPropertyChanged();
    }

    QString GanttChartCoordPath::getLabel() const { return m_label; }

    void GanttChartCoordPath::setLabelMargin(qreal margin) {
        if (m_labelMargin == margin) {
            return;
        }
        m_labelMargin = margin;
        updateLabelRect();
        emit eventPathPropertyChanged();
    }

    qreal GanttChartCoordPath::getLabelMargin() const { return m_labelMargin; }

    void GanttChartCoordPath::setLabelOffset(qreal offset) {
        if (offset < 0 || m_labelOffset == offset) {
            return;
        }
        m_labelOffset = offset;
        emit eventPathPropertyChanged();
    }

    qreal GanttChartCoordPath::getLabelOffset() const { return m_labelOffset; }

    void GanttChartCoordPath::setShowLabel(bool show) {
        if (m_showLabel == show) {
            return;
        }
        m_showLabel = show;
        updateLabelRect();
        emit eventPathPropertyChanged();
    }

    bool GanttChartCoordPath::isShowLabel() const { return m_showLabel; }

    void GanttChartCoordPath::setLabelPosition(CoordLabelPosition position) {
        if (m_labelPosition == position) {
            return;
        }
        m_labelPosition = position;
        emit eventPathPropertyChanged();
    }

    CoordLabelPosition GanttChartCoordPath::getLabelPosition() const { return m_labelPosition; }

    void GanttChartCoordPath::setAxisArrowVisible(bool show) {
        if (m_axisArrowVisible == show) {
            return;
        }
        m_axisArrowVisible = show;
        emit eventPathPropertyChanged();
    }

    bool GanttChartCoordPath::isAxisArrowVisible() const { return m_axisArrowVisible; }

    void GanttChartCoordPath::updateCoordEndPoints(QPointF start, QPointF end) {
        if (start == end) {
            qWarning() << "Invalid start and end points";
            return;
        }
        if (m_start == start && m_end == end) {
            return;
        }
        m_start = start;
        m_end = end;
        emit eventPathPropertyChanged();
    }

    QPair<QPointF, QPointF> GanttChartCoordPath::getCoordEndPoints() const {
        return {m_start, m_end};
    }

    void GanttChartCoordPath::setExtendSize(qreal extend) {
        if (extend < 0 || m_extendSize == extend) {
            return;
        }
        m_extendSize = extend;
        emit eventPathPropertyChanged();
    }

    qreal GanttChartCoordPath::getExtendSize() const { return m_extendSize; }

    QRectF GanttChartCoordPath::boundingRect() const { return m_boundingRect; }

    void GanttChartCoordPath::updateLabelRect() {
        if (m_label.isEmpty()) {
            m_labelRect = QRectF();
            return;
        }
        QFontMetricsF fmt(m_labelFont);
        m_labelRect = fmt.boundingRect(m_label);
        // Fix: Correct adjust direction
        m_labelRect.adjust(-m_labelMargin, -m_labelMargin, m_labelMargin, m_labelMargin);
        // Add extra margin for safety
        qreal extraMargin = m_labelMargin; // Same as m_labelMargin
        m_labelRect.adjust(-extraMargin, -extraMargin, extraMargin, extraMargin);
    }

    void GanttChartCoordPath::updateArrowHeads() {
        constexpr qreal OPPOSITE_ANGLE = 180.0;
        m_arrows.clear();
        if (!m_axisArrowVisible) {
            return;
        }
        qreal angle = 0.0;
        m_arrows << calculateArrowHead(m_start, angle);
        m_arrows << calculateArrowHead(m_end, angle + OPPOSITE_ANGLE);
    }

    QPolygonF GanttChartCoordPath::calculateArrowHead(const QPointF& point, qreal angleDeg) {
        constexpr qreal ANGLE_LIMIT = 90.0; // Limit the angle to prevent too large arrow head
        constexpr qreal ARROW_LENGTH = 10.0; // Arrow length
        constexpr qreal ARROW_WIDTH = 5.0;   // Half the width of the bottom edge

        QPointF arrowDir;
        QPointF perp;

        // Optimization for specific angles
        if (qFuzzyCompare(angleDeg, 0.0)) {
            arrowDir = QPointF(1.0, 0.0);
            perp = QPointF(0.0, 1.0);
        } else if (qFuzzyCompare(angleDeg, ANGLE_LIMIT)) {
            arrowDir = QPointF(0.0, 1.0);
            perp = QPointF(-1.0, 0.0);
        } else if (qFuzzyCompare(angleDeg, -ANGLE_LIMIT)) {
            arrowDir = QPointF(0.0, -1.0);
            perp = QPointF(1.0, 0.0);
        } else {
            qreal rad = qDegreesToRadians(angleDeg);
            arrowDir = QPointF(qCos(rad), qSin(rad));
            perp = QPointF(-arrowDir.y(), arrowDir.x());
        }

        // Calculate arrow points
        QPolygonF arrow;
        arrow << point << (point - arrowDir * ARROW_LENGTH + perp * ARROW_WIDTH)
              << (point - arrowDir * ARROW_LENGTH - perp * ARROW_WIDTH);
        return arrow;
    }

    void GanttChartCoordPath::updateBoundingRect() {
        constexpr qreal POS_RATIO = 2.0; // Ratio of position to extend size
        // Start with path bounding rect
        m_boundingRect = path().boundingRect();

        // Include label if visible
        if (m_showLabel && !m_label.isEmpty()) {
            QPointF labelPos;
            switch (m_labelPosition) {
            case CoordLabelPosition::RIGHT:
                labelPos.setX(m_end.x() + m_extendSize + m_labelOffset);
                labelPos.setY(m_end.y() - (m_labelRect.height() / POS_RATIO));
                break;
            case CoordLabelPosition::LEFT:
                labelPos.setX(m_start.x() - m_extendSize - m_labelOffset - m_labelRect.width());
                labelPos.setY(m_start.y() - (m_labelRect.height() / POS_RATIO));
                break;
            case CoordLabelPosition::BOTTOM_RIGHT:
                labelPos.setX(((m_end.x() - m_start.x()) / POS_RATIO) + m_start.x() + m_labelOffset);
                labelPos.setY(m_end.y() + m_labelOffset);
                break;
            case CoordLabelPosition::BOTTOM_LEFT:
                labelPos.setX(((m_end.x() - m_start.x()) / POS_RATIO) + m_start.x() - m_labelRect.width() -
                              m_labelOffset);
                labelPos.setY(m_end.y() + m_labelOffset);
                break;
            case CoordLabelPosition::BOTTOM_CENTER:
                labelPos.setX(((m_end.x() - m_start.x()) / POS_RATIO) + m_start.x());
                labelPos.setY(m_end.y() + m_labelOffset);
                break;
            }
            m_labelRect.moveTopLeft(labelPos);
            m_boundingRect = m_boundingRect.united(m_labelRect);
        }

        // Extend for pen width and extend size
        qreal penWidth = m_lineWidth; // Should be m_lineWidth
        m_boundingRect.adjust(-penWidth, -penWidth, penWidth, penWidth);
        m_boundingRect.adjust(-m_extendSize, -m_extendSize, m_extendSize, m_extendSize);
    }

    // --- GanttChartCoordX Implementation ---
    GanttChartCoordX::GanttChartCoordX(const QString& label, QGraphicsItem* parent)
        : GanttChartCoordPath(label, parent) {
        connect(this, &GanttChartCoordPath::eventPathPropertyChanged, this,
                &GanttChartCoordX::updatePath);
    }

    void GanttChartCoordX::updatePath() {
        QPainterPath path;
        QPointF start = m_start; // - m_extendSize;
        start.setX(start.x() - m_extendSize);
        QPointF end = m_end; // + m_extendSize;
        end.setX(end.x() + m_extendSize);
        path.moveTo(start);
        path.lineTo(end);
        setPath(path);
        updateArrowHeads();
        updateBoundingRect();
    }

    void GanttChartCoordX::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                 QWidget* widget) {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        //
        QPen pen;
        pen.setWidthF(m_lineWidth);
        pen.setColor(m_lineColor);
        pen.setStyle(m_lineStyle);
        painter->setPen(pen);
        painter->drawPath(path());

        // draw axis arrow
        if (m_axisArrowVisible) {
            painter->setPen(Qt::NoPen);
            for (const QPolygonF& arrow : m_arrows) {
                painter->drawPolygon(arrow);
            }
        }

        QPen pen2;
        pen2.setColor(Qt::black);
        // draw label
        if (!m_label.isEmpty() && m_showLabel) {
            // draw label
            painter->setPen(pen2);
            painter->drawText(m_labelRect, Qt::AlignCenter, m_label);
        }
    }

    // --- GanttChartXAxisGridLine Implementation ---
    GanttChartXAxisGridLine::GanttChartXAxisGridLine(const QString& label, QGraphicsItem* parent)
        : GanttChartCoordPath(label, parent) {
        m_labelPosition = CoordLabelPosition::BOTTOM_CENTER;
        m_isExtendGuideLine = true;
        connect(this, &GanttChartCoordPath::eventPathPropertyChanged, this,
                &GanttChartXAxisGridLine::updatePath);
    }

    void GanttChartXAxisGridLine::setExtendGuideLine(bool isExtendGuideLines) {
        if (m_isExtendGuideLine == isExtendGuideLines) {
            return;
        }
        m_isExtendGuideLine = isExtendGuideLines;
        emit eventPathPropertyChanged();
    }

    bool GanttChartXAxisGridLine::isExtendGuideLine() const { return m_isExtendGuideLine; }

    void GanttChartXAxisGridLine::updatePath() {
        QPainterPath path;
        QPointF start = m_start; // - m_extendSize;
        start.setY(start.y() + m_extendSize);
        QPointF end = m_end; // + m_extendSize;
        end.setY(end.y() - m_extendSize);
        path.moveTo(start);
        path.lineTo(end);
        setPath(path);
        updateBoundingRect();
    }

    void GanttChartXAxisGridLine::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                        QWidget* widget) {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        //
        QPen pen(m_lineColor);
        pen.setWidthF(m_lineWidth);
        pen.setStyle(m_lineStyle);
        painter->setPen(pen);
        painter->drawPath(path());

        QPen pen2;
        pen2.setColor(Qt::black);
        // draw label
        if (!m_label.isEmpty() && m_showLabel) {
            // draw label
            painter->setPen(pen2);
            painter->drawText(m_labelRect, Qt::AlignCenter, m_label);
        }
    }

    void GanttChartXAxisGridLine::updateBoundingRect() {
        constexpr qreal POS_RATIO = 2.0; // Ratio of position to extend size
        // Grid line is verctorial, so we only need to update its bounding rect
        m_boundingRect = path().boundingRect();

        // Include label if visible
        if (m_showLabel && !m_label.isEmpty()) {
            QPointF labelPos;
            switch (m_labelPosition) {
            case CoordLabelPosition::RIGHT:
                labelPos.setX(m_end.x() + m_extendSize + m_labelOffset);
                labelPos.setY(((m_end.y() - m_start.y()) / POS_RATIO) + m_start.y());
                break;
            case CoordLabelPosition::LEFT:
                labelPos.setX(m_start.x() - m_extendSize - m_labelOffset - m_labelRect.width());
                labelPos.setY(((m_end.y() - m_start.y()) / POS_RATIO) + m_start.y());
                break;
            case CoordLabelPosition::BOTTOM_RIGHT:
                labelPos.setX(m_start.x() + m_labelOffset);
                labelPos.setY(m_start.y() + m_extendSize);
                break;
            case CoordLabelPosition::BOTTOM_LEFT:
                labelPos.setX(m_start.x() - m_labelRect.width() - m_labelOffset);
                labelPos.setY(m_start.y() + m_extendSize);
                break;
            case CoordLabelPosition::BOTTOM_CENTER:
                labelPos.setX(m_start.x() - (m_labelRect.width() / POS_RATIO));
                labelPos.setY(m_start.y() + m_extendSize);
                break;
            }
            m_labelRect.moveTopLeft(labelPos);
            m_boundingRect = m_boundingRect.united(m_labelRect);
        }

        // Extend for pen width and extend size
        qreal penWidth = m_lineWidth; // Should be m_lineWidth
        qreal extra = qMax(penWidth, m_extendSize);
        m_boundingRect.adjust(-extra, -extra, extra, extra);
    }

    // --- GanttChartCoordinate Implementation ---
    GanttChartCoordinate::GanttChartCoordinate(GanttChartLayouter* layouter, GanttChartScene* scene,
                                               QObject* parent)
        : QObject(parent), m_scene(scene), m_layouter(layouter) {
        initialize();
    }

    void GanttChartCoordinate::initialize() {
        constexpr int PRIMARY_GRID_LINE_COUNT = 5;
        constexpr int SECONDARY_GRID_LINE_COUNT = 2;

        m_coordX = nullptr;
        m_coordXLabel = tr("Time");
        m_showXAxisPrimaryGridLine = true;
        m_showXAxisSecondaryGridLine = true;
        m_primaryGridLineCount = PRIMARY_GRID_LINE_COUNT;           ///< Number of primary grid lines
        m_secondaryGridLineCount = SECONDARY_GRID_LINE_COUNT;       ///< Number of secondary grid
    }

    GanttChartCoordinate::~GanttChartCoordinate() {
        resetCoordinate();
    }

    void GanttChartCoordinate::resetCoordinate() {
        // reset x axis
        if (m_coordX != nullptr) {
            delete m_coordX;
            m_coordX = nullptr;
        }

        // reset x axis grid lines
        for (auto& gridLine : m_primaryGridLines) {
            delete gridLine;
        }
        m_primaryGridLines.clear();
        for (auto& gridLine : m_secondaryGridLines) {
            delete gridLine;
        }
        m_secondaryGridLines.clear();
    }

    void GanttChartCoordinate::setCoordXLabel(const QString& label) {
        if (m_coordXLabel != label) {
            m_coordXLabel = label;
            emit eventCoordinatePropertyChanged();
        }
    }

    QString GanttChartCoordinate::getCoordXLabel() const { return m_coordXLabel; }

    void GanttChartCoordinate::setShowXAxisPrimaryGridLine(bool show) {
        if (m_showXAxisPrimaryGridLine != show) {
            m_showXAxisPrimaryGridLine = show;
            emit eventCoordinatePropertyChanged();
        }
    }

    bool GanttChartCoordinate::isShowXAxisPrimaryGridLine() const {
        return m_showXAxisPrimaryGridLine;
    }

    void GanttChartCoordinate::setShowXAxisSecondaryGridLine(bool show) {
        if (m_showXAxisSecondaryGridLine != show) {
            m_showXAxisSecondaryGridLine = show;
            emit eventCoordinatePropertyChanged();
        }
    }

    bool GanttChartCoordinate::isShowXAxisSecondaryGridLine() const {
        return m_showXAxisSecondaryGridLine;
    }

    void GanttChartCoordinate::setPrimaryGridLineCount(int count) {
        if (m_primaryGridLineCount != count && count >= 2) {
            m_primaryGridLineCount = count;
            emit eventCoordinatePropertyChanged();
        }
    }

    int GanttChartCoordinate::getPrimaryGridLineCount() const { return m_primaryGridLineCount; }

    void GanttChartCoordinate::setSecondaryGridLineCount(int count) {
        if (m_secondaryGridLineCount != count && count >= 1) {
            m_secondaryGridLineCount = count;
            emit eventCoordinatePropertyChanged();
        }
    }

    int GanttChartCoordinate::getSecondaryGridLineCount() const { return m_secondaryGridLineCount; }

    void GanttChartCoordinate::updateAll() {
        // reset
        resetCoordinate();
        // create x axis
        Q_ASSERT(m_coordX == nullptr);
        QPointF start = m_layouter->getChartOrigin();
        QPointF end = m_layouter->getChartEnd();
        qreal span = m_layouter->getExtraSpan();
        Q_ASSERT(start.x() < end.x());
        m_coordX = new GanttChartCoordX(m_coordXLabel);
        m_coordX->setExtendSize(m_scene->getSaftyMargin());
        m_coordX->updateCoordEndPoints(start, end);
        m_scene->addItem(m_coordX);

        // create x axis primary grid lines
        if (m_showXAxisPrimaryGridLine) {
            calculateXAxisPrimaryGridLine(start, end, span);
            for (auto& gridLine : m_primaryGridLines) {
                m_scene->addItem(gridLine);
            }
        }
        // create x axis secondary grid lines
        if (m_showXAxisSecondaryGridLine) {
            calculateXAxisSecondaryGridLine(start, end, span);
            for (auto& gridLine : m_secondaryGridLines) {
                m_scene->addItem(gridLine);
            }
        }
    }

    void GanttChartCoordinate::calculateXAxisPrimaryGridLine(QPointF start, QPointF end,
                                                             qreal span) {
        m_primaryGridLines.clear();
        if (!m_showXAxisPrimaryGridLine || m_primaryGridLineCount < 2) {
            return;
        }
        Q_ASSERT(m_coordX != nullptr);
        qreal baseTime = m_layouter->getModelBaseTime();
        TimeUnitType timeUnit = m_scene->getCoreTimeUnitType();
        qreal deltaX = (end.x() - span - start.x()) / (m_primaryGridLineCount - 1);
        qreal lineHeight = m_layouter->getChartInnerRect().height();
        constexpr qreal POS_RATIO = 2.0; // Ratio of position to extend size
        int iCount = qRound((end.x() - start.x()) / deltaX);
        for (int idx = 0; idx < iCount; idx++) {
            qreal xpos = start.x() + (idx * deltaX);
            QPointF start_new = QPointF(xpos, start.y());
            QPointF end_new = QPointF(xpos, start.y() - lineHeight);
            auto* gridLine = new GanttChartXAxisGridLine();
            gridLine->updateCoordEndPoints(start_new, end_new);
            gridLine->setExtendSize(m_scene->getSaftyMargin() / POS_RATIO);
            gridLine->setLabelPosition(CoordLabelPosition::BOTTOM_CENTER);
            qreal delatScene = m_scene->timeSceneRatio().fromScene(idx * deltaX);
            qreal deltaSec = timeUnit.toSeconds(delatScene);
            qreal timeSec = baseTime + deltaSec;
            QString label = m_scene->formatAxisLabel(timeSec);
            gridLine->setLabel(label);
            gridLine->setLineColor(Qt::gray);
            m_primaryGridLines.append(gridLine);
        }
    }

    void GanttChartCoordinate::calculateXAxisSecondaryGridLine(QPointF start, QPointF end,
                                                               qreal span) {
        m_secondaryGridLines.clear();
        if (!m_showXAxisPrimaryGridLine || m_secondaryGridLineCount < 1 ||
            m_primaryGridLineCount < 2) {
            return;
        }
        Q_ASSERT(m_coordX != nullptr);
        qreal deltaX = (end.x() - span - start.x()) / (m_primaryGridLineCount - 1);
        qreal deltaXSecondary = deltaX / (m_secondaryGridLineCount + 1);
        qreal lineHeight = m_layouter->getChartInnerRect().height();

        constexpr qreal POS_RATIO = 2.0; // Ratio of position to extend size
        int iCount = qRound((end.x() - start.x()) / deltaX); 
        for (int idx = 0; idx < iCount; idx++) {
            qreal xpos = start.x() + (idx * deltaX);
            qreal next_x = xpos + deltaX;
            if (next_x > end.x()) {
                break;
            }
            qreal sxstart = xpos + deltaXSecondary;
            qreal sxend = next_x - (deltaXSecondary / POS_RATIO);
            int jCount = qRound((sxend - sxstart) / deltaXSecondary);
            for (int jdx = 0; jdx < jCount; jdx++) {
                qreal sxpos = sxstart + (jdx * deltaXSecondary);
                QPointF secondaryStart = QPointF(sxpos, start.y());
                QPointF secondaryEnd = QPointF(sxpos, start.y() - lineHeight);
                auto* gridLine = new GanttChartXAxisGridLine();
                gridLine->updateCoordEndPoints(secondaryStart, secondaryEnd);
                gridLine->setLineColor(Qt::lightGray);
                gridLine->setLineStyle(Qt::DashLine);
                m_secondaryGridLines.append(gridLine);
            }
        }
    }

} // namespace hbchart
