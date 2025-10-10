/**
 * File: GanttChartScene.cpp
 * Description: The implementation of the GanttChartScene class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/13
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#include "GanttChartScene.h"

#include <QDebug>
#include <QGraphicsView>
#include <QList>
#include <QPainter>
#include <QtMath>

#include "GanttChartCoordinate.h"
#include "GanttChartShapeItem.h"

namespace hbchart {
    GanttChartScene::GanttChartScene(GanttChartModel* model, QObject* parent)
        : m_model(model), QGraphicsScene(parent) {
        m_timeSceneRatio.scene_per_time = DEFAULT_PIXELS_PER_UNIT;
        m_gridSize = DEFAULT_GRID_SIZE;
        m_rowHeight = DEFAULT_ROW_HEIGHT; ///< Row height
        m_rowMargin = DEFAULT_ROW_MARGIN; ///< Margin
        m_minChartWidth = CHART_MAX_WIDTH;
        m_minChartHeight = CHART_MIN_HEIGHT;
        m_chartMargin = CHART_MARGIN;
        m_saftyMargin = CHART_SAFETY_MARGIN;
        m_xAxisHeight = CHART_X_AXIS_HEIGHT;
        m_yAxisWidth = CHART_Y_AXIS_WIDTH;
        m_sceneMinTime = 0.0; // seconds / unit, i.e. unit time
        m_sceneMaxTime = 0.0; // seconds / unit, i.e. unit time
        m_titleItem = nullptr;

        // Set default view unit format
        m_viewTimeUnit = ViewTimeUnit::HOUR;

        m_defualtViewUnitFormat.insert(ViewTimeUnit::NUMBER, "%1");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::SECOND, "%1s");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::MINUTE, "%1m");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::HOUR, "%1h");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::TIME_HHMM, "%1:%2");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::TIME_HHMMSS, "%1:%2:%3");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::TIME_AMPM, "%1:%2 %3");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::DATE, "yyyy-MM-dd");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::WEEKDAY, "%1");
        m_defualtViewUnitFormat.insert(ViewTimeUnit::ISOWEEK, "Week %1");

        int weekdayCount = 0;
        m_defaultWeekdayName.insert(weekdayCount, "Mon");
        m_defaultWeekdayName.insert(++weekdayCount, "Tue");
        m_defaultWeekdayName.insert(++weekdayCount, "Wed");
        m_defaultWeekdayName.insert(++weekdayCount, "Thu");
        m_defaultWeekdayName.insert(++weekdayCount, "Fri");
        m_defaultWeekdayName.insert(++weekdayCount, "Sat");
        m_defaultWeekdayName.insert(++weekdayCount, "Sun");
        m_customWeekdayName = {"", "", "", "", "", "", ""};
    }

    void GanttChartScene::updateViewScaleFactor(qreal scaleFactor) {
        m_timeSceneRatio.view_scale = scaleFactor;
    }

    qreal GanttChartScene::getGridSize() const { return m_gridSize; }

    qreal GanttChartScene::rowHeight() const { return m_rowHeight; }

    qreal GanttChartScene::rowMargin() const { return m_rowMargin; }

    const TimeUnitType& GanttChartScene::getCoreTimeUnitType() const {
        return m_model->getTimeUnitType();
    }

    const TimeSceneRatio& GanttChartScene::timeSceneRatio() const { return m_timeSceneRatio; }

    void GanttChartScene::setViewUnit(ViewTimeUnit unit) {
        if (m_viewTimeUnit != unit) {
            m_viewTimeUnit = unit;
        }
    }

    ViewTimeUnit GanttChartScene::getViewUnit() const { return m_viewTimeUnit; }

    void GanttChartScene::setViewUnitFormat(ViewTimeUnit unit, const QString& format) {
        if (m_customViewUnitFormat.contains(unit)) {
            m_customViewUnitFormat[unit] = format;
        } else {
            m_customViewUnitFormat.insert(unit, format);
        }
    }

    QString GanttChartScene::getViewUnitFormat(ViewTimeUnit unit) const {
        QString format = m_customViewUnitFormat.contains(unit) ? m_customViewUnitFormat[unit]
                                                               : m_defualtViewUnitFormat[unit];
        return format;
    }

    void GanttChartScene::setWeekdayName(const QList<QString>& weekdayName) {
        for (int i = 0; i < weekdayName.size(); i++) {
            if (!weekdayName[i].isEmpty()) {
                m_customWeekdayName[i] = weekdayName[i];
            }
        }
    }

    void GanttChartScene::setWeekdayName(int index, const QString& weekdayName) {
        constexpr int weekdayCount = 7;
        if (index >= 0 && index < weekdayCount) {
            if (!weekdayName.isEmpty()) {
                m_customWeekdayName[index] = weekdayName;
            }
        }
    }

    QList<QString> GanttChartScene::getWeekdayName() const {
        constexpr int weekdayCount = 7;
        QList<QString> weekdayName;
        for (int i = 0; i < weekdayCount; i++) {
            QString weeekday =
                m_customWeekdayName[i].isEmpty() ? m_defaultWeekdayName[i] : m_customWeekdayName[i];
            weekdayName.append(weeekday);
        }
        return weekdayName;
    }

    QString GanttChartScene::getWeekdayName(int index) const {
        constexpr int weekdayCount = 7;
        if (index >= 0 && index < weekdayCount) {
            return m_customWeekdayName[index].isEmpty() ? m_defaultWeekdayName[index]
                                                        : m_customWeekdayName[index];
        }
        return "";
    }

    void GanttChartScene::setChartMinWidth(qreal width) {
        m_minChartWidth = width;
        updateTimeSceneRatio();
    }

    qreal GanttChartScene::chartMinWidth() const { return m_minChartWidth; }

    void GanttChartScene::setChartMinHeight(qreal height) { m_minChartHeight = height; }

    qreal GanttChartScene::chartMinHeight() const { return m_minChartHeight; }

    void GanttChartScene::setChartMargin(qreal margin) { m_chartMargin = margin; }

    qreal GanttChartScene::chartMargin() const { return m_chartMargin; }

    void GanttChartScene::setSaftyMargin(qreal margin) { m_saftyMargin = margin; }

    qreal GanttChartScene::getSaftyMargin() const { return m_saftyMargin; }

    void GanttChartScene::setXAxisHeight(qreal height) { m_xAxisHeight = height; }

    qreal GanttChartScene::xAxisHeight() const { return m_xAxisHeight; }

    void GanttChartScene::setYAxisWidth(qreal width) { m_yAxisWidth = width; }

    qreal GanttChartScene::yAxisWidth() const { return m_yAxisWidth; }

    GanttChartLabel* GanttChartScene::getTitleItem() const { return m_titleItem; }

    GanttChartShapeItem* GanttChartScene::findSectionItem(const GanttSection* section) const {
        return m_sectionItems.value(section, nullptr);
    }

    GanttChartShapeItem* GanttChartScene::findTaskItem(const GanttTask* task) const {
        return m_taskItems.value(task, nullptr);
    }

    GanttChartShapeItem* GanttChartScene::findMilestoneItem(const GanttMilestone* milestone) const {
        return m_milestoneItems.value(milestone, nullptr);
    }

    void GanttChartScene::setDrawChartBoundary(bool draw) { m_DrawChartBoundary = draw; }

    QRectF GanttChartScene::chartInnerRect() const { return m_chartInnerRect; }

    void GanttChartScene::setChartInnerRect(const QRectF& rect) { m_chartInnerRect = rect; }

    QRectF GanttChartScene::chartOuterRect() const { return m_chartOuterRect; }

    void GanttChartScene::setChartOuterRect(const QRectF& rect) { m_chartOuterRect = rect; }

    QPair<qreal, qreal> GanttChartScene::getSceneTimeRange() {
        updateSceneTimeRange();
        return {m_sceneMinTime, m_sceneMaxTime};
    }

    qreal GanttChartScene::getSceneMaxDuration() {
        qreal maxDuration = m_model->getLongestTaskDuration();
        TimeUnitType unitType = m_model->getTimeUnitType();
        return unitType.fromSeconds(maxDuration);
    }

    qreal GanttChartScene::getSceneBaseTime() const {
        qreal baseTime = m_model->getBaseTime();
        TimeUnitType unitType = m_model->getTimeUnitType();
        return unitType.fromSeconds(baseTime);
    }

    QString GanttChartScene::formatAxisLabel(qreal timeSec) const {
        QString format;
        QString dayName;
        qreal unitTime;
        qint64 timeSecInt;
        qint64 restTime;
        qint64 week;
        qint64 day;
        qint64 hour;
        qint64 minute;
        qint64 second;
        QDateTime dateTime;
        constexpr int VALUE_BASE = 10;
        constexpr int HOURS_PER_HALFDAY = 12;
        constexpr int SECONDS_PER_DAY = 86400;
        constexpr int SECONDS_PER_HOUR = 3600;
        constexpr int SECONDS_PER_MINUTE = 60;

        switch (m_viewTimeUnit) {
        case ViewTimeUnit::NUMBER:
        case ViewTimeUnit::SECOND:
            // unitTime = timeSec;
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(timeSec);
        case ViewTimeUnit::MINUTE: // 1 digits
            unitTime = timeSec / SECONDS_PER_MINUTE;
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(unitTime, 0, 'f', 3);
        case ViewTimeUnit::HOUR:
            unitTime = timeSec / SECONDS_PER_HOUR;
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(unitTime, 0, 'f', 4);
        case ViewTimeUnit::TIME_HHMM:
            timeSecInt = qCeil(timeSec);
            restTime = timeSecInt % SECONDS_PER_DAY; // seconds in a day
            hour = restTime / SECONDS_PER_HOUR;
            minute = (restTime % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(hour, 2, VALUE_BASE, QChar('0'))
                .arg(minute, 2, VALUE_BASE, QChar('0'));
        case ViewTimeUnit::TIME_HHMMSS:
            timeSecInt = qCeil(timeSec);
            restTime = timeSecInt % SECONDS_PER_DAY; // seconds in a day
            hour = restTime / SECONDS_PER_HOUR;
            minute = (restTime % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
            second = restTime % SECONDS_PER_MINUTE;
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(hour, 2, VALUE_BASE, QChar('0'))
                .arg(minute, 2, VALUE_BASE, QChar('0'))
                .arg(second, 2, VALUE_BASE, QChar('0'));
        case ViewTimeUnit::TIME_AMPM:
            timeSecInt = qCeil(timeSec);
            restTime = timeSecInt % SECONDS_PER_DAY; // seconds in a day
            hour = restTime / SECONDS_PER_HOUR;
            minute = (restTime % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
            format = getViewUnitFormat(m_viewTimeUnit);
            if (hour >= HOURS_PER_HALFDAY) {
                hour -= HOURS_PER_HALFDAY;
                return format.arg(hour, 2, VALUE_BASE, QChar('0'))
                    .arg(minute, 2, VALUE_BASE, QChar('0'))
                    .arg("PM");
            }
            return format.arg(hour, 2, VALUE_BASE, QChar('0'))
                .arg(minute, 2, VALUE_BASE, QChar('0'))
                .arg("AM");
        case ViewTimeUnit::DATE:
            timeSecInt = qCeil(timeSec);
            dateTime = QDateTime::fromSecsSinceEpoch(timeSecInt);
            format = getViewUnitFormat(m_viewTimeUnit);
            return dateTime.toString(format);
        case ViewTimeUnit::WEEKDAY:
            timeSecInt = qCeil(timeSec);
            dateTime = QDateTime::fromSecsSinceEpoch(timeSecInt);
            // Get dateTime What is the week
            day = dateTime.date().dayOfWeek();
            dayName = getWeekdayName((int)day);
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(dayName);
        case ViewTimeUnit::ISOWEEK:
            timeSecInt = qCeil(timeSec);
            dateTime = QDateTime::fromSecsSinceEpoch(timeSecInt);
            week = dateTime.date().weekNumber();
            format = getViewUnitFormat(m_viewTimeUnit);
            return format.arg(week);
        default:
            return QString::number(timeSec, 'f', 2);
        }
    }

    void GanttChartScene::updateScene() {
        updateSceneItems(EventGanttUpdateType::ALL);
    }

    void GanttChartScene::updateSceneItems(EventGanttUpdateType type) {
        // update time range
        updateSceneTimeRange();
        // update scene time ratioratio
        updateTimeSceneRatio();
        //
        switch (type) {
        case EventGanttUpdateType::BASE_TIME_CHANGED:
        case EventGanttUpdateType::TIME_UNIT_CHANGED:
            break;
        case EventGanttUpdateType::TITLE_CHANGED:
            updateTitle();
            break;
        case EventGanttUpdateType::SECTION_CHANGED:
            updateSections();
            break;
        case EventGanttUpdateType::TASK_CHANGED:
            updateTasks();
            break;
        case EventGanttUpdateType::MILESTONE_CHANGED:
            updateMilestones();
            break;
        case EventGanttUpdateType::ALL:
            updateTitle();
            updateSections();
            updateTasks();
            updateMilestones();
            break;
        default:
            break;
        };
        emit eventSceneDataChanged();
    }

    void GanttChartScene::updateTitle() {
        delete m_titleItem;
        //
        addTitle(m_model->getTitle());
    }

    void GanttChartScene::updateSections() {
        //
        for (auto* item : m_sectionItems) {
            delete item;
        }
        // Clear all items in the scene
        m_sectionItems.clear();
        // Reset m_sectionColorMap
        m_sectionColorMap.clear();
        // Add a section shape for each section item
        for (const GanttSection* section : m_model->getSections()) {
            addSection(section);
        }
    }

    void GanttChartScene::updateTasks() {
        // disconnect all signals of all task items in the scene
        for (auto* item : m_taskItems) {
            disconnectSignals(item);
            delete item;
        }
        m_taskItems.clear();
        // Add a task shape for each task item
        for (const GanttTask* task : m_model->getRootTasks()) {
            addTask(task);
        }
    }

    void GanttChartScene::updateMilestones() {
        // disconnect all signals of all milestone items in the scene
        for (auto* item : m_milestoneItems) {
            disconnectSignals(item);
            delete item;
        }
        m_milestoneItems.clear();
        // Add a milestone shape for each milestone item
        for (const GanttMilestone* milestone : m_model->getMilestones()) {
            addMilestone(milestone);
        }
    }

    void GanttChartScene::drawBackground(QPainter* painter, const QRectF& rect) {
        //
        const bool isDrawLine = false;                       // Draw grid lines?
        constexpr QColor LINE_COLOR = QColor(220, 220, 220); // Light gray
        constexpr QColor GRID_COLOR = QColor(120, 120, 120); // White
        constexpr qreal POINT_SIZE = 0.5;                    // Size of grid points
        QPen pen;
        if (isDrawLine) {
            pen.setColor(LINE_COLOR); // Light gray
        } else {
            pen.setColor(GRID_COLOR); // dark gray
        }
        pen.setWidth(1);
        painter->setPen(pen);

        qreal adjustedGridSize = getGridSize();

        // Calculate area to draw
        qreal left = std::floor(rect.left() / adjustedGridSize) * adjustedGridSize;
        qreal right = std::ceil(rect.right() / adjustedGridSize) * adjustedGridSize;
        qreal top = std::floor(rect.top() / adjustedGridSize) * adjustedGridSize;
        qreal bottom = std::ceil(rect.bottom() / adjustedGridSize) * adjustedGridSize;

        if (isDrawLine) {
            // Draw vertical grid lines
            int iCount = qCeil((right - left) / adjustedGridSize);
            for (int i = 0; i < iCount; i++) {
                qreal xpos = left + (i * adjustedGridSize);
                painter->drawLine(QPointF(xpos, top), QPointF(xpos, bottom));
            }

            int jCount = qCeil((bottom - top) / adjustedGridSize);
            for (int j = 0; j < jCount; j++) {
                qreal ypos = top + (j * adjustedGridSize);
                painter->drawLine(QPointF(left, ypos), QPointF(right, ypos));
            }
        } else {
            int iCount = qCeil((right - left) / adjustedGridSize);
            int jCount = qCeil((bottom - top) / adjustedGridSize);
            for (int i = 0; i < iCount; i++) {
                qreal xpos = left + (i * adjustedGridSize);
                for (int j = 0; j < jCount; j++) {
                    qreal ypos = top + (j * adjustedGridSize);
                    painter->drawPoint(QPointF(xpos, ypos));
                }
            }
        }

        // debug
        if (m_DrawChartBoundary) {
            painter->setPen(QPen(Qt::red, 1));
            painter->drawRect(m_chartInnerRect);
            painter->drawRect(m_chartOuterRect);
            // draw a black circle at  0,0
            painter->setPen(QPen(Qt::black, 1));
            painter->drawEllipse(QPointF(0, 0), 2, 2);
        }
    }

    void GanttChartScene::addTitle(const GanttTitle& title) {
        if (title.text.isEmpty() || !title.visible) {
            return;
        }
        // qDebug() << "GanttChartScene::addTitle" << title.text;
        m_titleItem = new GanttChartLabel(title.text, nullptr);
        addItem(m_titleItem);
    }

    void GanttChartScene::addSection(const GanttSection* section) {
        auto* item = new GanttChartItemSection(section, nullptr);
        QString sectionName = section->name;
        if (m_sectionColorMap.contains(sectionName)) { // Set color for each section
            item->setFillColor(m_sectionColorMap[sectionName]);
        } else { // Generate a random color
            constexpr int MAX_COLOR_VALUE = 255;
            constexpr int ALPHA_VALUE = 50;
            QColor fillcolor = QColor(qrand() % MAX_COLOR_VALUE, qrand() % MAX_COLOR_VALUE,
                                      qrand() % MAX_COLOR_VALUE, ALPHA_VALUE);
            item->setFillColor(fillcolor);
            m_sectionColorMap[sectionName] = fillcolor;
        }
        addItem(item);
        m_sectionItems[section] = item;
    }

    void GanttChartScene::addTask(const GanttTask* task) {
        auto* item = new GanttChartItemTask(
            task, nullptr); // Assume that TaskItem is a subclass of GanttChartShapeItem
        // Calculate the size
        qreal start = task->start;
        qreal end = task->end;
        qreal deltaTimeSec = end - start;
        TimeUnitType unitType = m_model->getTimeUnitType();
        qreal deltaUnitTime = unitType.fromSeconds(deltaTimeSec);
        qreal height = item->height();
        qreal width = timeSceneRatio().toScene(deltaUnitTime);
        item->setRect(RowWidth{width}, RowHeight{height});
        // Join the scene
        addItem(item);
        m_taskItems[task] = item;
        //
        connectSignals(item, task);
        // Add subtasks recursively
        if (task->isExpanded) {
            for (const GanttTask* child : task->children) {
                addTask(child);
            }
        }
    }

    void GanttChartScene::addMilestone(const GanttMilestone* milestone) {
        auto* item = new GanttChartItemMilestone(
            milestone, nullptr); // Assume that MilestoneItem is a subclass of GanttChartShapeItem
        // Join the scene
        addItem(item);
        //
        m_milestoneItems[milestone] = item;
        // connect signal
        connectSignals(item, milestone);
    }

    template <typename T>
    void GanttChartScene::connectSignals(GanttChartShapeItem* item, const T* obj) {
        // Safety inspection
        if (item == nullptr || obj == nullptr) {
            return;
        }
        connect(item, &GanttChartShapeItem::eventPositionChanged, this, [=](qreal deltaX) {
            qreal deltaUniTime = timeSceneRatio().fromScene(deltaX);
            using PureT = std::remove_const_t<std::remove_pointer_t<T>>;

            if constexpr (std::is_same_v<PureT, GanttTask>) {
                m_model->updateTaskTime(const_cast<GanttTask*>(obj), deltaUniTime);
            } else if constexpr (std::is_same_v<PureT, GanttMilestone>) {
                m_model->updateMilestoneTime(const_cast<GanttMilestone*>(obj), deltaUniTime);
            } else {
                qWarning() << "Unsupported object type:" << typeid(PureT).name();
            }
        });
    }

    void GanttChartScene::disconnectSignals(GanttChartShapeItem* item) {
        if (item == nullptr) {
            return;
        }
        disconnect(item);
    }

    void GanttChartScene::updateSceneTimeRange() {
        //
        TimeUnitType unitType = m_model->getTimeUnitType();
        auto [modelMinTime, modelMaxTime] = m_model->getModelTimeRange();
        if (modelMinTime > modelMaxTime) {
            return;
        }
        m_sceneMinTime = unitType.fromSeconds(modelMinTime);
        m_sceneMaxTime = unitType.fromSeconds(modelMaxTime);
    }

    void GanttChartScene::updateTimeSceneRatio() {
        qreal extraSpan = getSceneMaxDuration();
        qreal totalSpan = extraSpan + m_sceneMaxTime + 1 - m_sceneMinTime;
        if (totalSpan <= 0) {
            return;
        }
        qreal widthPerUnit = m_minChartWidth / totalSpan;

        widthPerUnit = std::ceil(widthPerUnit);

        if (widthPerUnit <= 0) {
            return;
        }
        m_timeSceneRatio.scene_per_time = widthPerUnit;
    }

} // namespace hbchart
