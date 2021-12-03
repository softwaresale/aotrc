#include <iostream>

#include "fa/nfa.h"
#include "fa/nfa_builders.h"

int main() {
    auto abcFa = aotrc::fa::nfa_builders::literal("abc");
    auto defFa = aotrc::fa::nfa_builders::literal("def");

    auto star = aotrc::fa::nfa_builders::star(std::move(abcFa));

    std::cout << star << std::endl;

    return 0;
}
