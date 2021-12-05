//
// Created by charlie on 12/4/21.
//

#ifndef _COMPILER_CONTEXT_H
#define _COMPILER_CONTEXT_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>

namespace aotrc::compiler {
    class CompilerContext {
    public:
        static std::shared_ptr<CompilerContext> instance();

        explicit CompilerContext();

        /**
         * Creates or gets a module by name
         * @param name name of the module
         * @return a new module by name, or the existing one if it already exists
         */
        std::shared_ptr<llvm::Module> addModule(const std::string &name);

        llvm::LLVMContext &context() {
            return llvmContext;
        }
        llvm::IRBuilder<> &builder() {
            return irBuilder;
        }
        const std::unordered_map<std::string, std::shared_ptr<llvm::Module>> &getModules() const {
            return modules;
        }

    private:
        llvm::LLVMContext llvmContext;
        llvm::IRBuilder<> irBuilder;
        std::unordered_map<std::string, std::shared_ptr<llvm::Module>> modules;
    };
}

#endif //_COMPILER_CONTEXT_H
