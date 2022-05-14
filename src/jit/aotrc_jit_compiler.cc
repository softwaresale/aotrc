//
// Created by charlie on 5/13/22.
//

#include "aotrc_jit_compiler.h"
#include "src/parser/regex_parser.h"
#include "src/fa/dfa.h"
#include "src/compiler/full_match/full_match_program_info_provider.h"
#include "src/compiler/regex_compiler.h"

#include <llvm/ExecutionEngine/Orc/LLJIT.h>

aotrc::jit::AotrcJITCompiler::AotrcJITCompiler()
: ctx(std::make_unique<llvm::LLVMContext>()) {
    llvm::orc::LLJITBuilder builder;
    auto expectedJIT = builder.create();
    if (expectedJIT) {
        // Create a JIT instance
        this->jit = std::move(expectedJIT.get());
    } else {
        throw std::runtime_error("Could not initialize JIT engine");
    }
}

std::function<bool(const std::string &)> aotrc::jit::AotrcJITCompiler::compileRegex(const std::string &label, const std::string &regex) {
    auto &context = *this->ctx.getContext();
    // First, create a module
    auto functionModule = std::make_unique<llvm::Module>("jit_module", *this->ctx.getContext());

    // Parse the regex into a dfa
    auto dfa = aotrc::parser::parseRegexDFA(regex);

    // Compile the regex into the module
    aotrc::compiler::FullMatchProgramCompiler compiler(context);
    compiler.compile(functionModule, label, dfa);

    // Add the module to the JIT compiler
    if (auto err = this->jit->addIRModule(llvm::orc::ThreadSafeModule(std::move(functionModule), ctx))) {
        throw std::runtime_error("Could not add IR module: " + toString(std::move(err)));
    }

    // Get the function symbol
    auto functionSymbol = this->jit->lookup(compiler.getFunctionName(label));
    if (!functionSymbol) {
        auto err = functionSymbol.takeError();
        throw std::runtime_error("Could not find symbol: " + toString(std::move(err)));
    }

    // Convert it into a function pointer
    auto functionPointer = reinterpret_cast<bool(*)(const char *, unsigned long)>(functionSymbol->getAddress());
    // Turn it into a functional object
    std::function<bool(const std::string &)> functionPointerWrapper = [functionPointer](const std::string &str) -> bool {
        return functionPointer(str.c_str(), str.size());
    };

    return functionPointerWrapper;
}
