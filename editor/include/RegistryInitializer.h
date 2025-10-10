// interface/include/RegistryInitializer.h
#ifndef REGISTRY_INITIALIZER_H
#define REGISTRY_INITIALIZER_H


#include "Factory.h"
#include "QtMermaidHighlighter.h"
#include "FlowRenderer.h"
#include "FlowHighlighter.h"
#include "FlowParser.h"
// #include "FlowCore.h"
#include <QDebug>

namespace hbchart {

    /**
     * @brief Initializes the registries.
     * @details This function should be called once at the beginning of the program.
     */
    inline void initializeRegistries() {
        static bool initialized = []() {
            // Default 
            bool defaultHighlighterInit = ::hbchart::QtMermaidHighlighter_registered;
            // Flow Extend
            bool rendererInit = ::hbchart::FlowRenderer_registered;
            bool highlighterInit = ::hbchart::FlowHighlighter_registered;
            bool parserInit = ::hbchart::FlowParser_registered;
            // Flow Core
            // bool coreInit = ::hbchart::FlowCore_registered;
            qDebug() << "Registries initialized";
            return true;
        }();
    }
} // namespace hbchart

#endif // REGISTRY_INITIALIZER_H