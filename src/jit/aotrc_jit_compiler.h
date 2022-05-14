//
// Created by charlie on 5/13/22.
//

#ifndef AOTRC_AOTRC_JIT_COMPILER_H
#define AOTRC_AOTRC_JIT_COMPILER_H

#include <functional>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>

namespace aotrc::jit {
    class AotrcJITCompiler {
    public:
        AotrcJITCompiler();

        std::function<bool(const std::string &)> compileRegex(const std::string &label, const std::string &regex);

    private:
        llvm::orc::ThreadSafeContext ctx;
        std::unique_ptr<llvm::orc::LLJIT> jit;
    };
}

#endif //AOTRC_AOTRC_JIT_COMPILER_H
