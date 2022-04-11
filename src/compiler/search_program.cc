//
// Created by charlie on 4/9/22.
//

#include "search_program.h"
#include "passes/flag_start_state_pass.h"
#include "passes/reject_to_conditional.h"

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

    // 2. Replace each standalone reject (i.e. the reject instructions if no edge is taken) with either a
    // goto start or an accept.
    std::function<std::unique_ptr<Instruction>()> acceptProducer = [](){ return std::make_unique<StoreLocationAcceptInstruction>(); };
    std::unique_ptr<Pass> conditionalRejectPass = std::make_unique<RejectToConditionalPass>(dfa, acceptProducer);

    // Now, run the passes
    this->runPass(flagStartStatePass);
    this->runPass(conditionalRejectPass);
}
