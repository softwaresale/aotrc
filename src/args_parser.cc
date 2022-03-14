
// Created by charlie on 12/5/21.
//

#include "args_parser.h"

#include <iostream>
#include <getopt.h>

aotrc::ArgsParser::ArgsParser(int argc, char **argv)
: help(0)
, version(0)
, outputType(OutputType::OBJ)
, linkerPath("/usr/bin/ld")
, skipShared(1)
, arPath("/usr/bin/ar")
, skipArchive(1) {
    struct option progOptions[] = {
            { "help", no_argument, &this->help, 1 },
            { "version", no_argument, &this->version, 1 },
            { "output-type", required_argument, nullptr, 't' },
            { "linker-path", required_argument, nullptr, '0' },
            { "build-shared", no_argument, nullptr, 's' },
            { "archiver-path", required_argument, nullptr, '1' },
            { "build-archive", no_argument, nullptr, 'a' },
            { "render-graphviz", required_argument, nullptr, '2' },
            {nullptr, 0, nullptr, 0 }
    };

    int c;
    int longId = 0;
    while ((c = getopt_long(argc, argv, "hvt:sa", progOptions, &longId)) != -1) {
        switch (c) {
            case 0:
                // Do nothing
                break;

            case 'h':
                this->help = 1;
                break;

            case 'v':
                this->version = 1;
                break;

            case 's':
#if 0
                if (optarg != nullptr) {
                    std::string argStr(optarg);
                    // TODO make these case insensitive
                    if (argStr == "true") {
                        this->skipShared = 0;
                    } else if (argStr == "false") {
                        this->skipShared = 1;
                    } else {
                        throw std::invalid_argument("invalid argument for 'build-shared'");
                    }
                } else {
                    this->skipShared = 0;
                }
#endif
                this->skipShared = 0;
                break;

            case 'a':
#if 0
                if (optarg != nullptr) {
                    std::string argStr(optarg);
                    // TODO make these case insensitive
                    if (argStr == "true") {
                        this->skipArchive = 0;
                    } else if (argStr == "false") {
                        this->skipArchive = 1;
                    } else {
                        throw std::invalid_argument("invalid argument for 'build-static'");
                    }
                } else {
                    this->skipArchive = 0;
                }
#endif
                this->skipArchive = 0;
                break;

            case 't':
                if (optarg) {
                    std::string optArg(optarg);
                    if (optArg == "asm") {
                        this->outputType = OutputType::ASM;
                    } else if (optArg == "obj") {
                        this->outputType = OutputType::OBJ;
                    } else if (optArg == "hir") {
                        this->outputType = OutputType::IR;
                    }
                } else {
                    this->outputType = OutputType::OBJ;
                }
                break;

            case '0':
                this->linkerPath = std::string(optarg);
                break;

            case '1':
                this->arPath = std::string(optarg);
                break;

            case '2': {
                // optarg is a comma separated list
                std::string optstr(optarg);
                std::vector<std::string> listItems;
                while (true) {
                    auto nextComma = optstr.find(',');
                    if (nextComma == std::string::npos)
                        break;

                    listItems.push_back(optstr.substr(0, nextComma));
                    optstr.erase(optstr.begin(), optstr.begin() + nextComma + 1);
                }
                listItems.push_back(optstr);

                // Now that we have all the render types, we can convert them to enums and such
                for (const auto &item : listItems) {
                    if (item == "nfa") {
                        this->graphvizTypes.insert(GraphVizOutputTypes::NFA);
                    } else if (item == "dfa_full") {
                        this->graphvizTypes.insert(GraphVizOutputTypes::DFA_FULL);
                    } else if (item == "dfa_sub") {
                        this->graphvizTypes.insert(GraphVizOutputTypes::DFA_SUB);
                    }
                }
            }
                break;

            default:
                break;
        }
    }

    // Eat the remaining things
    int lastOpt = optind;
    while (lastOpt < argc) {
        this->inputFilePaths.emplace_back(argv[lastOpt++]);
    }
}

void aotrc::ArgsParser::displayHelp() const {
    std::cout << "usage: aotrc [options] regexes..." << std::endl;
    std::cout << "aotrc - ahead of time regex compiler used for compiling regexes into machine code" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "  --output-type,  -t : output data type: asm, obj, hir. Default is obj" << std::endl;
    std::cout << "  --linker-path      : specify a path to the linker to use for linking shared libraries" << std::endl;
    std::cout << "  --archiver-path    : specify a path to the archiver to use for creating a static library" << std::endl;
    std::cout << "  --build-shared, -s : build a shared library, default is false" << std::endl;
    std::cout << "  --build-static, -a : build a static library, default is false" << std::endl;
    std::cout << "  --render-graphviz  : output graphviz for the automata intermediate representation. Options are: nfa, dfa_full, dfa_sub" << std::endl;
    std::cout << "  --help,         -h : display this help screen" << std::endl;
    std::cout << "  --version,      -v : display information about this program" << std::endl;
    std::cout << std::endl;
    std::cout << "aotrc is maintained by Charlie Sale <softwaresale01@gmail.com>" << std::endl;
}
