/**
 * File: GanttChartCoordinate.h
 * Description: The declaration of GanttChartCoordinate class for Gantt chart axis.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTCHARTCOORDINATE_H
#define GANTTCHARTCOORDINATE_H

#include <QGraphicsPathItem>
#include <QGraphicsSimpleTextItem>
#include <QObject>
#include <QTimer>

#include "GanttChartLayouter.h"
#include "GanttChartScene.h"

namespace hbchart {

    enum class CoordLabelPosition: std::uint8_t {
        RIGHT,
        LEFT,
        BOTTOM_RIGHT,
        BOTTOM_LEFT,
        BOTTOM_CENTER
    };

    /**
     * @brief The base class for Gantt chart axis.
     * @details This class is responsible for drawing the coordinate axis and the ticks.
     */
    class GanttChartCoordPath : public QObject, public QGraphicsPathItem {
        Q_OBJECT

      protected:
        QRectF m_boundingRect;              ///< Bounding rect of the coordinate line
        qreal m_lineWidth;                  ///< Label width
        QColor m_lineColor;                 ///< Color of the coordinate line
        Qt::PenStyle m_lineStyle;           ///< Line style of the coordinate line
        QFont m_labelFont;                  ///< Font of the label
        QString m_label;                    ///< Label format string
        qreal m_labelMargin;                ///< Margin of the label
        qreal m_labelOffset;                ///< Offset of the label
        bool m_showLabel;                   ///< Show label or not
        CoordLabelPosition m_labelPosition; ///< Label position
        bool m_axisArrowVisible;            ///< Show axis arrow or not
        QVector<QPolygonF> m_arrows;        ///< Arrows polygons
        QPointF m_start;                    ///< Start point of the connection line
        QPointF m_end;                      ///< End point of the connection line
        qreal m_extendSize;                 ///< Extend the coordinate line to the left and right
        QRectF m_labelRect;                 ///< Rect of the label

      Q_SIGNALS:
        void eventPathPropertyChanged();

      public:
        /**
         * @brief Constructor
         * @param label The label format string
         * @param parent The parent item
         */
        GanttChartCoordPath(QString label = "", QGraphicsItem* parent = nullptr);
        ~GanttChartCoordPath() override = default; ///< Destructor

        /**
         * @brief Set the line width of the coordinate line
         * @details The line width of the coordinate line is used to draw the ticks and the label.
         * @param width The line width in pixels
         */
        void setLineWidth(qreal width);

        /**
         * @brief Get the line width of the coordinate line
         * @return The line width in pixels
         * @see setLineWidth
         */
        [[nodiscard]] qreal getLineWidth() const;

        /**
         * @brief Set the color of the coordinate line
         * @param color The color of the coordinate line
         */
        void setLineColor(const QColor& color);

        /**
         * @brief Get the color of the coordinate line
         * @return The color of the coordinate line
         */
        [[nodiscard]] QColor getLineColor() const;

        /**
         * @brief Set the line style of the coordinate line
         * @param style The line style of the coordinate line
         */
        void setLineStyle(Qt::PenStyle style);

        /**
         * @brief Get the line style of the coordinate line
         * @return The line style of the coordinate line
         */
        [[nodiscard]] Qt::PenStyle getLineStyle() const;

        /**
         * @brief Set the font of the label
         * @param font The font of the label
         */
        void setLabelFont(const QFont& font);

        /**
         * @brief Get the font of the label
         * @return The font of the label
         */
        [[nodiscard]] QFont getLabelFont() const;

        /**
         * @brief Set the label format string
         * @details The label format string is used to format the time value in the coordinate.
         * @param label The label format string
         */
        void setLabel(const QString& label);

        /**
         * @brief Get the label format string
         * @return The label format string
         */
        [[nodiscard]] QString getLabel() const;

        /**
         * @brief Set the margin of the label
         * @details The margin of the label is used to adjust the position of the label relative to
         * the coordinate line.
         * @param margin The margin of the label
         */
        void setLabelMargin(qreal margin);

        /**
         * @brief Get the margin of the label
         * @return The margin of the label
         */
        [[nodiscard]] qreal getLabelMargin() const;

        /**
         * @brief Set the offset of the label
         * @details The offset of the label is used to adjust the position of the label relative to
         * the coordinate line.
         * @param offset The offset of the label
         */
        void setLabelOffset(qreal offset);

        /**
         * @brief Get the offset of the label
         * @return The offset of the label
         */
        [[nodiscard]] qreal getLabelOffset() const;

        /**
         * @brief Set the label position
         * @details The label position is used to position the label relative to the coordinate
         * line.
         * @param position The label position
         */
        void setShowLabel(bool show);

        /**
         * @brief Get the label position
         * @return The label position
         */
        [[nodiscard]] bool isShowLabel() const;

        /**
         * @brief Set the label position
         * @details The label position is used to position the label relative to the coordinate
         * line.
         * @param position The label position
         */
        void setLabelPosition(CoordLabelPosition position);

        /**
         * @brief Get the label position
         * @return The label position
         */
        [[nodiscard]] CoordLabelPosition getLabelPosition() const;

        /**
         * @brief Set the axis arrow visible or not
         * @details The axis arrow is used to indicate the direction of the coordinate line.
         * @param show The visibility of the axis arrow
         */
        void setAxisArrowVisible(bool show);

        /**
         * @brief Get the axis arrow visibility
         * @return The visibility of the axis arrow
         */
        [[nodiscard]] bool isAxisArrowVisible() const;

        /**
         * @brief Update the coordinate end points
         * @details The coordinate line would be updated if the start and end points is changed
         * @param start The start point of the connection line
         * @param end The end point of the connection line
         */
        void updateCoordEndPoints(QPointF start, QPointF end);

        /**
         * @brief Get the coordinate end points
         * @return The start and end points of the connection line
         */
        [[nodiscard]] QPair<QPointF, QPointF> getCoordEndPoints() const;

        /**
         * @brief Set the extend of the coordinate line to the left and right
         * @details The coordinate line would be updated if the extend is changed
         * @param extend The extend in pixels
         */
        void setExtendSize(qreal extend);

        /**
         * @brief Get the extend of the coordinate line to the left and right
         * @return The extend in pixels
         */
        [[nodiscard]] qreal getExtendSize() const;

        /**
         * @brief Get the bounding rect of the coordinate line
         * @return The bounding rect of the coordinate line
         */
        [[nodiscard]] QRectF boundingRect() const override;

      protected:
        /**
         * @brief update the connection line path
         * @details Recalculate the connection line path based on the source and target nodes and
         * anchor points
         */
        virtual void updatePath() = 0;

        /**
         * @brief Update the arrow heads
         * @details Caculate the arrow head path and add it to the path
         */
        void updateArrowHeads();

        /**
         * @brief Update the rect of the label
         */
        void updateLabelRect();

        /**
         * @brief Calculate the arrow head path
         * @param point The vertex of the arrow
         * @param angleDeg The angle of the arrow
         * @return The arrow head path
         */
        static QPolygonF calculateArrowHead(const QPointF& point, qreal angleDeg);

        /**
         * @brief Update the bounding rect of the coordinate line
         */
        virtual void updateBoundingRect();
    };

    /**
     * @brief The class for Gantt chart axis.
     * @details This class is responsible for drawing the coordinate axis and the ticks.
     */
    class GanttChartCoordX : public GanttChartCoordPath {

      public:
        /**
         * @brief Constructor
         * @param label The label format string
         * @param parent The parent item
         */
        GanttChartCoordX(const QString& label = "", QGraphicsItem* parent = nullptr);
        ~GanttChartCoordX() override = default; ///< Destructor

      protected:
        /**
         * @brief update the connection line path
         * @details Recalculate the connection line path based on the source and target nodes and
         * anchor points
         */
        void updatePath() override;

        /**
         * @brief paint event
         * @param painter Painter
         * @param option Graphics option
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;
    };

    /**
     * @brief The class for Gantt chart x-axis grid line.
     * @details This class is responsible for drawing the x-axis grid line.
     */
    class GanttChartXAxisGridLine : public GanttChartCoordPath {
      private:
        QTimer* m_timer;                  ///< Timer for updating the coordinate line
        bool m_isExtendGuideLine = false; ///< Extend guide line or not

      public:
        /**
         * @brief Constructor
         * @param label The label format string
         * @param parent The parent item
         */
        GanttChartXAxisGridLine(const QString& label = "", QGraphicsItem* parent = nullptr);
        ~GanttChartXAxisGridLine() override = default; ///< Destructor

        /**
         * @brief Set the extend guide line or not
         * @param isExtendGuideLine The extend guide line or not
         */
        void setExtendGuideLine(bool isExtendGuideLine);

        /**
         * @brief Get the extend guide line or not
         * @return The extend guide line or not
         */
        [[nodiscard]] bool isExtendGuideLine() const;

      protected:
        /**
         * @brief update the connection line path
         * @details Recalculate the connection line path based on the source and target nodes and
         * anchor points
         */
        void updatePath() override;

        /**
         * @brief paint event
         * @param painter Painter
         * @param option Graphics option
         * @param widget Widget
         */
        void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
                   QWidget* widget) override;

        /**
         * @brief Update the bounding rect of the coordinate line
         */
        void updateBoundingRect() override;
    };

    /**
     * @brief The class for Gantt chart coordinate.
     * @details This class is responsible for drawing the coordinate axis and the ticks.
     */
    class GanttChartCoordinate : public QObject {
        Q_OBJECT

      private:
        GanttChartScene* m_scene;                             ///< Scene of the chart
        GanttChartLayouter* m_layouter;                       ///< Layouter of the chart
        GanttChartCoordX* m_coordX;                           ///< X-axis
        QString m_coordXLabel;                                ///< Label of the x-axis
        QList<GanttChartXAxisGridLine*> m_primaryGridLines;   ///< Primary grid lines
        bool m_showXAxisPrimaryGridLine;                      ///< Show primary grid line or not
        QList<GanttChartXAxisGridLine*> m_secondaryGridLines; ///< Secondary grid lines
        bool m_showXAxisSecondaryGridLine;                    ///< Show secondary grid line or not
        int m_primaryGridLineCount;                           ///< Number of primary grid lines
        int m_secondaryGridLineCount;                         ///< Number of secondary grid lines

      Q_SIGNALS:
        void eventCoordinatePropertyChanged();

      public:
        /**
         * @brief Constructor
         * @param layouter The layouter of the chart
         * @param scene The scene of the chart
         * @param parent The parent item
         */
        GanttChartCoordinate(GanttChartLayouter* layouter, GanttChartScene* scene,
                             QObject* parent = nullptr);
        ~GanttChartCoordinate() override; ///< Destructor

        /**
         * @brief Reset the coordinate
         */
        void resetCoordinate();

        /**
         * @brief Set the label of the x-axis
         * @param label The label of the x-axis
         */
        void setCoordXLabel(const QString& label);

        /**
         * @brief Get the label of the x-axis
         * @return The label of the x-axis
         */
        [[nodiscard]] QString getCoordXLabel() const;

        /**
         * @brief Set the visibility of the x-axis primary grid line
         * @param show The visibility of the x-axis primary grid line
         */
        void setShowXAxisPrimaryGridLine(bool show);

        /**
         * @brief Get the visibility of the x-axis primary grid line
         * @return The visibility of the x-axis primary grid line
         */
        [[nodiscard]] bool isShowXAxisPrimaryGridLine() const;

        /**
         * @brief Set the visibility of the x-axis secondary grid line
         * @param show The visibility of the x-axis secondary grid line
         */
        void setShowXAxisSecondaryGridLine(bool show);

        /**
         * @brief Get the visibility of the x-axis secondary grid line
         * @return The visibility of the x-axis secondary grid line
         */
        [[nodiscard]] bool isShowXAxisSecondaryGridLine() const;

        /**
         * @brief Set the number of primary grid lines
         * @param count The number of primary grid lines
         */
        void setPrimaryGridLineCount(int count);

        /**
         * @brief Get the number of primary grid lines
         * @return The number of primary grid lines
         */
        [[nodiscard]] int getPrimaryGridLineCount() const;

        /**
         * @brief Set the number of secondary grid lines
         * @param count The number of secondary grid lines
         */
        void setSecondaryGridLineCount(int count);

        /**
         * @brief Get the number of secondary grid lines
         * @return The number of secondary grid lines
         */
        [[nodiscard]] int getSecondaryGridLineCount() const;

        /**
         * @brief Update the coordinate
         */
        void updateAll();

      private:
        void initialize();

        /**
         * @brief Calculate the x-axis primary grid line
         * @details Calculate the x-axis primary grid line based on the minimum and maximum time of
         * the chart and the number of primary grid lines.
         * @param start The start point of the connection line
         * @param end The end point of the connection line
         * @param span The span of from model end to chart end
         */
        void calculateXAxisPrimaryGridLine(QPointF start, QPointF end, qreal span);

        /**
         * @brief Calculate the x-axis secondary grid line
         * @details Calculate the x-axis secondary grid line based on the minimum and maximum time
         * of the chart and the number of secondary grid lines.
         * @param start The start point of the connection line
         * @param end The end point of the connection line
         * @param span The span of from model end to chart end
         */
        void calculateXAxisSecondaryGridLine(QPointF start, QPointF end, qreal span);
    };

} // namespace hbchart

#endif // GANTTCHARTCOORDINATE_H
