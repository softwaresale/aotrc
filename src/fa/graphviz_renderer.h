//
// Created by charlie on 12/31/21.
//

#ifndef _GRAPHVIZ_RENDERER_H
#define _GRAPHVIZ_RENDERER_H

#include <ostream>
#include "transition_table.h"

namespace aotrc::fa {
    /**
     * Renders a transition table (a graph) into graphviz
     * @param table The transition table to render
     * @param label label for the graph
     * @param output stream to write the graphviz to
     */
    void graphvizRenderOutput(const aotrc::fa::TransitionTable *table, const std::string &label, std::ostream &output);

    /**
     * Renders graphviz for a transition table directly to a file
     * @param table table to render
     * @param label label of the graph
     * @param path path to output the graphviz to
     */
    void graphvizRenderOutputFile(const aotrc::fa::TransitionTable *table, const std::string &label, const std::string &path);
}

#endif //_GRAPHVIZ_RENDERER_H
