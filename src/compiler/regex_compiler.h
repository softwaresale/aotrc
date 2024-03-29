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
#include "src/compiler/searching/search_program_info_provider.h"

namespace aotrc::compiler {

    /**
     * Handles end-to-end compilation of a regex DFA into LLVM IR. The regex compiler is responsible for
     * - creating the LLVM function
     * - Converting a DFA to the corresponding Program
     * - Translating the Program into LLVM IR
     *
     * @tparam ProgramTranslatorTp The program translator type to use
     * @tparam ProgramTp The type of program being compiled. Must be accepted by ProgramTranslatorTp
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
                    std::is_same_v<typename ProgramTranslatorTp::ProgramType, ProgramTp> &&
                    std::is_constructible_v<ProgramTp, llvm::LLVMContext&>
                    );
            // Finally, verify that ProgramInfoProvider is the right type
            static_assert(
                    std::is_base_of_v<ProgramInfoProvider, ProgramInfoProviderTp>
                    );

            // Templating is correct, so we're good
            progInfoProvider = std::make_unique<ProgramInfoProviderTp>();
        }

        /**
         * Compiles a regular regular expression DFA into LLVM IR
         * @param module The LLVM module to add the created LLVM function to
         * @param label The name/label of the regex
         * @param regexDFA The DFA to compile
         */
        void compile(std::unique_ptr<llvm::Module> &module, const std::string &label, const fa::DFA &regexDFA) {
            // First, create the function
            auto functionName = this->progInfoProvider->getFunctionName(label);
            auto functionType = this->progInfoProvider->getFunctionType(ctx);
            module->getOrInsertFunction(functionName, functionType);
            auto function = module->getFunction(functionName);

            // Next, create the program
            ProgramTp program(ctx);
            program.compile(regexDFA);

            // Now that the program is compiled, we need to translate it into LLVM IR
            llvm::IRBuilder<> builder(ctx);
            ProgramTranslatorTp programTranslator(this->ctx, builder, function);
            programTranslator.translate(program);

            // We have compiled a program and translated it into LLVM IR
        }

    private:
        llvm::LLVMContext &ctx;
        std::unique_ptr<ProgramInfoProvider> progInfoProvider;
    };

    /**
     * A full match program compiler
     */
    using FullMatchProgramCompiler = RegexCompiler<FullMatchProgramTranslator, FullMatchProgram, FullMatchProgramInfoProvider>;

    /**
     * A specialized sub-match program compiler. Compiles a regex into a sub-match program
     */
    using SubMatchProgramCompiler = RegexCompiler<SubMatchProgramTranslator, SubMatchProgram, SubMatchProgramInfoProvider>;

    /**
     * A specialised search program compiler. Compiles a regex into a searching program
     */
    using SearchProgramCompiler = RegexCompiler<SearchProgramTranslator, SearchProgram , SearchProgramInfoProvider>;
}

#endif //AOTRC_REGEX_COMPILER_H
