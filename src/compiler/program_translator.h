//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_PROGRAM_TRANSLATOR_H
#define AOTRC_PROGRAM_TRANSLATOR_H

#include <memory>
#include "instruction_translator.h"
#include "src/compiler/full_match/full_match_translator.h"
#include "src/compiler/full_match/full_match_dfa_compiler.h"

namespace aotrc::compiler {
    /**
     * Responsible for translating a high-level program into LLVM IR
     * @tparam TranslatorTp Type of translator to use to translate high level instructions
     * @tparam DFACompilerTp The type of program to compile
     */
    template <class TranslatorTp, class ProgramTp>
    class ProgramTranslator {
    public:
        /**
         * The type of instruction translator being used
         */
        using InstructionTranslatorType = TranslatorTp;

        /**
         * The type of program being used
         */
        using ProgramType = ProgramTp;

        ProgramTranslator(llvm::LLVMContext &ctx, llvm::IRBuilder<> &builder, llvm::Function *function) {
            // First, check that we can use the given translator
            static_assert(
                    std::is_base_of_v<InstructionTranslator, TranslatorTp> &&
                    std::is_constructible_v<TranslatorTp, llvm::LLVMContext &, llvm::IRBuilder<> &, llvm::Function*>
                    );
            // Next, check that the program type is valid
            static_assert(
                    std::is_base_of_v<Program<typename ProgramTp::DFACompilerType>, ProgramTp>
                    );

            // Next, make the translator
            translator = std::make_unique<TranslatorTp>(ctx, builder, function);
        }

        /**
         * Translates a program using the provided translator
         * @param program
         */
        void translate(const ProgramTp &program) {
            // First, build out the entry block
            const auto &instructions = program.getInstructions();
            auto instIter = instructions.cbegin();
            std::vector<std::reference_wrapper<const InstructionPtr>> setupInstructions;
            while (instIter->get()->type() != InstructionType::START_STATE) {
                setupInstructions.push_back(std::cref(*instIter));
                ++instIter;
            }
            this->translator->makeEntryBlock(std::move(setupInstructions));

            // Next, make the accept and reject blocks
            this->translator->makeAcceptBlock();
            this->translator->makeRejectBlock();

            // Next, we want to execute all start_state instructions to load the symbol table with all the
            // necessary state basic blocks
            auto backupInstIter = instIter;
            for (; instIter != instructions.cend(); ++instIter) {
                if (instIter->get()->type() == InstructionType::START_STATE) {
                    // Build the start state
                    this->translator.get()->makeStartStateInst(*instIter);
                }
            }

            // Link the entry block to the first state block
            translator->linkFirstStateToEntry();

            // Now that all state blocks exist, we can resume from where we backed up and we can translate
            // all the instructions. There will hopefully be no dangling references :)
            instIter = backupInstIter;
            for (; instIter != instructions.end(); ++instIter) {
                this->translator->makeInstruction(*instIter);
            }

            // At this point, we should be done
        }

    private:
        std::unique_ptr<InstructionTranslator> translator;
    };

    /**
     * A specialized FullMatchProgramTranslator
     */
    using FullMatchProgramTranslator = ProgramTranslator<FullMatchTranslator, FullMatchProgram>;

    /**
     * A specialized submatch program translator. This specific instance still uses the full match translator
     * as there are no differences in translation. However, the type of program (and therefore DFA compiler)
     * is different.
     */
    using SubMatchProgramTranslator = ProgramTranslator<FullMatchTranslator, SubMatchProgram>;
}

#endif //AOTRC_PROGRAM_TRANSLATOR_H
