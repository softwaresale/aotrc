//
// Created by charlie on 3/13/22.
//

#include "program.h"

#include <utility>

aotrc::compiler::Program::Program(std::string name, ProgramMode progType, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module)
: name(std::move(name)) {
    // Create a new function for this program to use
    std::string functionName = aotrc::compiler::getProgramType(progType)->getFunctionName(this->name);
    // Two arguments
    auto matchFuncType = getProgramType(progType)->getFunctionType(ctx);
    // Create the function
    module->getOrInsertFunction(functionName, matchFuncType);
    this->function = module->getFunction(functionName);
    // Create a new program state
    this->programState = getProgramType(progType)->getProgramState(this->function);
}

std::ostream &aotrc::compiler::operator<<(std::ostream &os, const aotrc::compiler::Program &program) {
    for (const auto &instruction : program.getInstructions()) {
        os << instruction->str() << '\n';
    }
    return os;
}

void aotrc::compiler::Program::build(const aotrc::fa::DFA &dfa) {
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

void aotrc::compiler::Program::compile() {
    for (const auto &instruction : this->instructions) {
        // compile the instruction
        instruction->build(this->programState);
    }
}

void aotrc::compiler::Program::runPass(std::unique_ptr<Pass> &pass) {
    for (auto it = this->instructions.begin(); it != this->instructions.end();) {
        it = std::invoke(*pass, *it, it, this->instructions);
        ++it;
    }
}
