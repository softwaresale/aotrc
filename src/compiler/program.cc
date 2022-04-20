//
// Created by charlie on 3/13/22.
//

#include "program.h"

#include <utility>

template <typename ProgramModeTp>
aotrc::compiler::Program<ProgramModeTp>::Program(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module)
: name(std::move(name))
, function(nullptr) /* this gets changed later */ {
    // Make sure that program mode can be used
    static_assert(
            std::is_base_of_v<aotrc::compiler::BaseProgramMode, ProgramModeTp> &&
            std::is_default_constructible_v<ProgramModeTp>
            );

    ProgramModeTp programMode;
    // Create a new function for this program to use
    std::string functionName = programMode.getFunctionName(this->name);
    // Two arguments
    auto matchFuncType = programMode.getFunctionType(ctx);
    // Create the function
    module->getOrInsertFunction(functionName, matchFuncType);
    this->function = module->getFunction(functionName);
    // Create a new program state
    this->programState = programMode.getProgramState(this->function);
}

template <class ProgramModeTp>
void aotrc::compiler::Program<ProgramModeTp>::build(const aotrc::fa::DFA &dfa) {
    // For each state
    for (unsigned int state = 0; state < dfa.stateCount(); state++) {
        // Push a new start_state instruction
        this->instructions.push_back(std::make_unique<StartStateInstruction>(state));

        // Check if we are at the end. If this is an accept state, accept. Otherwise, reject
        std::unique_ptr<Instruction> onEndInst;
        if (dfa.isAcceptState(state)) {
            onEndInst = std::make_unique<AcceptInstruction>();
        } else {
            onEndInst = std::make_unique<RejectInstruction>();
        }
        auto checkEndInst = std::make_unique<CheckEndInstruction>(std::move(onEndInst));
        this->instructions.push_back(std::move(checkEndInst));

        // If this state is a leaf, then just bypass the edges and such all together and just add a reject
        if (dfa.isLeaf(state)) {
            this->instructions.push_back(std::make_unique<RejectInstruction>());
            continue;
        }

        // Consume a character from the input string
        this->instructions.push_back(std::make_unique<ConsumeInstruction>());

        // For each outgoing edge...
        for (const auto &[dest, edge] : dfa.edgesForState(state)) {
            // Create a new goto instruction that tests all the ranges in edge
            auto edgeTest = std::make_unique<TestInstruction>(edge.getRanges());
            this->instructions.push_back(std::make_unique<GotoInstruction>(dest, std::move(edgeTest)));
        }

        // At this point, none of the other edges would pass, so we reject
        this->instructions.push_back(std::make_unique<RejectInstruction>());
    }
}

template <class ProgramModeTp>
void aotrc::compiler::Program<ProgramModeTp>::compile() {
    for (const auto &instruction : this->instructions) {
        // compile the instruction
        instruction->build(this->programState);
    }
}

template <class ProgramModeTp>
void aotrc::compiler::Program<ProgramModeTp>::runPass(std::unique_ptr<Pass> &pass) {
    for (auto it = this->instructions.begin(); it != this->instructions.end();) {
        it = std::invoke(*pass, *it, it, this->instructions);
        ++it;
    }
}

/*
 * Build out implementations for each program mode
 */
template class aotrc::compiler::Program<aotrc::compiler::FullMatchProgramMode>;
template class aotrc::compiler::Program<aotrc::compiler::SubMatchProgramMode>;
template class aotrc::compiler::Program<aotrc::compiler::SearchProgramMode>;
