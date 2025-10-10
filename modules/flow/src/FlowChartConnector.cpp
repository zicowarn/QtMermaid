/**
 * File: FlowChartConnector.cpp
 * Description: The implementation of the FlowChartConnector class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */
#include "FlowChartConnector.h"

#include <QDebug>
#include <QFontMetricsF>
#include <QObject>
#include <QPainter>
#include <qmath.h>

#define FLOW_EDGE_DEBUG

namespace hbchart {

    /// --- FlowChartCtrlPointItem ---
    FlowChartCtrlPointItem::FlowChartCtrlPointItem(const QPointF& pos, QGraphicsItem* parent)
        : QGraphicsEllipseItem(parent) {
        constexpr qreal defaultSize = 2.0;
        constexpr QColor bgColor(255, 0, 0, 255);
        constexpr QColor textColor(0, 0, 0, 255);
        QRectF rect(-defaultSize / 2, -defaultSize / 2, defaultSize, defaultSize);
        setRect(rect);
        setPos(pos);
        setZValue(1);
        setBrush(bgColor);
        setPen(QPen(textColor, 1));
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    }

    QVariant FlowChartCtrlPointItem::itemChange(GraphicsItemChange change, const QVariant& value) {
        if (change == ItemPositionChange) {
            emit eventCtrlPointMoved(value.toPointF());
        }
        return QGraphicsEllipseItem::itemChange(change, value);
    }

    /// --- FlowChartEdgeItem ---
    FlowChartEdgeItem::FlowChartEdgeItem(FlowChartScene* scene, QGraphicsItem* parent)
        : m_scene(scene), QGraphicsPathItem(parent) {
        constexpr int DEFAULT_WIDTH = 1.0;
        constexpr qreal BASE_ARROW_LENGTH = 10.0;
        constexpr qreal BASE_ARROW_WIDTH = 5.0;
        constexpr qreal ARROW_SCALE_FACTOR = 0.6;
        constexpr qreal DEFAULT_FONT_SIZE = 12.0;
        m_label = "";
        m_source = nullptr;
        m_target = nullptr;
        m_width = DEFAULT_WIDTH;
        m_isSelfLoop = false;
        m_pathType = PathType::POLYLINE;
        m_font = QFont("Arial", DEFAULT_FONT_SIZE);
        setZValue(-1); // Ensure that the connecting line is below the node
        m_baseArrowLength = BASE_ARROW_LENGTH;
        m_baseArrowWidth = BASE_ARROW_WIDTH;
        m_arrowScaleFactor = ARROW_SCALE_FACTOR;
        m_arrows = QVector<QPolygonF>();
        m_controlPoints = QVector<QPointF>();
        m_controlPointItems = QVector<FlowChartCtrlPointItem*>();
        m_isHorizontal = true;
    }

    void FlowChartEdgeItem::setLabel(const QString& label) { m_label = label; }

    void FlowChartEdgeItem::setEdgeStyle(EdgeStyle style) { m_style = style; }

    void FlowChartEdgeItem::setSource(FlowChartShapeItem* source) {
        Q_ASSERT(source);
        if (m_source != nullptr) {
            QObject::disconnect(m_source, &FlowChartShapeItem::positionChanged, this,
                                &FlowChartEdgeItem::updatePath);
        }
        m_source = source;
        if (m_source != nullptr) {
            QObject::connect(m_source, &FlowChartShapeItem::positionChanged, this,
                             &FlowChartEdgeItem::updatePath);
        }
    }

    FlowChartShapeItem* FlowChartEdgeItem::getSource() const { return m_source; }

    void FlowChartEdgeItem::setTarget(FlowChartShapeItem* target) {
        Q_ASSERT(target);
        if (m_target != nullptr) {
            QObject::disconnect(m_target, &FlowChartShapeItem::positionChanged, this,
                                &FlowChartEdgeItem::updatePath);
        }
        m_target = target;
        if (m_target != nullptr) {
            QObject::connect(m_target, &FlowChartShapeItem::positionChanged, this,
                             &FlowChartEdgeItem::updatePath);
        }
    }

    FlowChartShapeItem* FlowChartEdgeItem::getTarget() const { return m_target; }

    void FlowChartEdgeItem::setPathType(PathType type) { m_pathType = type; }

    void FlowChartEdgeItem::setSelfLoop(bool isSelfLoop) { m_isSelfLoop = isSelfLoop; }

    bool FlowChartEdgeItem::getSelfLoop() const { return m_isSelfLoop; }

    void FlowChartEdgeItem::setControlPoints(const QVector<QPointF>& points) {
        m_controlPointItems.clear();
        m_controlPoints = points;
        updateControlPointsVisibility(m_scene->showControlPoints());
    }

    void FlowChartEdgeItem::setHorizontal(bool isHorizontal) { m_isHorizontal = isHorizontal; }

    QRectF FlowChartEdgeItem::boundingRect() const {
        QRectF rect = path().boundingRect();
        for (const QPolygonF& arrow : m_arrows) {
            rect = rect.united(arrow.boundingRect());
        }

        constexpr qreal margin = 5.0;
        constexpr qreal posPercent = 0.5;
        constexpr qreal boundingMargin = 10.0;

        if (!m_label.isEmpty()) {
            QFontMetricsF qfm(m_font);
            qreal textWidth = qfm.horizontalAdvance(m_label);
            QRectF textBounds = qfm.boundingRect(m_label);
            qreal textHeight = textBounds.height();
            qreal ascent = qfm.ascent();
            QPointF centerPos = path().pointAtPercent(posPercent);
            QRectF textRect(centerPos.x() - (textWidth / 2) - margin,
                            centerPos.y() - ascent - margin, textWidth + (2 * margin),
                            textHeight + (2 * margin));
            rect = rect.united(textRect);
        }

        rect.adjust(-boundingMargin, -boundingMargin, boundingMargin, boundingMargin);
        return rect;
    }

    QPointF FlowChartEdgeItem::computeStartEndPoints(bool isStart) const {
        if (m_source == nullptr || m_target == nullptr) {
            return {0, 0};
        }

        QPointF sourcePos = m_source->scenePos();
        QPointF targetPos = m_target->scenePos();

        if (isStart) {
            // Use boundary points in single-out
            if (m_source->getOutDegree() == 1 && !m_isSelfLoop) {
                return m_isHorizontal ? m_source->getHorizontalEdge(true)
                                      : m_source->getVerticalEdge(true);
            }
            return m_source->getEdgeIntersection(sourcePos, targetPos);
        } 
        // Use boundary points in single-in
        if (m_target->getInDegree() == 1 && !m_isSelfLoop) {
            return m_isHorizontal ? m_target->getHorizontalEdge(false)
                                    : m_target->getVerticalEdge(false);
        }
        return m_target->getEdgeIntersection(targetPos, sourcePos);
        
    }

    void FlowChartEdgeItem::updatePath() { // NOLINT(readability-function-cognitive-complexity)
        if (m_source == nullptr || m_target == nullptr) {
            return;
        }

        QPainterPath path;
        QPointF start = computeStartEndPoints(true);
        QPointF end = computeStartEndPoints(false);

        qreal lineWidth = (m_style >= EdgeStyle::THICKSOLID) ? m_width * 2 : m_width;
        qreal scaleFactor = (qFuzzyCompare(m_width, 1.0)) ? m_arrowScaleFactor : m_width;
        qreal arrowLength = m_baseArrowLength * scaleFactor;
        qreal offset = (lineWidth / 2) + (arrowLength / 3);

        bool isAdjustVertex =
            (m_style == EdgeStyle::THICKSOLIDARROW || m_style == EdgeStyle::THICKDASHEDARROW ||
             m_style == EdgeStyle::THICKSOLIDDOUBLEARROW ||
             m_style == EdgeStyle::THICKDASHEDDOUBLEARROW);
        bool isSourceVirtual = m_source->isVirtual();
        bool isTargetVirtual = m_target->isVirtual();
        bool isSingleArrow =
            (m_style == EdgeStyle::THICKSOLIDARROW || m_style == EdgeStyle::THICKDASHEDARROW);

        QPointF adjustedStart = start;
        QPointF adjustedEnd = end;

        if (m_isSelfLoop) {
            QRectF rect = m_source->boundingRect();
            QPointF topLeft = rect.topLeft();
            constexpr qreal ARC_LEFT_OFFSET = 20.0;
            constexpr qreal ARC_SIZE = 40.0;
            QRectF arcRect(topLeft + QPointF(-ARC_LEFT_OFFSET, -ARC_SIZE),
                           QSizeF(ARC_SIZE, ARC_SIZE));
            path.moveTo(adjustedStart);
            constexpr qreal ARC_ANGLE = 360.0;
            path.arcTo(arcRect, 0, ARC_ANGLE);
            path.lineTo(adjustedEnd);
        } else if (!m_controlPoints.isEmpty()) {
            if (isAdjustVertex) {
                // Source side compensation (required only if non-virtual nodes or double arrows)
                if (!isSourceVirtual || !isSingleArrow) {
                    QPointF sourceDir = m_controlPoints[0] - start;
                    qreal sourceLength = QLineF(start, m_controlPoints[0]).length();
                    if (sourceLength > 0) {
                        sourceDir /= sourceLength;
                        adjustedStart += sourceDir * offset;
                    }
                }
                // Target side compensation (required only if non-virtual nodes or double arrows)
                if (!isTargetVirtual || !isSingleArrow) {
                    QPointF targetDir = end - m_controlPoints.last();
                    qreal targetLength = QLineF(m_controlPoints.last(), end).length();
                    if (targetLength > 0) {
                        targetDir /= targetLength;
                        adjustedEnd -= targetDir * offset;
                    }
                }
            }
            path.moveTo(adjustedStart);
            if (m_pathType == PathType::BEZIER) {
                if (m_controlPoints.size() == 1) {
                    // Quadratic Bezier curve of single control point
                    path.quadTo(m_controlPoints[0], adjustedEnd);
                } else if (m_controlPoints.size() == 2) {
                    // Cubic Bezier curve of two control points
                    path.cubicTo(m_controlPoints[0], m_controlPoints[1], adjustedEnd);
                } else {
                    constexpr qreal CURVE_FACTOR = 0.33;
                    // Multi-control points still use segmented quadratic Bessel
                    for (int i = 0; i < m_controlPoints.size() - 1; i++) {
                        QPointF ctrP1 = m_controlPoints[i];
                        QPointF ctrP2 = m_controlPoints[i + 1];
                        QPointF midP = (ctrP1 + ctrP2) / 2;
                        QPointF ctrl1 = ctrP1 + (midP - ctrP1) * CURVE_FACTOR;
                        QPointF ctrl2 = ctrP2 - (ctrP2 - midP) * CURVE_FACTOR;
                        path.quadTo(ctrl1, midP);
                        path.quadTo(ctrl2, ctrP2);
                    }
                    path.lineTo(adjustedEnd);
                }
            } else {
                // Folding line
                for (const QPointF& point : m_controlPoints) {
                    path.lineTo(point);
                }
                path.lineTo(adjustedEnd);
            }
        } else {
            if (isAdjustVertex) {
                QPointF dir = end - start;
                qreal length = QLineF(start, end).length();
                if (length > 0) {
                    dir /= length;
                    adjustedStart += dir * offset;
                    adjustedEnd -= dir * offset;
                }
            }
            path.moveTo(adjustedStart);
            path.lineTo(adjustedEnd);
        }

        setPath(path);
        updateArrowHeads();
    }

    void FlowChartEdgeItem::updateControlPointsVisibility(bool isVisible) {
        // Clean up existing control points
        for (FlowChartCtrlPointItem* item : m_controlPointItems) {
            m_scene->removeItem(item);
            delete item;
        }
        m_controlPointItems.clear();

        // Display control points
        if (isVisible && !m_controlPoints.isEmpty()) {
            drawControlPoints();
        }
    }

    void FlowChartEdgeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                                  QWidget* widget) {
        if (m_style == EdgeStyle::INVISIBLE) {
            return;
        }

        constexpr qreal margin = 5.0;
        constexpr qreal posPercent = 0.5;
        const QColor LightGray = QColor(230, 230, 230);

        QPen pen;
        pen.setWidthF(m_width);
        pen.setColor(Qt::black);
        switch (m_style) {
        case EdgeStyle::DASHED:
        case EdgeStyle::DASHEDARROW:
        case EdgeStyle::THICKDASHED:
        case EdgeStyle::THICKDASHEDARROW:
        case EdgeStyle::DASHEDDOUBLEARROW:
        case EdgeStyle::THICKDASHEDDOUBLEARROW:
            pen.setStyle(Qt::DashLine);
            break;
        default:
            pen.setStyle(Qt::SolidLine);
        }

        if (m_style >= EdgeStyle::THICKSOLID) {
            pen.setWidthF(m_width * 2);
        }

        painter->setPen(pen);
        painter->drawPath(path());

        painter->setBrush(Qt::black);
        painter->setPen(Qt::NoPen);
        for (const QPolygonF& arrow : m_arrows) {
            painter->drawPolygon(arrow);
        }

        if (!m_label.isEmpty()) {
            painter->setPen(Qt::black);
            QFontMetricsF qfm(m_font);
            qreal textWidth = qfm.horizontalAdvance(m_label);
            QRectF textBounds = qfm.boundingRect(m_label);
            qreal textHeight = textBounds.height();
            qreal ascent = qfm.ascent();

            QPointF centerPos = path().pointAtPercent(posPercent);

            painter->setBrush(LightGray);
            painter->setPen(Qt::NoPen);

            QRectF textRect(centerPos.x() - (textWidth / 2) - margin,
                            centerPos.y() - ascent - margin, textWidth + (2 * margin),
                            textHeight + (2 * margin));
            QPointF textPos(centerPos.x() - (textWidth / 2), centerPos.y());

            painter->drawRect(textRect);
            painter->setPen(Qt::black);
            painter->drawText(textPos, m_label);
        }

        if (isSelected()) {
            painter->setPen(QPen(Qt::blue, 1, Qt::DashLine));
            painter->drawPath(shape());
        }
    }

    void FlowChartEdgeItem::drawControlPoints() {
        if (m_controlPoints.isEmpty()) {
            m_controlPointItems.clear();
            return;
        }
        constexpr qreal defaultSize = 2.0;
         // NOLINTNEXTLINE(modernize-loop-convert)
        for (int idx = 0; idx < m_controlPoints.size(); idx++) {
            auto* item = new FlowChartCtrlPointItem(m_controlPoints[idx], this);
            m_scene->addItem(item);
            m_controlPointItems.append(item);
            QObject::connect(item, &FlowChartCtrlPointItem::eventCtrlPointMoved, this,
                             [idx, this](QPointF pos) {
                                 m_controlPoints[idx] = pos;
                                 updatePath();
                             });
        }
    }

    void FlowChartEdgeItem::updateArrowHeads() { // NOLINT(readability-function-cognitive-complexity)
        m_arrows.clear();
        if (m_source == nullptr || m_target == nullptr) {
            return;
        }

        QPointF start = computeStartEndPoints(true);
        QPointF end = computeStartEndPoints(false);
        bool isTargetVirtual = m_target->isVirtual();

        auto getArrowAngle = [&](const QPointF& vertex, bool isTarget) {
            if (path().isEmpty()) {
                qDebug() << "Warning: Empty path in updateArrowHeads";
                return 0.0;
            }
            if (!m_controlPoints.isEmpty() && path().elementCount() > 2) {
                qreal percent = isTarget ? 1.0 : 0.0;
                qreal slope = path().slopeAtPercent(percent);
                qreal angleRad = qAtan(slope);
                qreal angleDeg = qRadiansToDegrees(angleRad);
                constexpr qreal mirrorAngle = 180.0;
                if (isTarget) {
                    QPointF dir =
                        (m_controlPoints.isEmpty() ? start : m_controlPoints.last()) - end;
                    if (dir.x() * qCos(qDegreesToRadians(angleDeg)) +
                            dir.y() * qSin(qDegreesToRadians(angleDeg)) >
                        0) {
                        angleDeg += mirrorAngle;
                    }
                } else {
                    QPointF dir =
                        start - (m_controlPoints.isEmpty() ? end : m_controlPoints.first());
                    if (dir.x() * qCos(qDegreesToRadians(angleDeg)) +
                            dir.y() * qSin(qDegreesToRadians(angleDeg)) <
                        0) {
                        angleDeg += mirrorAngle;
                    }
                }
#ifdef FLOW_EDGE_DEBUG
                qDebug() << "Arrow angle:" << angleDeg << "for edge" << m_source->getLabel() << "->"
                         << m_target->getLabel() << "isTarget:" << isTarget
                         << "pathType:" << (m_pathType == PathType::BEZIER ? "BEZIER" : "POLYLINE");
#endif
                return angleDeg;
            } 
            QPointF dir = isTarget ? end - start : start - end;
            qreal angleDeg = qRadiansToDegrees(qAtan2(dir.y(), dir.x()));
#ifdef FLOW_EDGE_DEBUG
            qDebug() << "Straight arrow angle:" << angleDeg << "for edge"
                        << m_source->getLabel() << "->" << m_target->getLabel()
                        << "isTarget:" << isTarget;
#endif
            return angleDeg;
            
        };

        switch (m_style) {
        case EdgeStyle::SOLIDARROW:
        case EdgeStyle::DASHEDARROW:
        case EdgeStyle::THICKSOLIDARROW:
        case EdgeStyle::THICKDASHEDARROW: {
            if (isTargetVirtual) {
                // 目标为虚拟节点：箭头在源节点
                qreal sourceAngle = getArrowAngle(start, false);
                m_arrows << calculateArrowHead(start, sourceAngle);
            } else {
                // 正常情况：箭头在目标节点
                qreal targetAngle = getArrowAngle(end, true);
                m_arrows << calculateArrowHead(end, targetAngle);
            }
            break;
        }
        case EdgeStyle::SOLIDDOUBLEARROW:
        case EdgeStyle::DASHEDDOUBLEARROW:
        case EdgeStyle::THICKSOLIDDOUBLEARROW:
        case EdgeStyle::THICKDASHEDDOUBLEARROW: {
            qreal targetAngle = getArrowAngle(end, true);
            m_arrows << calculateArrowHead(end, targetAngle);
            qreal sourceAngle = getArrowAngle(start, false);
            m_arrows << calculateArrowHead(start, sourceAngle);
            break;
        }
        default:
            break;
        }
    }

    QPolygonF FlowChartEdgeItem::calculateArrowHead(const QPointF& vertex, qreal angleDeg) {
        constexpr qreal arrowAngleThreshold = 90.0;
        // 线宽为 1 时缩小到 30%，其他情况按比例调整
        qreal scaleFactor = (qFuzzyCompare(m_width, 1.0)) ? m_arrowScaleFactor : m_width;
        qreal arrowLength = m_baseArrowLength * scaleFactor;
        qreal arrowWidth = m_baseArrowWidth * scaleFactor;

        QPointF arrowDir;
        QPointF perp;

        if (qFuzzyCompare(angleDeg, 0.0)) {
            arrowDir = QPointF(1.0, 0.0);
            perp = QPointF(0.0, 1.0);
        } else if (qFuzzyCompare(angleDeg, arrowAngleThreshold)) {
            arrowDir = QPointF(0.0, 1.0);
            perp = QPointF(-1.0, 0.0);
        } else if (qFuzzyCompare(angleDeg, -arrowAngleThreshold)) {
            arrowDir = QPointF(0.0, -1.0);
            perp = QPointF(1.0, 0.0);
        } else {
            qreal rad = qDegreesToRadians(angleDeg);
            arrowDir = QPointF(qCos(rad), qSin(rad));
            perp = QPointF(-arrowDir.y(), arrowDir.x());
        }

        QPolygonF arrow;
        arrow << vertex << (vertex - arrowDir * arrowLength + perp * arrowWidth)
              << (vertex - arrowDir * arrowLength - perp * arrowWidth);
        return arrow;
    }

    /// --- FlowChartConnector ---
    FlowChartConnector::FlowChartConnector(FlowChartScene* scene, FlowChartModel* model,
                                           QObject* parent)
        : m_model(model), m_scene(scene), QObject(parent) {
        constexpr qreal OFFSET_BASE = 15.0;
        constexpr qreal OFFSET_EXTRA = 30.0;
        constexpr qreal OFFSET_FACTOR = 2.0;
        constexpr qreal GAP_MIN_FACTOR = 0.3;
        constexpr qreal GAP_MAX_FACTOR = 0.4;
        m_edgeItems = QVector<FlowChartEdgeItem*>();
        m_fullEdges = QVector<FullEdge*>();       // Full edge mapping QVector<FullEdge>;
        m_vertexPairs = QVector<EdgeEndpoints>(); // Edge endpoints mapping QVector<EdgeEndpoints>;
        m_nodes = QSet<FlowChartShapeItem*>();
        m_offsetBase = OFFSET_BASE;
        m_offsetExtra = OFFSET_EXTRA;
        m_offsetFactor = OFFSET_FACTOR;
        m_gapMinFactor = GAP_MIN_FACTOR;
        m_gapMaxFactor = GAP_MAX_FACTOR;
    };

    FlowChartConnector::~FlowChartConnector() {
        clear(); // Reuse existing clear logic
    }

    void FlowChartConnector::clear() {
        for (FlowChartEdgeItem* edgeItem : m_edgeItems) {
            m_scene->removeItem(edgeItem);
            delete edgeItem;
        }
        m_edgeItems.clear();
        // clear edge item pair map
        for (FullEdge* edge : m_fullEdges) {
            delete edge;
        }
        m_fullEdges.clear();
        // clear vertex pair map
        m_vertexPairs.clear();
        // clear the nodes
        m_nodes.clear();
    }

    void FlowChartConnector::updateAll() {
        clear();
        collectEdges(m_model);
        computeDegrees();
        renderEdges();
    }

    void FlowChartConnector::computeDegrees() {
        // 重置所有节点的度数
        for (FlowChartShapeItem* node : m_nodes) {
            node->setInDegree(0);
            node->setOutDegree(0);
        }

        // 统计度数
        for (const EdgeEndpoints& pair : m_vertexPairs) {
            pair.source->setOutDegree(pair.source->getOutDegree() + 1);
            pair.target->setInDegree(pair.target->getInDegree() + 1);
        }

#ifdef FLOW_EDGE_DEBUG
        qDebug() << "Computed degrees for connector";
        for (FlowChartShapeItem* node : m_nodes) {
            qDebug() << "Node" << node->getLabel() << ": inDegree=" << node->getInDegree()
                     << ", outDegree=" << node->getOutDegree();
        }
#endif
    }

    PathType FlowChartConnector::determinePathType(FlowChartShapeItem* source,
                                                   FlowChartShapeItem* target, int iEdgeCount) {
        if (iEdgeCount > 1) {
            return PathType::BEZIER; // Multiple edges and the coordinates of both ends are the same, use the Bezier curve
        }
        QPointF sourcePos = source->pos();
        QPointF targetPos = target->pos();
        if (qFuzzyCompare(sourcePos.x(), targetPos.x()) ||
            qFuzzyCompare(sourcePos.y(), targetPos.y())) {

            return PathType::POLYLINE; // Any coordinate dimensions are equal, use straight lines
        }
        return PathType::BEZIER; // Otherwise use Bezier curve
    }

    void FlowChartConnector::collectEdges(FlowChartModel* model) {
#ifdef FLOW_EDGE_DEBUG
        qDebug() << "Collecting edges from model: " << m_model->modelID();
#endif
        for (const FlowEdge* edge : model->edges()) {
            FlowNode* sourceNode = edge->source;
            FlowNode* targetNode = edge->target;
            // Check if the edge is valid
            if (sourceNode == nullptr || targetNode == nullptr) {
                continue;
            }
            QGraphicsItem* sourceItem = m_scene->getNodeItem(sourceNode);
            QGraphicsItem* targetItem = m_scene->getNodeItem(targetNode);
#ifdef FLOW_EDGE_DEBUG
            qDebug() << "Try to collect edge" << sourceNode->id << "->" << targetNode->id;
#endif
            // Check if the edge item is valid
            if (sourceItem == nullptr || targetItem == nullptr) {
                continue;
            }
            // Check if the edge already exists
            auto* src = dynamic_cast<FlowChartShapeItem*>(sourceItem);
            auto* tgt = dynamic_cast<FlowChartShapeItem*>(targetItem);
            // Check the convert Style is valid
            if (src == nullptr || tgt == nullptr) {
                continue;
            }

            // Collect nodes
            m_nodes.insert(src);
            m_nodes.insert(tgt);

            auto* fullEdge =
                new FullEdge{EdgeEndpoints{src, tgt}, edge->style, edge->label, PathType::POLYLINE};
            // Insert into full edge list and edge item pair map
            m_fullEdges.append(fullEdge);
            EdgeEndpoints pair{src, tgt}; // It must not exist
            Q_ASSERT(std::find(m_vertexPairs.begin(), m_vertexPairs.end(), pair) ==
                     m_vertexPairs.end());
            m_vertexPairs.append(pair);
#ifdef FLOW_EDGE_DEBUG
            qDebug() << "Collected edge" << src->getLabel() << "->" << tgt->getLabel();
#endif
        }
        for (FlowChartModel* subModel : model->subgraphs().values()) {
            collectEdges(subModel);
        }
    }

    void FlowChartConnector::renderEdges() {

        for (FullEdge* edge : m_fullEdges) {
            auto* edgeItem = new FlowChartEdgeItem(m_scene, nullptr);
            FlowChartShapeItem* sourceNode = edge->source;
            FlowChartShapeItem* targetNode = edge->target;
            bool isSelfLoop = isEdgeSelfLoop(sourceNode, targetNode);

            int iEdgeIndex = getEdgeIndex(sourceNode, targetNode);
            int iEdgeCount = getEdgeCount(sourceNode, targetNode);

            PathType pathType = determinePathType(sourceNode, targetNode, iEdgeCount);
            edge->pathType = pathType;

            edgeItem->setSource(sourceNode);
            edgeItem->setTarget(targetNode);
            edgeItem->setSelfLoop(isSelfLoop);
            edgeItem->setEdgeStyle(edge->style);
            edgeItem->setLabel(edge->label);
            edgeItem->setPathType(pathType);
            edgeItem->setHorizontal(m_model->effectiveLayoutDirection() ==
                                    LayoutDirection::HORIZONTAL);

            QVector<QPointF> controlPoints;
            if (pathType == PathType::BEZIER) {
                controlPoints =
                    calculateControlPoints(edgeItem, iEdgeIndex, iEdgeCount, isSelfLoop);
                edgeItem->setControlPoints(controlPoints);
            }

            edgeItem->updatePath();
            m_scene->addItem(edgeItem);
            m_edgeItems.append(edgeItem);
#ifdef FLOW_EDGE_DEBUG
            qDebug() << "Rendered edge" << sourceNode->getLabel() << "->" << targetNode->getLabel()
                     << "Source pos:" << sourceNode->pos() << "Target pos:" << targetNode->pos()
                     << "pathType:" << (pathType == PathType::BEZIER ? "BEZIER" : "POLYLINE")
                     << "iEdgeCount:" << iEdgeCount << "iEdgeIndex:" << iEdgeIndex
                     << "Control points:" << controlPoints;
#endif
        }
    }

    QVector<QPointF> FlowChartConnector::calculateControlPoints(const FlowChartEdgeItem* edgeItem, // NOLINT(readability-function-cognitive-complexity)
                                                                int iEdgeIndex, int iEdgeCount,
                                                                bool isSelfLoop) const {
        QVector<QPointF> controlPoints;

        // Get node location and boundary
        FlowChartShapeItem* source = edgeItem->getSource();
        FlowChartShapeItem* target = edgeItem->getTarget();
        QPointF sourcePos = source->scenePos();
        QPointF targetPos = target->scenePos();
        bool isHorizontal = (m_model->effectiveLayoutDirection() == LayoutDirection::HORIZONTAL);

        int edgeCount = qMin(iEdgeCount, 2); // Up to two sides (A->B, B->A)

        // Self-ring connection
        if (isSelfLoop) {
            QRectF sourceRect = source->boundingRect().translated(sourcePos);
            QPointF topRight = sourceRect.topRight();
            constexpr qreal offsetFactor = 0.2;
            qreal loopOffset = qMax(sourceRect.width(), sourceRect.height()) * offsetFactor;
            qreal offset = (iEdgeIndex - (edgeCount / m_offsetFactor)) * m_offsetBase;
            if (isHorizontal) {
                controlPoints << QPointF(topRight.x() + loopOffset,
                                         topRight.y() - loopOffset + offset);
            } else {
                controlPoints << QPointF(topRight.x() - loopOffset + offset,
                                         topRight.y() + loopOffset);
            }
            return controlPoints;
        }

        // Calculate the starting point and end point
        QPointF start = edgeItem->computeStartEndPoints(true);
        QPointF end = edgeItem->computeStartEndPoints(false);

        // Calculate bias
        qreal yDiff = qAbs(start.y() - end.y());
        constexpr qreal offsetFactor1 = 0.8;
        constexpr qreal offsetFactor2 = 0.6;
        qreal offset1 = yDiff * offsetFactor1; // Bias 1: 80% of y difference
        qreal offset2 = yDiff * offsetFactor2; // Bias 2: 60% of y difference
        qreal bendFactor = (iEdgeIndex - (iEdgeCount / m_offsetFactor)) * m_offsetBase;
        qreal bendDirection = (iEdgeIndex == 0) ? 1.0 : -1.0;

        // Determine the node type
        bool isSourceVirtual = source->isVirtual();
        bool isTargetVirtual = target->isVirtual();

        // Calculate control points
        QPointF controlPoint;

        // Determine whether it is multiple edges
        if (edgeCount > 1) { // Handle multiple edges (iEdgeCount > 1)
            // contrPntOffset, since there are reciprocating edges, namely A->B, B->A, the offset of the control point needs to be adjusted according to the direction of the edge.
            //For A->B, the control point is on the right side of B, so it needs to be offset to the right; for B->A, the control point is on the left side of A, so it needs to be offset to the left
            constexpr qreal contrPntOffsetFactor = 1.2;
            qreal contrPntOffset = bendFactor * bendDirection * contrPntOffsetFactor;
            if (isSourceVirtual && isTargetVirtual) {
                controlPoint = (start + end) / 2;
                if (isHorizontal) {
                    controlPoint.setY(controlPoint.y() + contrPntOffset);
                } else {
                    controlPoint.setX(controlPoint.x() + contrPntOffset);
                }
            } else if (isSourceVirtual != isTargetVirtual) {
                if (isSourceVirtual) {
                    // The source is a virtual node
                    if (isHorizontal) {
                        controlPoint.setY(sourcePos.y() + contrPntOffset);
                        controlPoint.setX(sourcePos.x() + offset2);
                    } else {
                        controlPoint.setX(sourcePos.x() + contrPntOffset);
                        controlPoint.setY(sourcePos.y() + offset2);
                    }
                } else {
                    // The target is a virtual node
                    if (isHorizontal) {
                        controlPoint.setY(targetPos.y() + contrPntOffset);
                        controlPoint.setX(targetPos.x() - offset2);
                    } else {
                        controlPoint.setX(targetPos.x() + contrPntOffset);
                        controlPoint.setY(targetPos.y() - offset2);
                    }
                }
            } else {
                // Normal node
                if (isHorizontal) {
                    controlPoint.setY(end.y() + contrPntOffset);
                    controlPoint.setX(end.x() - offset1);
                } else {
                    controlPoint.setX(end.x() + contrPntOffset);
                    controlPoint.setY(end.y() - offset1);
                }
            }

        } else {
            if (isSourceVirtual && isTargetVirtual) {
                // Dual virtual nodes: The control point is at the midpoint of the line connecting the starting point and the end point
                controlPoint = (start + end) / 2;
            } else if (isSourceVirtual != isTargetVirtual) {
                // Single virtual node
                if (isSourceVirtual) {
                    // The source is a virtual node
                    if (isHorizontal) {
                        controlPoint.setY(sourcePos.y());
                        controlPoint.setX(sourcePos.x() + offset2);
                    } else {
                        controlPoint.setX(sourcePos.x());
                        controlPoint.setY(sourcePos.y() + offset2);
                    }
                } else {
                    // The target is a virtual node
                    if (isHorizontal) {
                        controlPoint.setY(targetPos.y());
                        controlPoint.setX(targetPos.x() - offset2);
                    } else {
                        controlPoint.setX(targetPos.x());
                        controlPoint.setY(targetPos.y() - offset2);
                    }
                }
            } else {
                // Normal node
                if (isHorizontal) {
                    controlPoint.setY(end.y());
                    controlPoint.setX(end.x() - offset1);
                } else {
                    controlPoint.setX(end.x());
                    controlPoint.setY(end.y() - offset1);
                }
            }
        }
        // Join the control point
        controlPoints << controlPoint;

#ifdef FLOW_EDGE_DEBUG
        qDebug() << "Control points for edge" << source->getLabel() << "->" << target->getLabel()
                 << ": start=" << start << ", end=" << end << ", controlPoints=" << controlPoints
                 << ", isSourceVirtual=" << isSourceVirtual
                 << ", isTargetVirtual=" << isTargetVirtual << ", offset1=" << offset1
                 << ", offset2=" << offset2 << ", bendFactor=" << bendFactor
                 << ", bendDirection=" << bendDirection
                 << ", sourceOutDegree=" << source->getOutDegree()
                 << ", targetInDegree=" << target->getInDegree();
#endif

        return controlPoints;
    }

    bool FlowChartConnector::isEdgeSelfLoop(FlowChartShapeItem* source,
                                            FlowChartShapeItem* target) {
        return source == target;
    }

    int FlowChartConnector::getEdgeIndex(FlowChartShapeItem* source, FlowChartShapeItem* target) {
        int index = -1;
        EdgeEndpoints key1{source, target};
        EdgeEndpoints key2{target, source};
        QVector<EdgeEndpoints> tempRecords = QVector<EdgeEndpoints>();
        for (auto& pair : m_vertexPairs) {
            if (pair == key1 || pair == key2) {
                tempRecords.append(pair);
            }
        }
        return tempRecords.indexOf(key1);
    }

    int FlowChartConnector::getEdgeCount(FlowChartShapeItem* source,
                                         FlowChartShapeItem* target) const {
        int count = 0;
        EdgeEndpoints key1{source, target};
        EdgeEndpoints key2{target, source};
        for (const auto& pair : m_vertexPairs) {
            if (pair == key1 || pair == key2) {
                count++;
            }
        }
        return count;
    }

} // namespace hbchart
