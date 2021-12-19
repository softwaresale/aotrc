//
// Created by charlie on 12/19/21.
//

#ifndef _BUILD_PATTERN_FUNC_H
#define _BUILD_PATTERN_FUNC_H

#include <string>
#include <memory>
#include "compiler_context.h"

namespace aotrc::compiler {
    /**
     * Build a function that retrieves the pattern for a given function
     * @param label Label associated with the pattern
     * @param pattern The regex pattern
     * @param parent module to build this pattern function into
     * @param ctx compiler context
     * @return true if successful
     */
    bool build_pattern_func(const std::string &label, const std::string &pattern,
                            std::shared_ptr<llvm::Module> &parent, const std::shared_ptr<CompilerContext> &ctx);
}

#endif //_BUILD_PATTERN_FUNC_H
