//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_FULL_MATCH_DFA_COMPILER_H
#define AOTRC_FULL_MATCH_DFA_COMPILER_H

#include "src/compiler/dfa_compiler.h"

namespace aotrc::compiler {
    /**
     * Compiles a DFA into a full match program.
     */
    class FullMatchDFACompiler : public DFACompiler {
    public:
        explicit FullMatchDFACompiler(llvm::LLVMContext &ctx)
        : DFACompiler(ctx) {}

        std::vector<InstructionPtr> buildSetup(const aotrc::fa::DFA &dfa) override;
        std::vector<InstructionPtr> buildState(unsigned int state, const fa::DFA &dfa) override;
    };
}

#endif //AOTRC_FULL_MATCH_DFA_COMPILER_H
