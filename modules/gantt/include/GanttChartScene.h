/**
 * File: GanttChartScene.h
 * Description: The declaration of the GanttChartScene class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTCHARTSCENE_H
#define GANTTCHARTSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QPair>

#include "GanttChartModel.h"
#include "GanttChartShapeItem.h"

namespace hbchart {

    class GanttChartCoordinate;
    class GanttChartGuideLineManager;

    constexpr int DEFAULT_GRID_SIZE = 20;       ///< Default grid size
    constexpr int DEFAULT_ROW_HEIGHT = 28;      ///< Default row height
    constexpr int DEFAULT_PIXELS_PER_UNIT = 10; ///< Default unit width
    constexpr int DEFAULT_ROW_MARGIN = 5;       ///< Default margin
    constexpr qreal CHART_MAX_WIDTH = 600.0;    ///< Minimum chart width
    constexpr qreal CHART_MIN_HEIGHT = 400.0;   ///< Minimum chart height
    constexpr qreal CHART_MARGIN = 30.0;        ///< Scene margin
    constexpr qreal CHART_SAFETY_MARGIN = 50.0; ///< Safety margin
    constexpr qreal CHART_X_AXIS_HEIGHT = 20.0; ///< X-axis height
    constexpr qreal CHART_Y_AXIS_WIDTH = 50.0;  ///< Y-axis width

    /**
     * @brief View time unit
     */
    enum class ViewTimeUnit: std::uint8_t {
        NUMBER,      ///< Pure numbers (such as 1.5)
        SECOND,      ///< Seconds (120s)
        MINUTE,      ///< Minutes (2m)
        HOUR,        ///< Hour number (1.5h), non-formatted time
        TIME_HHMM,   ///< hh:mm (for example, 14:25)
        TIME_HHMMSS, ///< hh:mm:ss (such as 14:25:12)
        TIME_AMPM,   ///< 12-hour time (2:25 PM)
        DATE,        ///< Date (2025-01-01)
        WEEKDAY,     ///< Weekday (Monday)
        ISOWEEK,     ///< ISO Week (2025-W01)
    };

    /**
     * @brief TimeSceneRatio
     * @details Time scene ratio is used to convert time to pixels, and pixels to time
     *In the scene, the time unit is seconds, the pixel unit is pixel, 1 second = 1 pixel
     *When 100% zoom, 1 pixel = 1 pixel When 50% zoom, 1 pixel = 2 pixels When 200% zoom, 1 pixel =
     *5 pixels
     */
    struct TimeSceneRatio {
        qreal scene_per_time;   ///< Unit time per Scene unit
        qreal view_scale = 1.0; // View is currently zoomed, 1.0=100%

        /**
         * @brief Convert time to scene units
         * @param time Time in seconds
         * @return Time in scene units
         */
        [[nodiscard]] qreal toScene(qreal time) const { return time * scene_per_time; }

        /**
         * @brief Convert scene units to time
         * @param scene Scene units
         * @return Time in seconds
         */
        [[nodiscard]] qreal fromScene(qreal scene) const { return scene / scene_per_time; }

        /**
         * @brief Convert Scene units to pixels
         * @param SceneUnits Scene units
         * @return Pixels
         */
        [[nodiscard]] qreal toPixels(qreal sceneUnits) const { return sceneUnits * view_scale; }

        /**
         * @brief Convert pixels to scene units
         * @param pixels Pixels
         * @return Scene units
         */
        [[nodiscard]] qreal fromPixels(qreal pixels) const { return pixels / view_scale; }

        /**
         * @brief Convert pixels to time
         * @param pixels Pixels
         * @return Time in seconds
         */
        [[nodiscard]] qreal pixelsToTime(qreal pixels) const {
            return fromScene(fromPixels(pixels));
        }

        /**
         * @brief Convert time to pixels
         * @param time Time in seconds
         * @return Pixels
         */
        [[nodiscard]] qreal timeToPixels(qreal time) const { return toPixels(toScene(time)); }
    };

    /**
     * @brief GanttChart scene
     * @details Inherits from QGraphicsScene, used for drawing GanttCharts
     */
    class GanttChartScene : public QGraphicsScene {
        Q_OBJECT

      private:
        QRectF m_visibleRect;                                                ///< Visible area
        TimeSceneRatio m_timeSceneRatio;                                     ///< Time scene ratio
        ViewTimeUnit m_viewTimeUnit;                                         ///< View time unit axis or tooltip
        qreal m_gridSize;                                                    ///< Grid size
        qreal m_rowHeight;                                                   ///< Row height
        qreal m_rowMargin;                                                   ///< Margin
        qreal m_minChartWidth;                                               ///< Minimum chart width
        qreal m_minChartHeight;                                              ///< Minimum chart height
        qreal m_chartMargin;                                                 ///< Chart margin
        qreal m_saftyMargin;                                                 ///< Safety margin
        qreal m_xAxisHeight;                                                 ///< X-axis height
        qreal m_yAxisWidth;                                                  ///< Y-axis width
        GanttChartModel* m_model = nullptr;                                  ///< Model
        QHash<const GanttSection*, GanttChartShapeItem*> m_sectionItems;     ///< Section items
        QMap<QString, QColor> m_sectionColorMap;                             ///< Section color map
        QHash<const GanttTask*, GanttChartShapeItem*> m_taskItems;           ///< Task items
        QHash<const GanttMilestone*, GanttChartShapeItem*> m_milestoneItems; ///< Milestone items
        qreal m_sceneMinTime;                                                ///< Minimum time in scene units
        qreal m_sceneMaxTime;                                                ///< Maximum time in scene units
        GanttChartLabel* m_titleItem;                                        ///< Title item

        // Debugging and using
        bool m_DrawChartBoundary = false;                                    ///< Whether to draw chart boundary
        QRectF m_chartInnerRect;                                             ///< Chart inner rect
        QRectF m_chartOuterRect;                                             ///< Chart outer rect

        QMap<ViewTimeUnit, QString> m_defualtViewUnitFormat;                 ///< Default view unit format
        QMap<ViewTimeUnit, QString> m_customViewUnitFormat;                  ///< Custom view unit format

        QList<QString> m_defaultWeekdayName;                                 ///< Default weekday name
        QList<QString> m_customWeekdayName;                                  ///< Custom weekday name

      Q_SIGNALS:
        void eventSceneDataChanged();

      public:
        /**
         * @brief Constructor
         * @param parent Parent object
         */
        explicit GanttChartScene(GanttChartModel* model, QObject* parent = nullptr);
        ~GanttChartScene() override = default; ///< Destructor

        /**
         * @brief Get the grid size.
         * @return The grid size.
         */
        [[nodiscard]] qreal getGridSize() const;

        /**
         * @brief Get the row height.
         * @return The row height.
         */
        [[nodiscard]] qreal rowHeight() const;

        /**
         * @brief Get the margin.
         * @return The margin.
         */
        [[nodiscard]] qreal rowMargin() const;

        /**
         * @brief Set scale factor
         * @param scaleFactor Scale factor
         */
        void updateViewScaleFactor(qreal scaleFactor);

        /**
         * @brief Get core time unit type
         * @return Core time unit type
         */
        [[nodiscard]] const TimeUnitType& getCoreTimeUnitType() const;

        /**
         * @brief Get time scale
         * @return Time scale object
         */
        [[nodiscard]] const TimeSceneRatio& timeSceneRatio() const;

        /**
         * @brief Set the view time unit
         * @param unit The view time unit
         */
        void setViewUnit(ViewTimeUnit unit);

        /**
         * @brief Get the view time unit
         * @return The view time unit
         */
        [[nodiscard]] ViewTimeUnit getViewUnit() const;

        /**
         * @brief Set the view time unit format
         * @param unit The view time unit
         * @param format The view time unit format
         */
        void setViewUnitFormat(ViewTimeUnit unit, const QString& format);

        /**
         * @brief Get the view time unit format
         * @param unit The view time unit
         * @return The view time unit format
         */
        [[nodiscard]] QString getViewUnitFormat(ViewTimeUnit unit) const;

        /**
         * @brief Set the weekday name list
         * @param weekdayName The weekday name list
         */
        void setWeekdayName(const QList<QString>& weekdayName);

        /**
         * @brief Set the weekday name at index
         * @param index The index of the weekday name to set
         * @param weekdayName The weekday name to set
         */
        void setWeekdayName(int index, const QString& weekdayName);

        /**
         * @brief Get the weekday name list
         * @return The weekday name list
         */
        [[nodiscard]] QList<QString> getWeekdayName() const;

        /**
         * @brief Get the weekday name at index
         * @param index The index of the weekday name to get
         * @return The weekday name at index
         */
        [[nodiscard]] QString getWeekdayName(int index) const;

        /**
         * @brief Set minimum chart width
         * @param width Minimum chart width
         * @note unit pixel
         */
        void setChartMinWidth(qreal width);

        /**
         * @brief Get minimum chart width
         * @return Minimum chart width
         */
        [[nodiscard]] qreal chartMinWidth() const;

        /**
         * @brief Set minimum chart height
         * @param height Minimum chart
         * @note unit pixel
         */
        void setChartMinHeight(qreal height);

        /**
         * @brief Get minimum chart height
         * @return Minimum chart height
         */
        [[nodiscard]] qreal chartMinHeight() const;

        /**
         * @brief Set chart margin
         * @param margin Chart margin
         * @note unit pixel
         */
        void setChartMargin(qreal margin);

        /**
         * @brief Get chart margin
         * @return Chart margin
         */
        [[nodiscard]] qreal chartMargin() const;

        /**
         * @brief Set chart safety margin (margin outside of outer rect)
         * @param margin Chart safety margin
         * @note unit pixel
         */
        void setSaftyMargin(qreal margin);

        /**
         * @brief Get chart safety margin (margin outside of outer rect)
         * @return Chart safety margin
         */
        [[nodiscard]] qreal getSaftyMargin() const;

        /**
         * @brief Set x-axis height
         * @param height X-axis height
         * @note unit pixel
         */
        void setXAxisHeight(qreal height);

        /**
         * @brief Get x-axis height
         * @return X-axis height
         */
        [[nodiscard]] qreal xAxisHeight() const;

        /**
         * @brief Set y-axis width
         * @param width Y-axis width
         * @note unit pixel
         */
        void setYAxisWidth(qreal width);

        /**
         * @brief Get y-axis width
         * @return Y-axis width
         */
        [[nodiscard]] qreal yAxisWidth() const;

        /**
         * @brief Get the title item
         * @return The title item
         */
        [[nodiscard]] GanttChartLabel* getTitleItem() const;

        /**
         * @brief find section item by section
         * @param section The section to find
         * @return The section item if found, otherwise nullptr
         */
        GanttChartShapeItem* findSectionItem(const GanttSection* section) const;

        /**
         * @brief find task item by task
         * @param task The task to find
         * @return The task item if found, otherwise nullptr
         */
        GanttChartShapeItem* findTaskItem(const GanttTask* task) const;

        /**
         * @brief find milestone item by milestone
         * @param milestone The milestone to find
         * @return The milestone item if found, otherwise nullptr
         */
        GanttChartShapeItem* findMilestoneItem(const GanttMilestone* milestone) const;

        /**
         * @brief Set whether to draw chart boundary
         * @param draw Whether to draw chart boundary
         * @note debug use only
         */
        void setDrawChartBoundary(bool draw);

        /**
         * @brief Get whether to draw chart boundary
         * @return Whether to draw chart boundary
         * @note debug use only
         */
        [[nodiscard]] QRectF chartInnerRect() const;

        /**
         * @brief Set chart inner rect
         * @param rect Chart inner
         * @note debug use only
         */
        void setChartInnerRect(const QRectF& rect);

        /**
         * @brief Get chart outer rect
         * @return Chart outer rect
         * @note debug use only
         */
        [[nodiscard]] QRectF chartOuterRect() const;

        /**
         * @brief Set chart outer rect
         * @param rect Chart outer rect
         * @note debug use only
         */
        void setChartOuterRect(const QRectF& rect);

        /**
         * @brief Get the time range in scene units
         * @return The time range in scene units
         */
        QPair<qreal, qreal> getSceneTimeRange();

        /**
         * @brief Get the max duration in scene units
         * @return The max duration in scene units
         */
        qreal getSceneMaxDuration();

        /**
         * @brief Get the base time in scene units
         * @return The base time in scene units
         */
        [[nodiscard]] qreal getSceneBaseTime() const;

        /**
         * @brief Format the label
         * @param timeSec The time in seconds to format
         * @return The formatted label
         */
        [[nodiscard]] QString formatAxisLabel(qreal timeSec) const;

        /**
         * @brief Update the scene
         * @details This method is called by the model when the model data changes. It updates the scene
         * according to the changes.
         */
        void updateScene();

      protected:
        /**
         * @brief Set the model
         * @param model The model to set
         */
        void updateSceneItems(EventGanttUpdateType type);

        /**
         * @brief Update the title in the scene
         */
        void updateTitle();

        /**
         * @brief Update the sections in the scene
         */
        void updateSections();

        /**
         * @brief Update the tasks in the scene
         */
        void updateTasks();

        /**
         * @brief Update the milestones in the scene
         */
        void updateMilestones();

        /**
         * @brief Draw background
         * @details Draw grid lines
         * @param painter Painter object
         * @param rect Drawing area
         * @note Overrides parent class method to draw grid lines
         */
        void drawBackground(QPainter* painter, const QRectF& rect) override;

      private:
        /**
         * @brief Add a title to the scene
         * @param title The title to add
         * @note This method is called by updateScene() to add a title to the scene
         */
        void addTitle(const GanttTitle& title);

        /**
         * @brief Add a section to the scene
         * @param section The section to add
         * @note This method is called by updateScene() to add a section to the scene
         */
        void addSection(const GanttSection* section);

        /**
         * @brief Add a task to the scene
         * @param task The task to add
         * @note This method is called by updateScene() to add a task to the scene
         */
        void addTask(const GanttTask* task);

        /**
         * @brief Add a milestone to the scene
         * @param milestone The milestone to add
         * @note This method is called by updateScene() to add a milestone to the scene
         */
        void addMilestone(const GanttMilestone* milestone);

        /**
         * @brief connect signals to an item
         * @param item The item to connect signals to
         */
        template <typename T> void connectSignals(GanttChartShapeItem* item, const T* obj);

        /**
         * @brief Disconnect signals from an
         * @param item The item to disconnect signals from
         */
        void disconnectSignals(GanttChartShapeItem* item);

        /**
         * @brief Update the time range in scene units
         */
        void updateSceneTimeRange();

        /**
         * @brief Update the scene_per_time ratio
         */
        void updateTimeSceneRatio();
    };
} // namespace hbchart

#endif // GANTTCHARTSCENE_H
