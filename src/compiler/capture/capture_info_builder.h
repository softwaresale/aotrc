//
// Created by charlie on 8/21/22.
//

#ifndef AOTRC_CAPTURE_INFO_BUILDER_H
#define AOTRC_CAPTURE_INFO_BUILDER_H

#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>

namespace aotrc::compiler {

    /**
     * Builds the capture group info structure
     */
    class CaptureInfoBuilder {
    public:
        /**
         * Sets up info
         * @param ctx llvm context
         * @param regexName Name of the regex, gets turned into the struct name
         * @param groupNames Names for each of the groups
         */
        CaptureInfoBuilder(llvm::LLVMContext &ctx, const std::string &regexName, std::vector<std::string> groupNames);

        /**
         * Creates the struct type
         * @return
         */
        llvm::StructType *buildStruct() const;

        void writeCStructTemplate(std::ostream &os) const;

    private:
        llvm::LLVMContext &ctx;
        std::string name;
        std::vector<std::string> groupNames;
        std::vector<llvm::Type*> groupTypes;
    };

} // compiler

#endif //AOTRC_CAPTURE_INFO_BUILDER_H
