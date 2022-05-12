//
// Created by charlie on 5/11/22.
//

#include "search_dfa_compiler.h"

std::vector<aotrc::compiler::InstructionPtr> aotrc::compiler::SearchDFACompiler::buildSetup() {
    auto fullMatchSetups = FullMatchDFACompiler::buildSetup();
    // Add an extra variable
    fullMatchSetups.push_back(std::make_unique<DeclareVarInstruction>("start_pos", VariableType::SIZE));
    fullMatchSetups.push_back(std::make_unique<DeclareVarInstruction>("end_pos", VariableType::SIZE));

    return fullMatchSetups;
}

std::vector<aotrc::compiler::InstructionPtr>
aotrc::compiler::SearchDFACompiler::buildState(unsigned int state, const aotrc::fa::DFA &dfa) {
    std::vector<InstructionPtr> insts;

    bool isAccept = dfa.isAcceptState(state);

    // First, enter a new state
    insts.push_back(std::make_unique<StartStateInstruction>(state, isAccept));

    InstructionPtr onEndInst;
    if (isAccept) {
        onEndInst = std::make_unique<AcceptInstruction>();
    } else {
        onEndInst = std::make_unique<RejectInstruction>();
    }
    // Next, check if we are at the end
    insts.push_back(std::make_unique<CheckEndInstruction>(std::move(onEndInst)));

    // If this is a leaf and not an accept state, then
    if (!dfa.isLeaf(state)) {
        // If we are not at the end, then consume a new character
        insts.push_back(std::make_unique<ConsumeInstruction>());

        // Now, build gotos for each ougoing edge
        for (const auto &[destId, edge] : dfa.edgesForState(state)) {
            // Inst to test if we can take the edge
            InstructionPtr testEdgeInst = std::make_unique<TestEdgeInstruction>(edge);
            // Conditionally take the edge
            InstructionPtr gotoInst = std::make_unique<GoToInstruction>(destId, std::move(testEdgeInst));
            // TODO need a way to show that we are changing end_pos
            // Add the instruction
            insts.push_back(std::move(gotoInst));
        }
    }

    // No edge can be taken. If we are on an accept state, then accept. If we are not, then go back to
    // the first state
    if (isAccept) {
        insts.push_back(std::make_unique<AcceptInstruction>());
    } else {
        insts.push_back(std::make_unique<StoreVarInstruction>("start_pos", "counter"));
        insts.push_back(std::make_unique<GoToInstruction>(0));
    }

    // done
    return insts;
}
