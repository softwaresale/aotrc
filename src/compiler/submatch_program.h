//
// Created by charlie on 3/19/22.
//

#ifndef AOTRC_SUBMATCH_PROGRAM_H
#define AOTRC_SUBMATCH_PROGRAM_H

#include "program.h"

namespace aotrc::compiler {
    /**
     * Like a program, except sub-matching is supported. Sub-matching gets the longest
     * left-most match.
     *
     * Note: if you're matching something that accepts the empty
     * string (e.g. a*), then the left-most longest match will likely be the empty string
     * as well.
     */
    class SubMatchProgram : public Program<SubMatchProgramMode> {
    public:
        SubMatchProgram(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &mod);
        void build(const fa::DFA &dfa) override;
    };
}

#endif //AOTRC_SUBMATCH_PROGRAM_H
