//
// Created by charlie on 12/17/21.
//

#include "shared_lib_linker.h"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <unistd.h>

aotrc::compiler::SharedLibLinker::SharedLibLinker() {
    // Look for an environment variable
    const char *sysLinkerPath = std::getenv("LD_PATH");
    if (!sysLinkerPath) {
        this->linkerPath = std::string(sysLinkerPath);
    }
}

aotrc::compiler::SharedLibLinker::SharedLibLinker(const std::string &linkerPath)
: linkerPath(linkerPath) {
}

int aotrc::compiler::SharedLibLinker::link(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal) {

    std::vector<const char *> args = {this->linkerPath.c_str(), "-shared", objectFilePath.c_str(), "-o", outputFileName.c_str(), nullptr};

    // Use the linker to link the original file into a shared lib
    int ret = execvp(args[0], (char * const *) args.data());
    if (ret) {
        std::cerr << "Error while invoking linker. errno: " << errno << std::endl;
    }

    // If the flag is set, then remove the original object file
    if (removeOriginal) {
        ret = remove(objectFilePath.c_str());
        if (ret) {
            std::cerr << "Error while deleting original file. Errno: " << errno << std::endl;
        }
    }

    return 0;
}


