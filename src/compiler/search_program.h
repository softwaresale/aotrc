//
// Created by charlie on 4/9/22.
//

#ifndef AOTRC_SEARCH_PROGRAM_H
#define AOTRC_SEARCH_PROGRAM_H

#include "program.h"

namespace aotrc::compiler {

    /**
     * Like a submatch program, except this one actually keeps track of where the sub match occurs
     */
    class SearchProgram : public Program<SubMatchProgramMode> {
    public:
        SearchProgram(std::string name, llvm::LLVMContext &ctx, const std::unique_ptr<llvm::Module> &mod);

        void build(const fa::DFA &dfa) override;
    };
}

#endif //AOTRC_SEARCH_PROGRAM_H
