//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_REGEX_COMPILER_H
#define AOTRC_REGEX_COMPILER_H

#include <iostream>
#include <string>
#include <utility>
#include <llvm/IR/LLVMContext.h>
#include "program.h"
#include "program_translator.h"
#include "program_info_provider.h"
#include "src/compiler/full_match/full_match_program_info_provider.h"
#include "src/compiler/sub_match/sub_match_program_info_provider.h"

namespace aotrc::compiler {

    /**
     * Handles end-to-end compilation of a regex DFA into LLVM IR
     * @tparam ProgramTranslatorTp The program translator type to use
     * @tparam ProgramTp The type of program being compiled
     * @tparam ProgramInfoProviderTp The type that provides program info
     */
    template <class ProgramTranslatorTp, class ProgramTp, class ProgramInfoProviderTp>
    class RegexCompiler {
    public:
        explicit RegexCompiler(llvm::LLVMContext &ctx)
        : ctx(ctx)
        {
            // First, verify that translator is the correct type
            static_assert(
                    std::is_base_of_v<
                            ProgramTranslator<typename ProgramTranslatorTp::InstructionTranslatorType, typename ProgramTranslatorTp::ProgramType>,
                            ProgramTranslatorTp
                            >
                    );
            // Next, verify that ProgramTp is valid, and that the provided program type lines up with the
            // program translator program type
            static_assert(
                    std::is_base_of_v<Program<typename ProgramTp::DFACompilerType>, ProgramTp> &&
                    std::is_same_v<typename ProgramTranslatorTp::ProgramType, ProgramTp>
                    );
            // Finally, verify that ProgramInfoProvider is the right type
            static_assert(
                    std::is_base_of_v<ProgramInfoProvider, ProgramInfoProviderTp>
                    );

            // Templating is correct, so we're good
            progInfoProvider = std::make_unique<ProgramInfoProviderTp>();
        }

        /**
         * Compiles a regular expression represented by a DFA
         * @param label The name/label of the regex
         * @param regexDFA The DFA ty compile
         */
        void compile(std::unique_ptr<llvm::Module> &module, const std::string &label, const fa::DFA &regexDFA) {
            // First, create the function
            auto functionName = this->progInfoProvider->getFunctionName(label);
            auto functionType = this->progInfoProvider->getFunctionType(ctx);
            module->getOrInsertFunction(functionName, functionType);
            auto function = module->getFunction(functionName);

            // Next, create the program
            ProgramTp program;
            program.compile(regexDFA);

            std::cout << program << std::endl;

            // Now that the program is compiled, we need to translate it into LLVM IR
            llvm::IRBuilder<> builder(ctx);
            ProgramTranslatorTp programTranslator(this->ctx, builder, function);
            programTranslator.translate(program);

            // We have compiled a program and translated it into LLVM IR
        }

    private:
        llvm::LLVMContext &ctx;
        std::unique_ptr<ProgramInfoProvider> progInfoProvider;

        /**
         * Gets the desired function type for the given program
         * @param label Name/label of the regex
         * @return The name of the function
         */
        std::string getFunctionName(const std::string &label) const {
            if constexpr(std::is_same_v<ProgramTp, FullMatchProgram>) {
                return label + "_full_match";
            } else {
                return label + "_unknown";
            }
        }
    };

    /**
     * A full match program compiler
     */
    using FullMatchProgramCompiler = RegexCompiler<FullMatchProgramTranslator, FullMatchProgram, FullMatchProgramInfoProvider>;

    /**
     * A specialized sub-match program compiler. Compiles a regex into a sub-match program
     */
    using SubMatchProgramCompiler = RegexCompiler<SubMatchProgramTranslator, SubMatchProgram, SubMatchProgramInfoProvider>;
}

#endif //AOTRC_REGEX_COMPILER_H
