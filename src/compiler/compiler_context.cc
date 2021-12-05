//
// Created by charlie on 12/4/21.
//

#include "compiler_context.h"

aotrc::compiler::CompilerContext::CompilerContext()
: llvmContext()
, irBuilder(this->llvmContext) {
}

std::shared_ptr<llvm::Module> aotrc::compiler::CompilerContext::addModule(const std::string &name) {
    if (this->modules.find(name) == this->modules.end()) {
        auto newModule = std::make_shared<llvm::Module>(name, this->llvmContext);
        this->modules[name] = std::move(newModule);
    }

    return this->modules[name];
}

std::shared_ptr<aotrc::compiler::CompilerContext> aotrc::compiler::CompilerContext::instance() {
    static std::shared_ptr<CompilerContext> singleton = std::make_shared<CompilerContext>();
    return singleton;
}


