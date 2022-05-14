//
// Created by charlie on 5/14/22.
//

#include <gtest/gtest.h>
#include "src/jit/aotrc_jit_compiler.h"

TEST(JitCompiler, compile_full_match) {
    aotrc::jit::AotrcJITCompiler compiler;
    auto result = compiler.compileRegex("abc", "abc");
    bool matches = result("abc");
    ASSERT_TRUE(matches);
}

