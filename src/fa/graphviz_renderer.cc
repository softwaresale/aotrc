//
// Created by charlie on 12/31/21.
//

#include "graphviz_renderer.h"
#include "dfa.h"
#include <fstream>

static void renderAcceptNodes(const aotrc::fa::TransitionTable *table, std::ostream &output) {
    for (unsigned int state = 0; state < table->stateCount(); state++) {
        if (table->isAcceptState(state)) {
            output << state << " [shape=doublecircle]" << std::endl;
        }
    }
}

static void renderEdge(const aotrc::fa::Edge &edge, std::ostream &output) {
    if (edge.epsilon()) {
        output << "$";
        return;
    }
    auto ranges = edge.getRanges();
    auto it = ranges.begin();
    for (; it != ranges.end() - 1; ++it) {
        output << it->lower;
        if (it->lower != it->upper) {
            output << '-' << it->upper;
        }
        output << ", ";
    }
    output << it->lower;
    if (it->lower != it->upper) {
        output << '-' << it->upper;
    }
}

void aotrc::fa::graphvizRenderOutput(const aotrc::fa::TransitionTable *table, const std::string &label, std::ostream &output) {
    // Write a graph
    output << "digraph " << label << " {" << std::endl;

    // Specially design accept states
    renderAcceptNodes(table, output);

    // Render out all the connections
    for (unsigned int state = 0; state < table->stateCount(); state++) {
        // Get transitions for state
        const auto& transitions = table->edgesForState(state);
        for (const auto &[destination, edge] : transitions) {
            output << state << " -> " << destination << " [ label = \"";
            renderEdge(edge, output);
            output << "\"]" << std::endl;
        }
    }

    // Finish writing graph
    output << '}' << std::endl;
}

void aotrc::fa::graphvizRenderOutputFile(const aotrc::fa::TransitionTable *table, const std::string &label, const std::string &path) {
    std::ofstream output_file(path);
    if (output_file.is_open()) {
        graphvizRenderOutput(table, label, output_file);
        output_file.close();
    } else {
        throw std::runtime_error("Could not open output file");
    }
}
