/**
 * File: GanttChartModel.h
 * Description: The declaration of GanttChartModel class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */

#ifndef GANTTCHARTMODEL_H
#define GANTTCHARTMODEL_H

#include <QColor>
#include <QDateTime>
#include <QFont>
#include <QList>
#include <QObject>
#include <QVariant>
#include <QDebug>

namespace hbchart {

    /**
     * @brief The template class StrongType.
     * @details This class is used to define strong types for task name, task description, task
     * start time, etc.
     * @tparam Tag The tag type.
     */
    template <typename Tag, typename ValueType>
    struct StrongType {
        // Tag type
        ValueType value;
        
        // Allow explicit construction (recommended to avoid implicit conversion)
        explicit StrongType(ValueType val) : value(std::move(val)) {}
        
        // Provides conversion to ValueType (optional)
        operator ValueType() const { return value; }
    };

    using SectionName = StrongType<struct SectionNameTag, QString>;                             ///< Strong type for section name
    using TaskName = StrongType<struct TaskNameTag, QString>;                                   ///< Strong type for task name
    using TaskDescription = StrongType<struct TaskDescTag, QString>;                            ///< Strong type for task description
    using TaskStart = StrongType<struct TaskStartTag, QString>;                                 ///< Strong type for task start time
    using TaskEndOrDuration = StrongType<struct EndOrDurationTag, QString>;                     ///< Strong type for task end or duration
    using MilestoneName = StrongType<struct MilestoneNameTag, QString>;                         ///< Strong type for milestone name
    using MilestoneDescription = StrongType<struct MilestoneDescTag, QString>;                  ///< Strong type for milestone description
    using MilestoneTime = StrongType<struct MilestoneTimeTag, QString>;                         ///< Strong type for milestone time

    /**
     * @brief The TimeUnit enum.
     */
    enum class TimeUnit : std::uint8_t {
        NONE,   ///< No unit, used for custom or undefined cases
        CUSTOM, ///< Unit length, real world length
        SECOND, ///< seconds
        MINUTE, ///< minutes
        HOUR,   ///< hours
        DAY,    ///< days
        WEEK,   ///< weeks
        MONTH,  ///< months
        YEAR    ///< years
    };

    /**
     * @brief The TimeUnitType struct.
     */
    struct TimeUnitType {
        TimeUnit unit;
        qreal seconds_per_unit;

        /**
         * @brief Constructor.
         * @param unt The time unit.
         * @param sec The seconds per unit.
         */
        TimeUnitType(TimeUnit unt, qreal sec) : unit(unt), seconds_per_unit(sec) {}

        /**
         * @brief Convert model to seconds.
         * @param unt The model unit time.
         * @return The seconds.
         */
        [[nodiscard]] qreal toSeconds(qreal unt) const { return unt * seconds_per_unit; }

        /**
         * @brief Convert seconds to model.
         * @param sec The seconds.
         * @return The model unit time.
         */
        [[nodiscard]] qreal fromSeconds(qreal sec) const { return sec / seconds_per_unit; }
    };

    /**
     * @brief The ViewTimeUnit factory function.
     * @param unit The time unit.
     * @param customSec The custom unit length in seconds.
     * @return The TimeUnitType object.
     */

    inline TimeUnitType makeTimeUnit(TimeUnit unit, qreal customSec = 1.0) {
        constexpr qreal SECONDS_IN_MINUTE = 60.0;
        constexpr qreal SECONDS_IN_HOUR = 60.0 * SECONDS_IN_MINUTE;  // 3600
        constexpr qreal SECONDS_IN_DAY = 24.0 * SECONDS_IN_HOUR;     // 86400
        constexpr qreal SECONDS_IN_WEEK = 7.0 * SECONDS_IN_DAY;      // 604800
        constexpr qreal SECONDS_IN_MONTH = 30.4375 * SECONDS_IN_DAY; // ≈2628000
        constexpr qreal SECONDS_IN_YEAR = 365.25 * SECONDS_IN_DAY;   // ≈31536000
        switch (unit) {
        case TimeUnit::SECOND:
            return {unit, 1.0};
        case TimeUnit::MINUTE:
            return {unit, SECONDS_IN_MINUTE};
        case TimeUnit::HOUR:
            return {unit, SECONDS_IN_HOUR};
        case TimeUnit::DAY:
            return {unit, SECONDS_IN_DAY};
        case TimeUnit::WEEK:
            return {unit, SECONDS_IN_WEEK};
        case TimeUnit::MONTH:
            return {unit, SECONDS_IN_MONTH};
        case TimeUnit::YEAR:
            return {unit, SECONDS_IN_YEAR};
        case TimeUnit::CUSTOM:
            return {unit, customSec};
        default:
            return {unit, 1.0};
        }
    }

    // Time parser, can parse 1d, 1h, 1m, 1s, 1w, 1m, 1y to convert it into seconds
    inline qreal parseTime(const QString& str, bool* okay = nullptr) {
        if (okay != nullptr) {
            *okay = false;
        }
        qreal result = 0.0;
        QString cleanStr = str.simplified();
        if (cleanStr.isEmpty()) {
            return result;
        }

        // Try to parse the date (e.g., "2025-04-27")
        QDateTime dti = QDateTime::fromString(cleanStr, "yyyy-MM-dd");
        if (dti.isValid()) {
            if (okay != nullptr) {
                *okay = true;
            }
            return (qreal)dti.toSecsSinceEpoch();
        }

        // Parsing time string ("1d 2h")
        // qDebug() << "Parsing time string: " << cleanStr;
        QStringList segments = cleanStr.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        // qDebug() << "Segments: " << segments;
        bool parsedAny = false;
        QRegExp rxe(R"((-?\d+(?:\.\d+)?)([a-zA-Z]+)?)");
        for (const QString& segment : segments) {
            if (rxe.indexIn(segment) != -1) {
                bool convertOk;
                qreal value = rxe.cap(1).toDouble(&convertOk);
                if (!convertOk) {
                    continue;
                }
                QString unit = rxe.cap(2).toLower();
                // qDebug() << "Value: " << value << " Unit: " << unit;
                TimeUnitType type = makeTimeUnit(TimeUnit::NONE);
                if (unit.isEmpty()) {
                    type = makeTimeUnit(TimeUnit::NONE);
                } else if (unit == "s") {
                    type = makeTimeUnit(TimeUnit::SECOND);
                } else if (unit == "m") {
                    type = makeTimeUnit(TimeUnit::MINUTE);
                } else if (unit == "h") {
                    type = makeTimeUnit(TimeUnit::HOUR);
                } else if (unit == "d") {
                    type = makeTimeUnit(TimeUnit::DAY);
                } else if (unit == "w") {
                    type = makeTimeUnit(TimeUnit::WEEK);
                } else if (unit == "mo") {
                    type = makeTimeUnit(TimeUnit::MONTH);
                } else if (unit == "y") {
                    type = makeTimeUnit(TimeUnit::YEAR);
                } else {
                    continue;
                }
                result += type.toSeconds(value);
                parsedAny = true;
            }
        }

        if (okay != nullptr && parsedAny) {
            *okay = true;
        }
        return result;
    }

    /**
     * @brief GanttChart Title
     * @details Title is used to display text on the top of the chart,
     * it can be used to display some information or instructions.
     */
    struct GanttTitle {
        QString text;         ///< title text
        QFont font;           ///< font
        QColor color;         /// title color
        bool visible = false; /// whether title is visible

        GanttTitle() = default;

        GanttTitle(QString text_, const QFont& font_ = QFont(), QColor color_ = Qt::black)
            : text(std::move(text_)), font(font_), color(std::move(color_)) {}
    };

    /**
     * @brief The GanttSection struct.
     */
    struct GanttSection {
        int id;        ///< Section id
        QString name;  ///< Section name
        int taskIndex; ///< Task index
        int taskCount; ///< Task count
    };

    /**
     * @brief The GanttTask struct.
     */
    struct GanttTask {
        int sectionId;               ///< Section id
        QString name;                ///< Task name
        QString description;         ///< Task description
        qreal start;                 ///< Unit time
        qreal end;                   ///< Unit time
        QList<GanttTask*> children;  ///< Tasks list
        GanttTask* parent = nullptr; ///< Parent task
        bool isExpanded = true;      ///< Whether the task is expanded or not
        ~GanttTask() { qDeleteAll(children); }
    };

    /**
     * @brief The GanttMilestone struct.
     */
    struct GanttMilestone {
        QString name;                ///< Milestone name
        QString description;         ///< Milestone description
        qreal time;                  ///< Unit time
        GanttTask* parent = nullptr; ///< Tasks list
    };

    /**
     * @brief The EventGanttUpdateType enum.
     */
    enum class EventGanttUpdateType : std::uint8_t {
        ALL,
        BASE_TIME_CHANGED,
        TIME_UNIT_CHANGED,
        TITLE_CHANGED,
        SECTION_CHANGED,
        TASK_CHANGED,
        MILESTONE_CHANGED
    };

    /**
     * @brief The GanttChartModel class.
     */
    class GanttChartModel : public QObject {
        Q_OBJECT

      private:
        GanttTitle m_title;                    ///< Title
        TimeUnitType m_timeUnitType;           ///< Unit time type
        qreal m_baseTime;                      ///< Base time
        int m_nextSectionId;                   ///< Self-incremented section id
        bool m_sectionsDirty;                  ///< Whether the sections are dirty or not
        bool m_tasksDirty;                     ///< Whether the tasks are dirty or not
        bool m_milestonesDirty;                ///< Whether the milestones are dirty or not
        QList<GanttSection*> m_sections;       ///< Section list
        QList<GanttTask*> m_rootTasks;         ///< Root tasks list
        QList<GanttMilestone*> m_milestones;   ///< Milestones list
        QMap<int, GanttSection*> m_sectionMap; ///< Section map

      signals:
        /**
         * @brief Data change signal.
         */
        void eventModelDataChanged(EventGanttUpdateType type); ///< Data change signal

      public:
        /**
         * @brief Constructor.
         */
        explicit GanttChartModel(TimeUnitType timeUnitType, QObject* parent = nullptr);
        ~GanttChartModel() override = default; ///< Destructor.

        /**
         * @brief Set the title
         * @param title The title to set
         */
        void setTitle(const QString& title);

        /**
         * @brief Get the title
         * @return The title
         */
        GanttTitle& title();

        /**
         * @brief Set the time unit type.
         * @param type The time unit type.
         */
        [[nodiscard]] const GanttTitle& getTitle() const;

        /**
         * @brief Get the time unit type.
         * @return The time unit type.
         */
        [[nodiscard]] const TimeUnitType& getTimeUnitType() const;

        /**
         * @brief Set the time unit type.
         * @param type The time unit type.
         */
        void setTimeUnitType(const TimeUnitType& type);

        /**
         * @brief Clear the model.
         * @note This method clears all sections, root tasks and milestones.
         */
        void clearModel();

        /**
         * @brief Set the base time.
         * @param baseTime The base time.
         * @return Whether the base time is set successfully.
         */
        bool setBaseTime(const QString& baseTime);

        /**
         * @brief Get the base time.
         * @return The base time in seconds.
         */
        [[nodiscard]] qreal getBaseTime() const;

        /**
         * @brief Add a section.
         * @param name The section name.
         * @return Whether the section is added successfully.
         */
        bool addSection(const SectionName& name);

        /**
         * @brief Add a task.
         * @param parentTask The parent task.
         * @param name The task name.
         * @param description The task description.
         * @param start The start time
         * @param endOrDuration The end time or duration
         * @param isDuration Whether the end time is a duration.
         * @return Whether the task is added successfully.
         */
        bool addTask(GanttTask* parentTask, const TaskName& name, const TaskDescription& description,
                     const TaskStart& start, const TaskEndOrDuration& endOrDuration, bool isDuration = false);

        /**
         * @brief Add a milestone.
         * @param name The milestone name.
         * @param time The milestone time in unit time.
         * @return Whether the milestone is added successfully.
         */
        bool addMilestone(GanttTask* parentTask, const MilestoneName& name, const MilestoneDescription& description,
                          const MilestoneTime& time);

        /**
         * @brief Get the sections list.
         * @return The sections list.
         */
        const QList<GanttSection*>& getSections();

        /**
         * @brief Get the root tasks list.
         * @return The root tasks list.
         */
        [[nodiscard]] const QList<GanttTask*>& getRootTasks() const;

        /**
         * @brief Get the milestones list.
         * @return The milestones list.
         */
        [[nodiscard]] const QList<GanttMilestone*>& getMilestones() const;

        /**
         * @brief Get task total number of rows.
         * @return The total number of rows.
         */
        [[nodiscard]] int getTaskRowsNumber() const;

        /**
         * @brief Get the task by index.
         * @param index The task index.
         * @return The task.
         */
        [[nodiscard]] QPair<qreal, qreal> getModelTimeRange() const;

        /**
         * @brief Calculate the time range.
         * @return The time range.
         */
        [[nodiscard]] QPair<qreal, qreal> getTaskTimesRange() const;

        /**
         * @brief Get the time range of a milestone.
         * @return The time range.
         */
        [[nodiscard]] QPair<qreal, qreal> getMilestoneTimeRange() const;

        /**
         * @brief Get the longest task duration.
         * @return The longest task duration.
         */
        [[nodiscard]] qreal getLongestTaskDuration() const;

        /**
         * @brief Update a task in the scene
         * @param task The task to update
         * @param unit_time The delta time in unit time
         * @note This method is called by updateScene() to update a task in the scene
         */
        void updateTaskTime(GanttTask* task, qreal unit_time) const;

        /**
         * @brief Update a milestone in the scene
         * @param milestone The milestone to update
         * @param unit_time The delta time in unit time
         * @note This method is called by updateScene() to update a milestone in the scene
         */
        void updateMilestoneTime(GanttMilestone* milestone, qreal unit_time) const;

      private:
        /**
         * @brief Calculate the time range of a task.
         * @param task The task.
         * @param minTime The minimum time.
         * @param maxTime The maximum time.
         * @param first Whether it is the first task.
         */
        void calculateTaskTimeRange(const GanttTask* task, qreal& minTime, qreal& maxTime,
                                    bool& first) const;

        /**
         * @brief Calculate the number of rows of a task.
         * @param task The task.
         * @return The number of rows.
         */
        int calculateTaskRows(const GanttTask* task) const;

        /**
         * @brief Update the section infos
         */
        void updateSectionInfos();
    };
} // namespace hbchart

#endif // GANTTCHARTMODEL_H
