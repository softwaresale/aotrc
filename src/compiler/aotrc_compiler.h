//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_AOTRC_COMPILER_H
#define AOTRC_AOTRC_COMPILER_H

#include <string>
#include <optional>
#include <llvm/IR/Type.h>
#include <llvm/Support/CodeGen.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include "src/fa/dfa.h"

namespace aotrc::compiler {
    /**
     * The *big* compiler. Facade around all of the regex compiler types. Responsible for
     * - Compiling regex DFAs into the requested type
     * - Emitting the output of compiled modules
     * - Generating other sorts of code not related to LLVM
     */
    class AotrcCompiler {
    public:
        /**
         * Creates a new compiler instance for the host machine.
         */
        AotrcCompiler();

        /**
        * Builds a full match regex. This is the equivalent of a regex anchored on both sides
         *
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @return True if successful.
        */
        void compileRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA);

        /**
        * Builds a sub match regex. This is the equivalent of an un-anchored regex
         *
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @return True if successful.
        */
        void compileSubMatchRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA);

        /**
        * Builds a search regex. Finds a sub-match and returns its location within the subject
         *
        * @param module Name of module to place the regex in
        * @param label Name of the regex
        * @param regex The pattern for the regex
        * @return True if successful.
        */
        void compileSearchRegex(const std::string &module, const std::string &label, const aotrc::fa::DFA &regexDFA);

        /**
         * Writes the LLVM IR of a module
         * @param module The module to write
         * @return
         */
        std::string emitIr(const std::string &module);

        /**
         * Writes targeted assembly of a module
         * @param module The module to emit
         * @return The path of the file containing the output
         */
        std::string emitAssembly(const std::string &module);

        /**
         * Writes targeted assembly of a module to the desired output file
         * @param module Module to emit
         * @param outputPath The path to write the emission to
         * @return The path of the file containing the output
         */
        std::string emitAssembly(const std::string &module, const std::string &outputPath);

        /**
         * Writes object code of a module
         * @param module The module to emit
         * @return The path of the file containing the output
         */
        std::string emitObjectFile(const std::string &module);

        /**
         * Writes object code of a module to the desired output file
         * @param module Module to emit
         * @param outputPath The path to write the emission to
         * @return The path of the file containing the output
         */
        std::string emitObjectFile(const std::string &module, const std::string &outputPath);

        /**
         * Writes a C header file containing all of the function definitions
         * @param module The module to generate the header for
         * @return The path of the file containing the output
         */
        std::string emitHeaderFile(const std::string &module);

        /**
         * Writes a C header file containing all of the function definitions to the desired output file
         * @param module Module to emit
         * @param outputPath The path to write the emission to
         * @return The path of the file containing the output
         */
        std::string emitHeaderFile(const std::string &module, const std::string &outputPath);

        /**
         * Generates a function that will retrieve the pattern for a given regex
         * @param module The module to add to
         * @param label Regex's label
         * @param regex The pattern of the regex
         */
        void generatePatternFunc(const std::string &module, const std::string &label, const std::string &regex);

    private:
        /**
         * Helper function to write a given output type to a file
         * @param module The module to write
         * @param outputPath The desired output path
         * @param type The type of code to generate
         * @return The path to which the output was written to
         */
        std::string emitCode(const std::string &module, const std::string &outputPath, llvm::CodeGenFileType type);

        llvm::LLVMContext llvmContext;
        std::unordered_map <std::string, std::unique_ptr<llvm::Module>> modules;
        const llvm::Target *target;
        std::unique_ptr <llvm::TargetMachine> targetMachine;
    };
};


#endif //AOTRC_AOTRC_COMPILER_H
