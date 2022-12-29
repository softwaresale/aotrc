//
// Created by charlie on 5/14/22.
//

#include <gtest/gtest.h>
#include "src/jit/aotrc_jit_compiler.h"

TEST(JitCompiler, CompileFullMatch) {
    aotrc::jit::AotrcJITCompiler compiler;
    auto result = compiler.compileRegex("abc", "abc");
    bool matches = result("abc");
    ASSERT_TRUE(matches);

    bool doesNotMatch = result("def");
    ASSERT_FALSE(doesNotMatch);
}

TEST(JitCompiler, CompileCharacterClass) {
    aotrc::jit::AotrcJITCompiler compiler;
    auto result = compiler.compileRegex("abc", "[a-z]+");
    bool matches = result("hello");
    ASSERT_TRUE(matches);

    bool doesNotMatch = result("HELLO_WORLD");
    ASSERT_FALSE(doesNotMatch);
}
