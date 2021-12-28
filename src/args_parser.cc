//
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
, skipShared(0) {
    struct option progOptions[] = {
            { "help", no_argument, &this->help, 1 },
            { "version", no_argument, &this->version, 1 },
            { "output-type", required_argument, nullptr, 0 },
            { "linker-path", optional_argument, nullptr, 0 },
            { "skip-shared", optional_argument, &this->skipShared, 1 },
            {nullptr, 0, nullptr, 0 }
    };

    int c;
    int longId = 0;
    while ((c = getopt_long(argc, argv, "hvt:S", progOptions, &longId)) != -1) {
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

            case 'S':
                this->skipShared = 1;
                break;

            case 't':
                if (optarg) {
                    std::string optArg(optarg);
                    if (optArg == "asm") {
                        this->outputType = OutputType::ASM;
                    } else if (optArg == "obj") {
                        this->outputType = OutputType::OBJ;
                    } else if (optArg == "ir") {
                        this->outputType = OutputType::IR;
                    }
                } else {
                    this->outputType = OutputType::OBJ;
                }
                break;

            case '?':
                if (std::string(progOptions[longId].name) == "linker-path") {
                    this->linkerPath = std::string(optarg);
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
    std::cout << "  --output-type, -t: output data type: asm, obj, ir. Default is obj" << std::endl;
    std::cout << "  --linker-path: specify a path to the linker to use for linking shared libraries" << std::endl;
    std::cout << "  --build-shared, -s: build a shared library, default is true" << std::endl;
    std::cout << "  --help, -h: display this help screen" << std::endl;
    std::cout << "  --version, -v: display information about this program" << std::endl;
    std::cout << std::endl;
    std::cout << "aotrc is maintained by Charlie Sale <softwaresale01@gmail.com>" << std::endl;
}
