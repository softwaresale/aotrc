//
// Created by charlie on 12/31/21.
//

#ifndef _GRAPHVIZ_RENDERER_H
#define _GRAPHVIZ_RENDERER_H

#include <ostream>
#include "transition_table.h"

namespace aotrc::fa {
    void graphvizRenderOutput(const aotrc::fa::TransitionTable *table, const std::string &label, std::ostream &output);
}

#endif //_GRAPHVIZ_RENDERER_H
