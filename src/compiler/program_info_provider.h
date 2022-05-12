//
// Created by charlie on 5/11/22.
//

#ifndef AOTRC_PROGRAM_INFO_PROVIDER_H
#define AOTRC_PROGRAM_INFO_PROVIDER_H

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/DerivedTypes.h>

namespace aotrc::compiler {
    /**
     * Provides meta data for a given program. This is used by the regex compiler to
     * create the function prototype (name and function type).
     */
    struct ProgramInfoProvider {
        /**
         * Builds out the desired function type
         * @param ctx llvm context
         * @return The function type to create the function with
         */
        virtual llvm::FunctionType *getFunctionType(llvm::LLVMContext &ctx) const = 0;

        /**
         * Gets the name of the function based on the name of the regex
         * @param regexLabel Name of the regex to be compiled
         * @return The name of the function to create
         */
        virtual std::string getFunctionName(const std::string &regexLabel) const noexcept = 0;
    };
}

#endif //AOTRC_PROGRAM_INFO_PROVIDER_H
