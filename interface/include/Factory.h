/**
 * File: Factory.h
 * Description: The declaration of Factory class for Gantt chart axis.
 * Author: Barbossa Zhang
 *
 * Created on 2025/04/22
 *
 * License: MIT License
 *
 * Copyright (c) 2025 Barbossa Zhang
 */
#ifndef FACTORY_H
#define FACTORY_H

#include <functional>
#include <unordered_map>

#include "Interfaces.h"

namespace hbchart {

    // Global inline registries
    inline std::unordered_map<QtMermaidType, std::function<std::unique_ptr<Core>()>>
        CoreFactoryRegistry; ///< Core factory registry
    inline std::unordered_map<QtMermaidType, std::function<std::unique_ptr<Renderer>()>>
        RendererFactoryRegistry; ///< Renderer factory registry
    inline std::unordered_map<QtMermaidType, std::function<std::unique_ptr<Parser>()>>
        ParserFactoryRegistry; ///< Parser factory registry
    inline std::unordered_map<QtMermaidType, std::function<std::unique_ptr<SyntaxHighlighter>()>>
        HighlighterFactoryRegistry; ///< Highlighter factory registry

    /**
     * @brief The factory class for creating chart core, renderer, parser, and highlighter.
     * @details The factory class is responsible for creating chart core, renderer, parser, and
     * highlighter.
     */
    class CoreFactory {
      public:
        /**
         * @brief Using Creator as a function type for creating chart core.
         */
        using Creator = std::function<std::unique_ptr<Core>()>;

        /**
         * @brief Register a chart core creator.
         * @param type The chart type.
         * @param creator The creator function for creating the chart core.
         */
        static void registerCore(QtMermaidType type, Creator creator) {
            CoreFactoryRegistry[type] = std::move(creator);
        }

        /**
         * @brief Create a chart core.
         * @param type The chart type.
         * @return The chart core.
         */
        static std::unique_ptr<Core> create(QtMermaidType type) {
            auto typeFactoryIt = CoreFactoryRegistry.find(type);
            return typeFactoryIt != CoreFactoryRegistry.end() ? typeFactoryIt->second() : nullptr;
        }
    };

    /**
     * @brief The factory class for creating chart renderer.
     * @details The factory class is responsible for creating chart renderer.
     */
    class RendererFactory {
      public:
        /**
         * @brief Using Creator as a function type for creating chart renderer.
         */
        using Creator = std::function<std::unique_ptr<Renderer>()>;

        /**
         * @brief Register a chart renderer creator.
         * @param type The chart type.
         * @param creator The creator function for creating the chart renderer.
         */
        static void registerRenderer(QtMermaidType type, Creator creator) {
            RendererFactoryRegistry[type] = std::move(creator);
        }

        /**
         * @brief Create a chart renderer.
         * @param type The chart type.
         * @return The chart renderer.
         */
        static std::unique_ptr<Renderer> create(QtMermaidType type) {
            auto typeFactoryIt = RendererFactoryRegistry.find(type);
            return typeFactoryIt != RendererFactoryRegistry.end() ? typeFactoryIt->second()
                                                                  : nullptr;
        }
    };

    /**
     * @brief The factory class for creating chart parser.
     * @details The factory class is responsible for creating chart parser.
     */
    class ParserFactory {
      public:
        using Creator = std::function<std::unique_ptr<Parser>()>;
        static void registerParser(QtMermaidType type, Creator creator) {
            ParserFactoryRegistry[type] = std::move(creator);
        }
        static std::unique_ptr<Parser> create(QtMermaidType type) {
            auto typeFactoryIt = ParserFactoryRegistry.find(type);
            return typeFactoryIt != ParserFactoryRegistry.end() ? typeFactoryIt->second() : nullptr;
        }
    };

    /**
     * @brief The factory class for creating chart highlighter.
     * @details The factory class is responsible for creating chart highlighter.
     */
    class HighlighterFactory {
      public:
        using Creator = std::function<std::unique_ptr<SyntaxHighlighter>(QTextDocument*)>;
        static void registerHighlighter(QtMermaidType type, Creator creator) {
            registry[type] = std::move(creator);
        }
        static std::unique_ptr<SyntaxHighlighter> create(QtMermaidType type,
                                                         QTextDocument* parent = nullptr) {
            auto typeFactoryIt = registry.find(type);
            return typeFactoryIt != registry.end() ? typeFactoryIt->second(parent) : nullptr;
        }

      private:
        inline static std::unordered_map<QtMermaidType, Creator> registry;
    };

#define REGISTER_CORE(Type, Class)                                                                 \
    static bool Class##_registered = []() {                                                        \
        CoreFactory::registerCore(QtMermaidType::Type,                                             \
                                  []() { return std::make_unique<Class>(); });                     \
        return true;                                                                               \
    }();

#define REGISTER_RENDERER(Type, Class)                                                             \
    static bool Class##_registered = []() {                                                        \
        RendererFactory::registerRenderer(QtMermaidType::Type,                                     \
                                          []() { return std::make_unique<Class>(); });             \
        return true;                                                                               \
    }();

#define REGISTER_PARSER(Type, Class)                                                               \
    static bool Class##_registered = []() {                                                        \
        ParserFactory::registerParser(QtMermaidType::Type,                                         \
                                      []() { return std::make_unique<Class>(); });                 \
        return true;                                                                               \
    }();

#define REGISTER_HIGHLIGHTER(Type, Class)                                                          \
    static bool Class##_registered = []() {                                                        \
        ::hbchart::HighlighterFactory::registerHighlighter(                                        \
            ::hbchart::QtMermaidType::Type,                                                        \
            [](QTextDocument* parent) { return std::make_unique<Class>(parent); });                \
        return true;                                                                               \
    }();
} // namespace hbchart
#endif // FACTORY_H