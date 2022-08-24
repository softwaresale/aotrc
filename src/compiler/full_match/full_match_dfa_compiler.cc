//
// Created by charlie on 5/11/22.
//

#include "full_match_dfa_compiler.h"

std::vector<aotrc::compiler::InstructionPtr> aotrc::compiler::FullMatchDFACompiler::buildSetup() {
    // Declare two variables:
    std::vector<InstructionPtr> insts;
    // counter - keeps track of where in the subject string we are
    insts.push_back(std::make_unique<DeclareVarInstruction>("counter", this->ctx, VariableType::SIZE));
    // cursor - the current value being examined in the subject string
    insts.push_back(std::make_unique<DeclareVarInstruction>("cursor", this->ctx, VariableType::CHAR));
    return insts;
}

std::vector<aotrc::compiler::InstructionPtr>
aotrc::compiler::FullMatchDFACompiler::buildState(unsigned int state, const aotrc::fa::DFA &dfa) {
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
            // Add the instruction
            insts.push_back(std::move(gotoInst));
        }
    }

    // No edge can be taken, so reject
    insts.push_back(std::make_unique<RejectInstruction>());

    // done
    return insts;
}
