//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_UTIL_H
#define AOTRC_UTIL_H

#include <string>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace aotrc::compiler {
    /**
     * Make the label for a basic block for the given state
     * @param stateId State id
     * @return label used for the corresponding label
     */
    std::string getStateBlockLabel(unsigned int stateId);

    /**
     * Stringify llvm types to C types
     * @param ctx llvm context
     * @param type The type to convert
     * @return c string form of the llvm type
     */
    std::string llvmTypeToCType(llvm::LLVMContext& ctx, llvm::Type *type);
}

#endif //AOTRC_UTIL_H
