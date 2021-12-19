//
// Created by charlie on 12/6/21.
//

#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include <memory>
#include <llvm/Target/TargetMachine.h>
#include "compiler_context.h"

namespace aotrc::compiler {
    class CodeGen {
    public:
        explicit CodeGen(const std::shared_ptr<CompilerContext> &ctx);

        bool compileModule(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath, llvm::CodeGenFileType outputType);
        void generateHeader(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath);

    private:
        std::shared_ptr<CompilerContext> ctx;
        std::unique_ptr<llvm::TargetMachine> machine;
    };
}

#endif //_CODE_GEN_H
