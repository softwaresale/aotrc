//
// Created by charlie on 12/5/21.
//

#include "args_parser.h"

#include <iostream>
#include <getopt.h>
#include <iomanip>

aotrc::ArgsParser::ArgsParser(int argc, char **argv)
: version(0)
, help(0) {
    struct option progOptions[] = {
            { "help", no_argument, &this->help, 'h' },
            { "version", no_argument, &this->version, 'v' },
            {nullptr, 0, nullptr, 0 }
    };

    int c;
    int optionIndex = 1;
    while ((c = getopt_long(argc, argv, "hv", progOptions, &optionIndex)) != -1) {
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

            default:
                break;
        }
    }

    // Eat the remaining things
    while (optionIndex < argc) {
        this->remainingData.emplace_back(argv[optionIndex++]);
    }
}

void aotrc::ArgsParser::displayHelp() const {
    std::cout << "usage: aotrc [options] regexes..." << std::endl;
    std::cout << "aotrc - ahead of time regex compiler used for compiling regexes into machine code" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "  --help, -h: display this help screen" << std::endl;
    std::cout << "  --version, -v: display information about this program" << std::endl;
    std::cout << std::endl;
    std::cout << "aotrc is maintained by Charlie Sale <softwaresale01@gmail.com>" << std::endl;
}
