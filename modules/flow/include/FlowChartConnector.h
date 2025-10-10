/**
 * File: FlowChartConnector.h
 * Description: The declaration of the FlowChartConnector class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */
#ifndef FLOWCHARTCONNECTOR_H
#define FLOWCHARTCONNECTOR_H

#include <QGraphicsPathItem>
#include <QHash>
#include <QPolygonF>
#include <QSet>

#include "FlowChartModel.h"
#include "FlowChartScene.h"
#include "FlowChartShapeItem.h"

namespace hbchart {

    /**
     * @brief The PathType enum
     */
    enum class PathType : std::uint8_t {
        POLYLINE, ///< Polyline path
        BEZIER    ///< Bezier path
    };

    /**
     * @brief HalfEdge
     * @details Half-Edge includes target node, line type, and description text
     */
    struct HalfEdge {                  ///< Half-Edge
        FlowChartShapeItem* otherNode; ///< Target node
        FlowChartShapeItem* thisNode;  ///< Previous node
        EdgeStyle style;               ///< Line type
        QString label;                 ///< Description text
        PathType pathType;             ///< Path type

        /**
         * @brief Equality operator
         * @param other Other HalfEdge object
         * @return Whether the two HalfEdge objects are equal
         */
        bool operator==(const HalfEdge& other) const {

            return otherNode == other.otherNode && thisNode == other.thisNode &&
                   style == other.style && label == other.label && pathType == other.pathType;
        }
    };

    /**
     * @brief EdgeEndpoints
     * @details Edge endpoints include source node and target node
     */
    struct EdgeEndpoints {
        FlowChartShapeItem* source;
        FlowChartShapeItem* target;

        bool operator==(const EdgeEndpoints& other) const {
            return source == other.source && target == other.target;
        }

        //Optional: Support hash (such as you want to use it for QHash/QMap, etc.)
        friend size_t qHash(const EdgeEndpoints& key, size_t seed = 0) {
            return qHash(key.source, seed) ^ qHash(key.target, seed);
        }
    };

    /**
     * @brief FullEdge
     * @details Full-Edge information includes source node, target node, line type, and description
     * text
     */
    struct FullEdge {               ///< Full-Edge
        FlowChartShapeItem* source; ///< Source node
        FlowChartShapeItem* target; ///< Target node
        EdgeStyle style;            ///< Line type
        QString label;              ///< Description text
        PathType pathType;          ///< Path type

        /**
         * @brief Constructor
         * @param endpoints Source and target nodes
         * @param style Line type
         * @param label Description text
         * @param pathType Path type
         */
        FullEdge(const EdgeEndpoints& endpoints, EdgeStyle style, QString label,
                 PathType pathType = PathType::POLYLINE)
            : source(endpoints.source), target(endpoints.target), style(style),
              label(std::move(label)), pathType(pathType) {}

        /**
         * @brief Equality operator
         * @param other Other FullEdge object
         * @return Whether the two FullEdge objects are equal
         */
        bool operator==(const FullEdge& other) const {

            return source == other.source && target == other.target &&
                   (style == other.style || label == other.label) && pathType == other.pathType;
        }
    };

    /**
     * @brief FlowChartCtrlPointItem
     * @details Control point item used to adjust the position of the connection line
     */
    class FlowChartCtrlPointItem : public QObject, public QGraphicsEllipseItem {
        Q_OBJECT

      signals:
        void eventCtrlPointMoved(QPointF pos);

      public:
        /**
         * @brief Constructor
         * @param parent Parent item
         */
        FlowChartCtrlPointItem(const QPointF& pos, QGraphicsItem* parent = nullptr);
        ~FlowChartCtrlPointItem() override = default;

      protected:
        QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    };

    /**
     *@brief flow graph connection line class
     *@details Inherited from QGraphicsPathItem, used to draw flow graph connection lines
     *@note The anchor position of the connection line is determined by the source node and the
     *target node. Note that QGraphicsPathItem is not inherited from QObject
     */
    class FlowChartEdgeItem : public QObject, public QGraphicsPathItem {
        Q_OBJECT

      private:
        FlowChartScene* m_scene;                              ///< Flow chart model
        QString m_label;                                      ///< Description text
        FlowChartShapeItem* m_source;                         ///< Source node
        FlowChartShapeItem* m_target;                         ///< Target node
        EdgeStyle m_style;                                    ///< Connection line style
        PathType m_pathType;                                  ///< Path type
        QFont m_font;                                         ///< Font
        qreal m_width;                                        ///< Connection line width
        qreal m_baseArrowLength;                              ///< Base arrow length
        qreal m_baseArrowWidth;                               ///< Base arrow width
        qreal m_arrowScaleFactor;                             ///< Arrow scale factor
        QVector<QPolygonF> m_arrows;                          ///< Arrows polygons
        bool m_isSelfLoop;                                    ///< Self-loop
        QVector<QPointF> m_controlPoints;                     ///< Control points
        QVector<FlowChartCtrlPointItem*> m_controlPointItems; ///< Control point items
        bool m_isHorizontal; ///< Whether the connection line is horizontal

      signals:
        void eventCtrlPointMoved(QPointF pos);

      public:
        /**
         * @brief Constructor
         * @param parent Parent item
         */
        explicit FlowChartEdgeItem(FlowChartScene* scene = nullptr,
                                   QGraphicsItem* parent = nullptr);
        ~FlowChartEdgeItem() override = default;

        /**
         * @brief Set the label of the connection line
         * @param label Label text
         */
        void setLabel(const QString& label);

        /**
         * @brief Set line style
         * @param style Line style
         */
        void setEdgeStyle(EdgeStyle style);

        /**
         * @brief Set the source node and anchor point direction
         * @param source Source node
         * @param sourceDir Source anchor point direction
         */
        void setSource(FlowChartShapeItem* source);

        /**
         * @brief Get the source node
         * @return Source node
         */
        [[nodiscard]] FlowChartShapeItem* getSource() const;

        /**
         * @brief Set the target node and anchor point direction
         * @param target Target node
         * @param targetDir Target anchor direction
         */
        void setTarget(FlowChartShapeItem* target);

        /**
         * @brief Get the target node
         * @return Target node
         */
        [[nodiscard]] FlowChartShapeItem* getTarget() const;

        /**
         * @brief Set the connection line type (path type)
         * @param type Path type
         */
        void setPathType(PathType type);

        /**
         * @brief Set up self-loop
         * @param isSelfLoop Whether it is self-loop
         */
        void setSelfLoop(bool isSelfLoop);

        /**
         * @brief Get whether it is self-loop
         * @return Whether it is self-loop
         */
        [[nodiscard]] bool getSelfLoop() const;

        /**
         * @brief Set the control points of the connection line
         * @param points control points
         */
        void setControlPoints(const QVector<QPointF>& points);

        /**
         * @brief Set the edge layout direction
         * @param isHorizontal Whether the edge is horizontal
         */
        void setHorizontal(bool isHorizontal);

        /**
         * @brief Get the control points of the connection line
         * @return Control points
         */
        [[nodiscard]] QRectF boundingRect() const override;

        /**
         * @brief Get the connection line start point and end point
         * @return Connection line start point and end point
         */
        [[nodiscard]] QPointF computeStartEndPoints(bool isStart) const;

        /**
         * @brief update the connection line path
         * @details Recalculate the connection line path based on the source and target nodes and
         * anchor points
         */
        void updatePath();

      protected:
        /**
         * @brief update the control points visibility
         * @param isVisible Whether the control points are visible
         */
        void updateControlPointsVisibility(bool isVisible);

        /**
         * @brief paint event
         * @param painter Painter
         * @param option Graphics option
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief draw control points
         * @details Draw the control points of the connection line
         */
        void drawControlPoints();

      private:
        /**
         * @brief Update the arrow heads
         * @details Caculate the arrow head path and add it to the path
         */
        void updateArrowHeads();

        /**
         * @brief Calculate the arrow head path
         * @param vertex The vertex of the arrow
         * @param angleDeg The angle of the arrow
         * @return The arrow head path
         */
        QPolygonF calculateArrowHead(const QPointF& vertex, qreal angleDeg);
    };

    class FlowChartConnector : public QObject {
        Q_OBJECT

      private:
        FlowChartScene* m_scene = nullptr;       ///< Flow chart scene
        FlowChartModel* m_model = nullptr;       ///< Flow chart model
        QVector<FlowChartEdgeItem*> m_edgeItems; ///< Connection line list
        QSet<FlowChartShapeItem*> m_nodes;       ///< Connected nodes
        QVector<FullEdge*> m_fullEdges;          ///< Full-Edge list
        QVector<EdgeEndpoints> m_vertexPairs;    ///< Edge pair map
        qreal m_offsetBase;
        qreal m_offsetExtra;
        qreal m_offsetFactor;
        qreal m_gapMinFactor;
        qreal m_gapMaxFactor;

      public:
        /**
         * @brief Constructor
         * @param scene Flow chart scene
         * @param layouter Flow chart layouter
         * @param parent Parent object
         */
        explicit FlowChartConnector(FlowChartScene* scene, FlowChartModel* model,
                                    QObject* parent = nullptr);
        ~FlowChartConnector() override; ///< Destructor

        /**
         * @brief Clear all connection lines
         */
        void clear();

        /**
         * @brief Draw all connection lines
         * @details Traverse all connection lines, call their paint method to draw
         */
        void updateAll();

      private:
        /**
         * @brief Compute the degrees of the connected nodes
         * @details Compute the degrees of the connected nodes based on the number of edges
         */
        void computeDegrees();

        /**
         * @brief Determine the path type of the connection line
         * @param source Source node
         * @param target Target node
         * @param iEdgeCount Total number of edges with the same source and target
         * @return Path type
         */
        static PathType determinePathType(FlowChartShapeItem* source, FlowChartShapeItem* target,
                                   int iEdgeCount);

        /**
         * @brief Collect all connection lines from the model
         * @details Recursive processing of model connection
         * @param model Flow chart model
         */
        void collectEdges(FlowChartModel* model);

        /**
         * @brief Render all connection lines
         * @details Recursive processing of model connection
         */
        void renderEdges();

        /**
         * @brief Calculate the control points of the connection line
         * @param edgeItem Connection line item
         * @param iEdgeIndex Edge index (index of the same source and target)
         * @param iEdgeCount Total number of edges with the same source and target
         * @param isSelfLoop Whether the edge is a self-loop
         * @return Control points
         */
        [[nodiscard]] QVector<QPointF> calculateControlPoints(const FlowChartEdgeItem* edgeItem,
                                                              int iEdgeIndex, int iEdgeCount,
                                                              bool isSelfLoop) const;

        /**
         * @brief Check if the edge is a self-loop
         * @param source Source node
         * @param target Target node
         * @return Whether the edge is a self-loop
         */
        static bool isEdgeSelfLoop(FlowChartShapeItem* source, FlowChartShapeItem* target);

        /**
         * @brief Getter of the total number of edges with the same source
         * and target
         * @param source Source node
         * @param target Target node
         * @return Total number of edges with the same source and target
         */
        int getEdgeIndex(FlowChartShapeItem* source, FlowChartShapeItem* target);

        /**
         * @brief Getter of the total number of edges with the same source
         * and target
         * @param source Source node
         * @param target Target node
         * @return Total number of edges with the same source and target
         */
        int getEdgeCount(FlowChartShapeItem* source, FlowChartShapeItem* target) const;
    };

} // namespace hbchart

#endif // FLOWCHARTCONNECTOR_H
