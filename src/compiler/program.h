//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_PROGRAM_H
#define AOTRC_PROGRAM_H

#include <vector>
#include "instruction.h"
#include "dfa_compiler.h"
#include "src/compiler/full_match/full_match_dfa_compiler.h"
#include "src/compiler/sub_match/sub_match_dfa_compiler.h"
#include "src/compiler/searching/search_dfa_compiler.h"
#include "src/compiler/capture/capture_dfa_compiler.h"

namespace aotrc::compiler {

    /**
     * A high-level program that describes a DFA in a sequence of HIR instructions.
     * This is the first intermediate representation that a DFA will take.
     *
     * TODO make this class iterable (like LLVM)
     * @tparam DFACompilerTp The DFA compiler to use to compile a DFA into a sequence of instructions
     */
    template <class DFACompilerTp>
    class Program {
    public:
        /**
         * Type of compiler being used to compile the DFA
         */
        using DFACompilerType = DFACompilerTp;

        /**
         * Creates an empty program
         */
        explicit Program(llvm::LLVMContext &ctx)
        : compiled(false) {
            // Make sure that the DFA compiler type can be used
            static_assert(
                    std::is_base_of_v<DFACompiler, DFACompilerTp> &&
                    std::is_constructible_v<DFACompilerTp, llvm::LLVMContext&>
                    );

            // Make a new DFA compiler
            // TODO this might not be necessary. We can also just make the unique ptr a DFACompilerTp
            this->dfaCompiler = std::make_unique<DFACompilerTp>(ctx);
        }

        /**
         * Compiles the given DFA into a sequence of instructions. This should only be called once. If it
         * has been compiled, and is compiled again, it will throw a runtime_error
         * @param dfa The DFA to compile
         */
        void compile(const fa::DFA &dfa) {
            if (compiled)
                throw std::runtime_error("Program has already been compiled");

            // First, compile the setup
            std::vector<InstructionPtr> setupInstructions = this->dfaCompiler->buildSetup(dfa);
            std::move(setupInstructions.begin(), setupInstructions.end(), std::back_inserter(this->instructions));

            // for each state in the DFA, compile it
            for (unsigned int state = dfa.getStartState(); state < dfa.stateCount(); state++) {
                std::vector<InstructionPtr> stateInsts = this->dfaCompiler->buildState(state, dfa);
                std::move(stateInsts.begin(), stateInsts.end(), std::back_inserter(this->instructions));
            }
            // Done
            compiled = true;
        }

        /**
         * Gets the sequence of instructions for this program
         * @return The vector of instructions used by this program
         */
        const std::vector<InstructionPtr> &getInstructions() const {
            return this->instructions;
        }

    private:
        std::vector<InstructionPtr> instructions;
        std::unique_ptr<DFACompiler> dfaCompiler;
        bool compiled;
    };

    template <class DFACompilerTp>
    std::ostream &operator<<(std::ostream &os, const Program<DFACompilerTp> &prog) {
        for (const auto &inst : prog.getInstructions()) {
            os << inst->str() << std::endl;
        }

        return os;
    }

    /**
     * Specialization of program that uses the full-match DFA compiler
     */
    using FullMatchProgram = Program<FullMatchDFACompiler>;

    /**
     * Specialization of program that uses the sub-match DFA compiler
     */
    using SubMatchProgram = Program<SubMatchDFACompiler>;

    /**
     * Specialization of program that uses the search DFA compiler
     */
    using SearchProgram = Program<SearchDFACompiler>;

    using CaptureProgram = Program<CaptureDFACompiler>;
}

#endif //AOTRC_PROGRAM_H
