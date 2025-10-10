
/**
 * File: GanttParser.h
 * Description: The declaration of the GanttParser class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/20
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef GANTTPARSER_H
#define GANTTPARSER_H

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     * @brief The GanttParser class. ihnerits from Parser.
     */
    class GanttParser : public Parser {
      public:
        /**
         * @brief GanttParser constructor
         */
        GanttParser();
        ~GanttParser() override = default; ///< Destructor

        /**
         * @brief Parse the input text to the chart graph data.
         * @param input The input text.
         * @return The chart graph data.
         */
        ChartGraph* parse(const QString& input) override;
    };
    REGISTER_PARSER(GANTT, GanttParser)
} // namespace hbchart

#endif // GANTTPARSER_H