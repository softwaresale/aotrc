//
// Created by charlie on 3/13/22.
//

#include "program.h"

#include <utility>

aotrc::compiler::Program::Program(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &module)
: name(std::move(name)) {
    // Create a new function for this program to use
    std::string functionName = this->name + "_match";
    // Two arguments
    std::vector<llvm::Type *> paramTypes = {llvm::Type::getInt8PtrTy(ctx), llvm::Type::getInt64Ty(ctx)};
    auto matchFuncType = llvm::FunctionType::get(llvm::Type::getInt1Ty(ctx), paramTypes, false);
    // Create the function
    module->getOrInsertFunction(functionName, matchFuncType);
    this->function = module->getFunction(functionName);
    // Create a new program state
    this->programState = std::make_unique<ProgramState>(this->function);
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
