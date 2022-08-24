//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_SEARCH_DFA_COMPILER_H
#define AOTRC_SEARCH_DFA_COMPILER_H

#include "src/compiler/full_match/full_match_dfa_compiler.h"

namespace aotrc::compiler {
    class SearchDFACompiler : public FullMatchDFACompiler {
    public:
        explicit SearchDFACompiler(llvm::LLVMContext &ctx)
        : FullMatchDFACompiler(ctx)
        {}

        std::vector<InstructionPtr> buildSetup(const aotrc::fa::DFA &dfa) override;
        std::vector<InstructionPtr> buildState(unsigned int state, const fa::DFA &dfa) override;
    };
}

#endif //AOTRC_SEARCH_DFA_COMPILER_H
