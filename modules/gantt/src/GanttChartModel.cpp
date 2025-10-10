/**
 * File: GanttChartModel.cpp
 * Description: The implementation of GanttChartModel class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/

#include "GanttChartModel.h"

#include <QDebug>
#include <QFont>

namespace hbchart {

    // --- GanttChartModel Implementation ---
    GanttChartModel::GanttChartModel(TimeUnitType timeUnitType, QObject* parent)
        : QObject(parent), m_timeUnitType(timeUnitType) {
        m_baseTime = 0.0;
        m_nextSectionId = 1;
        QFont titlefont = QFont();
        titlefont.setBold(true);
        constexpr int TITLE_FONT_SIZE = 14;
        titlefont.setPointSize(TITLE_FONT_SIZE);
        m_title = GanttTitle("", titlefont, Qt::black);
        m_sectionsDirty = true;
    }

    const TimeUnitType& GanttChartModel::getTimeUnitType() const { return m_timeUnitType; }

    void GanttChartModel::setTitle(const QString& title) {
        if (m_title.text == title || title.isEmpty()) {
            return;
        }
        m_title.text = title;
        m_title.visible = true;
        emit eventModelDataChanged(EventGanttUpdateType::TITLE_CHANGED);
    }

    GanttTitle& GanttChartModel::title() { return m_title; }

    const GanttTitle& GanttChartModel::getTitle() const { return m_title; }

    void GanttChartModel::setTimeUnitType(const TimeUnitType& type) {
        m_timeUnitType = type;
        emit eventModelDataChanged(EventGanttUpdateType::TIME_UNIT_CHANGED);
    }

    void GanttChartModel::clearModel() {
        //
        m_baseTime = 0.0;
        // Delete sections
        qDeleteAll(m_sections);
        m_sections.clear();
        m_sectionMap.clear();
        // Delete rootTasks (recursively delete subtasks)
        qDeleteAll(m_rootTasks);
        m_rootTasks.clear();
        // delete milestones
        qDeleteAll(m_milestones);
        m_milestones.clear();
        // Notification updates
        m_sectionsDirty = true;
        //
        emit eventModelDataChanged(EventGanttUpdateType::ALL);
    }

    bool GanttChartModel::setBaseTime(const QString& baseTime) {
        bool okay = false;
        qreal timeValue = parseTime(baseTime, &okay);
        if (!okay) {
            qWarning() << "Invalid base time";
            return false;
        }
        m_baseTime = timeValue;
        emit eventModelDataChanged(EventGanttUpdateType::BASE_TIME_CHANGED);
        return true;
    }

    qreal GanttChartModel::getBaseTime() const { return m_baseTime; }

    bool GanttChartModel::addSection(const SectionName& name) {
        m_nextSectionId++;
        auto* newSection = new GanttSection{m_nextSectionId, name, -1, -1};
        m_sections.append(newSection);
        m_sectionMap[m_nextSectionId] = newSection;
        m_sectionsDirty = true;
        emit eventModelDataChanged(EventGanttUpdateType::SECTION_CHANGED);
        return true;
    }

    bool GanttChartModel::addTask(GanttTask* parentTask, const TaskName& name,
                                  const TaskDescription& description, const TaskStart& start,
                                  const TaskEndOrDuration& endOrDuration, bool isDuration) {
        bool okStart = false;
        bool okEndOrDuration = false;
        qreal startTime = parseTime(start, &okStart);
        qreal endOrDurationTime = parseTime(endOrDuration, &okEndOrDuration);
        if (!okStart || !okEndOrDuration) {
            qWarning() << "Invalid start or end time";
            return false;
        }
        qreal endTime = 0.0;
        if (isDuration) {
            endTime = startTime + endOrDurationTime;
        } else {
            endTime = endOrDurationTime;
        }
        auto* newTask = new GanttTask{-1, name, description, startTime, endTime, {}, parentTask};
        if (parentTask != nullptr) {
            parentTask->children.append(newTask);
        } else {
            if (!m_sections.isEmpty()) {
                newTask->sectionId = m_nextSectionId;
            }
            m_rootTasks.append(newTask);
        }
        m_sectionsDirty = true;
        emit eventModelDataChanged(EventGanttUpdateType::TASK_CHANGED);
        return true;
    }

    bool GanttChartModel::addMilestone(GanttTask* parentTask, const MilestoneName& name,
                                       const MilestoneDescription& description,
                                       const MilestoneTime& time) {
        bool okTime = false;
        qreal timeValue = parseTime(time, &okTime);
        if (!okTime) {
            qWarning() << "Invalid milestone time";
            return false;
        }
        auto* newMilestone = new GanttMilestone{name, description, timeValue, parentTask};
        m_milestones.append(newMilestone);
        m_sectionsDirty = true;
        emit eventModelDataChanged(EventGanttUpdateType::MILESTONE_CHANGED);
        return true;
    }

    const QList<GanttSection*>& GanttChartModel::getSections() {
        if (m_sectionsDirty) {
            updateSectionInfos();
            m_sectionsDirty = false;
        }
        return m_sections;
    }

    const QList<GanttTask*>& GanttChartModel::getRootTasks() const { return m_rootTasks; }

    const QList<GanttMilestone*>& GanttChartModel::getMilestones() const { return m_milestones; }

    qreal GanttChartModel::getLongestTaskDuration() const {
        qreal maxDuration = 0.0;
        for (const GanttTask* task : m_rootTasks) {
            qreal duration = task->end - task->start;
            maxDuration = std::max(duration, maxDuration);
        }
        return maxDuration;
    }

    void GanttChartModel::updateTaskTime(GanttTask* task, qreal unit_time) const {
        if (task == nullptr) {
            return;
        }
        qreal newStart = task->start + unit_time;
        qreal duration = task->end - task->start;
        qreal newEnd = newStart + duration;
        auto [minTime, maxTime] = getTaskTimesRange();
        if (newStart < minTime || newEnd > maxTime) {
            qWarning() << "Task time out of project range";
            return;
        }
        task->start = newStart;
        task->end = newEnd;
    }

    void GanttChartModel::updateMilestoneTime(GanttMilestone* milestone, qreal unit_time) const {
        if (milestone == nullptr) {
            return;
        }
        qreal newTime = milestone->time + unit_time;
        auto [minTime, maxTime] = getTaskTimesRange();
        if (newTime < minTime || newTime > maxTime) {
            qWarning() << "Milestone time out of project range";
            return;
        }
        milestone->time = newTime;
    }

    int GanttChartModel::getTaskRowsNumber() const {
        int totalRows = 0;
        for (const GanttTask* task : m_rootTasks) {
            totalRows += calculateTaskRows(task);
        }
        return totalRows;
    }

    QPair<qreal, qreal> GanttChartModel::getModelTimeRange() const {
        auto [minTime, maxTime] = getTaskTimesRange();
        auto [minMilestoneTime, maxMilestoneTime] = getMilestoneTimeRange();
        // If there is a valid milestone (because there may be no milestone)
        if (minMilestoneTime <= maxMilestoneTime) {
            minTime = std::min(minTime, minMilestoneTime);
            maxTime = std::max(maxTime, maxMilestoneTime);
        }
        return {minTime, maxTime};
    }

    QPair<qreal, qreal> GanttChartModel::getTaskTimesRange() const {
        qreal minTime = std::numeric_limits<qreal>::max();
        qreal maxTime = std::numeric_limits<qreal>::lowest();
        bool first = true;
        for (const GanttTask* task : m_rootTasks) {
            calculateTaskTimeRange(task, minTime, maxTime, first);
        }
        return {minTime, maxTime};
    }

    QPair<qreal, qreal> GanttChartModel::getMilestoneTimeRange() const {
        qreal minTime = std::numeric_limits<qreal>::max();
        qreal maxTime = std::numeric_limits<qreal>::lowest();
        for (const GanttMilestone* milestone : m_milestones) {
            minTime = std::min(minTime, milestone->time);
            maxTime = std::max(maxTime, milestone->time);
        }
        return {minTime, maxTime};
    }

    void GanttChartModel::calculateTaskTimeRange(const GanttTask* task, qreal& minTime,
                                                 qreal& maxTime, bool& first) const {
        if (first) {
            minTime = task->start;
            maxTime = task->end;
            first = false;
        } else {
                minTime = std::min(minTime, task->start);
                maxTime = std::max(maxTime, task->end);
        }

        if (task->isExpanded) {
            for (const GanttTask* child : task->children) {
                calculateTaskTimeRange(child, minTime, maxTime, first);
            }
        }
    }

    int GanttChartModel::calculateTaskRows(const GanttTask* task) const {
        int rows = 1;
        if (task->isExpanded) {
            for (const GanttTask* child : task->children) {
                rows += calculateTaskRows(child);
            }
        }
        return rows;
    }

    void GanttChartModel::updateSectionInfos() {
        if (m_sections.isEmpty() || m_rootTasks.isEmpty()) {
            return;
        }

        for (GanttSection* section : m_sections) {
            section->taskCount = 0;
            section->taskIndex = -1;
        }
        int taskIndex = 0;
        for (GanttTask* task : m_rootTasks) {
            int taskCount = calculateTaskRows(task);
            if (task->sectionId != -1) {
                GanttSection* section = m_sectionMap[task->sectionId];
                if (section != nullptr) {
                    section->taskCount += taskCount;
                    if (section->taskIndex == -1) {
                        section->taskIndex = taskIndex;
                    }
                }
            }
            taskIndex += taskCount;
        }
    }

} // namespace hbchart
