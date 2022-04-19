//
// Created by charlie on 4/9/22.
//

#include "search_program.h"
#include "passes/flag_start_state_pass.h"
#include "passes/reject_to_conditional.h"
#include "src/compiler/passes/accept_to_store_loc_pass.h"

aotrc::compiler::SearchProgram::SearchProgram(std::string name, llvm::LLVMContext &ctx,
                                              const std::unique_ptr<llvm::Module> &mod)
: Program(std::move(name), ctx, mod) {
    // All the same program stuff should be used
}

void aotrc::compiler::SearchProgram::build(const aotrc::fa::DFA &dfa) {
    // Build out a normal
    Program::build(dfa);
    // 1. For each accept state, insert an Enter Accept state inst after the enter state inst
    std::unique_ptr<Pass> flagStartStatePass = std::make_unique<aotrc::compiler::FlagStartStatePass>(dfa);

    // 2. Replace all store instructions with a store location accept instruction
    std::unique_ptr<Pass> acceptToStoreLocPass = std::make_unique<aotrc::compiler::AcceptToStoreLocPass>();

    // 3. Replace each standalone reject (i.e. the reject instructions if no edge is taken) with either a
    // goto start or an accept.
    std::function<std::unique_ptr<Instruction>()> acceptProducer = [](){ return std::make_unique<StoreLocationAcceptInstruction>(); };
    std::unique_ptr<Pass> conditionalRejectPass = std::make_unique<RejectToConditionalPass>(dfa, acceptProducer);

    // 4. Add a set start position at the beginning of state 0 that sets the start location to the value of the
    // current counter
    Pass::OperatorType setStartPos = [this](std::unique_ptr<Instruction> &currentInst, Pass::InstructionPos pos, Pass::InstructionList &instructions) {
        if (currentInst->type() == START_STATE) {
            auto startStateInst = dynamic_cast<aotrc::compiler::StartStateInstruction*>(currentInst.get());
            if (startStateInst->getId() == 0) {
                // We are at the start state. Insert an instruction build the state
                auto searchProgramState = static_cast<aotrc::compiler::SearchProgramState *>(this->programState.get());
                std::unique_ptr<Instruction> setStoreInst = std::make_unique<aotrc::compiler::StoreVariableInstruction>(searchProgramState->getStartPos(), searchProgramState->getCounter());
                // Insert this instruction after the start state instruction
                auto before = std::next(pos);
                return instructions.insert(before, std::move(setStoreInst));
            }
        }

        return pos;
    };
    std::unique_ptr<Pass> setStartPosPass = std::make_unique<Pass>(setStartPos);

    // Now, run the passes
    this->runPass(flagStartStatePass);
    this->runPass(acceptToStoreLocPass);
    this->runPass(conditionalRejectPass);
    this->runPass(setStartPosPass);
}
