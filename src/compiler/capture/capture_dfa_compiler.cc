//
// Created by charlie on 8/21/22.
//

#include "capture_dfa_compiler.h"

std::vector<aotrc::compiler::InstructionPtr> aotrc::compiler::CaptureDFACompiler::buildSetup(const aotrc::fa::DFA &dfa) {
    // auto defaultInsts = FullMatchDFACompiler::buildSetup(dfa);
    // std::vector<unsigned int> dimensions { dfa.getGroupCount(), 2 };
    // defaultInsts.push_back(std::make_unique<DeclareArrayInstruction>("groups", VariableType::SIZE, dimensions));

    // return defaultInsts;
    return FullMatchDFACompiler::buildSetup(dfa);
}

std::vector<aotrc::compiler::InstructionPtr>
aotrc::compiler::CaptureDFACompiler::buildState(unsigned int state, const aotrc::fa::DFA &dfa) {
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

        // Now, build gotos for each outgoing edge
        for (const auto &[destId, edge] : dfa.edgesForState(state)) {
            std::vector<InstructionPtr> onEdgeInsts;

            if (edge.tagged()) {
                // If the edge is tagged, then we need extra instructions
                for (const auto &tag : edge.getTags()) {
                    auto tagId = (unsigned int) abs(tag);
                    unsigned int tagIdx = tag > 0 ? 0 : 1;
                    onEdgeInsts.push_back(std::make_unique<StoreArrayInstruction>("groups", std::vector { tagId, tagIdx }, "counter"));
                }
            }

            // Inst to test if we can take the edge
            InstructionPtr testEdgeInst = std::make_unique<TestEdgeInstruction>(edge);
            // Conditionally take the edge
            InstructionPtr gotoInst;
            if (onEdgeInsts.empty()) {
                gotoInst = std::make_unique<GoToInstruction>(destId, std::move(testEdgeInst));
            } else {
                gotoInst = std::make_unique<GoToInstruction>(destId, std::move(testEdgeInst), std::move(onEdgeInsts));
            }
            // Add the instruction
            insts.push_back(std::move(gotoInst));
        }
    }

    // No edge can be taken, so reject
    insts.push_back(std::make_unique<RejectInstruction>());

    // done
    return insts;
}
