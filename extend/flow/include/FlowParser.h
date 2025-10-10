
/**
 * File: FlowParser.h
 * Description: The declaration of the FlowParser class.
 * Author: Barbossa Zhang
 *
 * Created on 2025/05/08
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
*/
#ifndef FLOWPARSER_H
#define FLOWPARSER_H

#include "Factory.h"
#include "Interfaces.h"

namespace hbchart {
    /**
     * @brief The FlowParser class. ihnerits from Parser.
     */
    class FlowParser : public Parser {
      public:
        /**
         * @brief FlowParser constructor
         */
        FlowParser();
        ~FlowParser() override = default; ///< Destructor

        /**
         * @brief Parse the input text to the chart graph data.
         * @param input The input text.
         * @return The chart graph data.
         */
        ChartGraph* parse(const QString& input) override;
    };
    REGISTER_PARSER(FLOW, FlowParser)
} // namespace hbchart

#endif // FLOWPARSER_H