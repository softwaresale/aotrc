//
// Created by charlie on 12/6/21.
//

#ifndef _CODE_GEN_H
#define _CODE_GEN_H

#include <memory>
#include <llvm/Target/TargetMachine.h>

namespace aotrc::compiler {
    class CodeGen {
    public:
        CodeGen();

        bool compileModule(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath, llvm::CodeGenFileType outputType);
        void generateHeader(const std::shared_ptr<llvm::Module> &module, const std::string &outputPath);

    private:
        std::unique_ptr<llvm::TargetMachine> machine;
    };
}

#endif //_CODE_GEN_H
