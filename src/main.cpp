#include <iostream>

#include <getopt.h>

#include "parser/regex_parser.h"
#include "fa/nfa.h"
#include "compiler/compiler_context.h"
#include "compiler/match_function.h"
#include "args_parser.h"

int main(int argc, char **argv) {

    aotrc::ArgsParser parser(argc, argv);

    if (parser.hasHelp()) {
        parser.displayHelp();
        return 0;
    }

    return 0;
}
