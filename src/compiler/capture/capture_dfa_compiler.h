//
// Created by charlie on 8/21/22.
//

#ifndef AOTRC_CAPTURE_DFA_COMPILER_H
#define AOTRC_CAPTURE_DFA_COMPILER_H

#include "src/compiler/full_match/full_match_dfa_compiler.h"

namespace aotrc::compiler {
    class CaptureDFACompiler : public FullMatchDFACompiler {
    public:
        std::vector<InstructionPtr> buildSetup(const aotrc::fa::DFA &dfa) override;
        std::vector<InstructionPtr> buildState(unsigned int state, const fa::DFA &dfa) override;
    };
} // compiler

#endif //AOTRC_CAPTURE_DFA_COMPILER_H
