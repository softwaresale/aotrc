//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_PROGRAM_H
#define AOTRC_PROGRAM_H

#include <vector>
#include "instruction.h"
#include "dfa_compiler.h"
#include "src/compiler/full_match/full_match_dfa_compiler.h"

namespace aotrc::compiler {

    template <class DFACompilerTp>
    class Program {
    public:
        /**
         * Type of compiler being used to compile the DFA
         */
        using DFACompilerType = DFACompilerTp;

        Program() {
            // Make sure that the DFA compiler type can be used
            static_assert(
                    std::is_base_of_v<DFACompiler, DFACompilerTp> &&
                    std::is_default_constructible_v<DFACompilerTp>
                    );

            // Make a new DFA compiler
            // TODO this might not be necessary. We can also just make the unique ptr a DFACompilerTp
            this->dfaCompiler = std::make_unique<DFACompilerTp>();
        }

        void compile(const fa::DFA &dfa) {
            // First, compile the setup
            std::vector<InstructionPtr> setupInstructions = this->dfaCompiler->buildSetup();
            std::move(setupInstructions.begin(), setupInstructions.end(), std::back_inserter(this->instructions));

            // for each state in the DFA, compile it
            for (unsigned int state = dfa.getStartState(); state < dfa.stateCount(); state++) {
                std::vector<InstructionPtr> stateInsts = this->dfaCompiler->buildState(state, dfa);
                std::move(stateInsts.begin(), stateInsts.end(), std::back_inserter(this->instructions));
            }
            // Done
        }

        const std::vector<InstructionPtr> &getInstructions() const {
            return this->instructions;
        }

    private:
        std::vector<InstructionPtr> instructions;
        std::unique_ptr<DFACompiler> dfaCompiler;
    };

    template <class DFACompilerTp>
    std::ostream &operator<<(std::ostream &os, const Program<DFACompilerTp> &prog) {
        for (const auto &inst : prog.getInstructions()) {
            os << inst->str() << std::endl;
        }

        return os;
    }

    using FullMatchProgram = Program<FullMatchDFACompiler>;
}

#endif //AOTRC_PROGRAM_H
