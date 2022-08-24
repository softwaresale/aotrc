//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_SUB_MATCH_DFA_COMPILER_H
#define AOTRC_SUB_MATCH_DFA_COMPILER_H

#include "src/compiler/full_match/full_match_dfa_compiler.h"

namespace aotrc::compiler {

    class SubMatchDFACompiler : public FullMatchDFACompiler {
    public:
        explicit SubMatchDFACompiler(llvm::LLVMContext &ctx)
        : FullMatchDFACompiler(ctx)
        {}

        std::vector<InstructionPtr> buildState(unsigned int state, const fa::DFA &dfa) override;
    };

}

#endif //AOTRC_SUB_MATCH_DFA_COMPILER_H
