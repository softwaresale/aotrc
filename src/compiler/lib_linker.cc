//
// Created by charlie on 12/17/21.
//

#include "lib_linker.h"

#include <iostream>
#include <cstdlib>
#include <utility>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>

aotrc::compiler::LibLinker::LibLinker() {
    // Look for an environment variable
    const char *sysLinkerPath = std::getenv("LD_PATH");
    if (!sysLinkerPath) {
        this->linkerPath = std::string(sysLinkerPath);
    } else {
        this->linkerPath = "/usr/bin/ld"; // TODO this is probably a bad default...
    }

    const char *sysArchiverPath = std::getenv("AR_PATH");
    if (!sysArchiverPath) {
        this->archiverPath = std::string(sysArchiverPath);
    } else {
        this->archiverPath = "/usr/bin/ar"; // TODO this is probably a bad default...
    }
}

aotrc::compiler::LibLinker::LibLinker(std::string linkerPath, std::string archiverPath)
: linkerPath(std::move(linkerPath))
, archiverPath(std::move(archiverPath)) {
}

int aotrc::compiler::LibLinker::linkShared(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal) {

    if (this->linkerPath.empty()) {
        throw std::runtime_error("Must set linker path via command line argument or LD_PATH environment variable");
    }

    std::vector<const char *> args = {this->linkerPath.c_str(), "-shared", objectFilePath.c_str(), "-o", outputFileName.c_str(), nullptr};

    // Use the linker to link the original file into a shared lib
    pid_t childPid = fork();
    if (childPid == 0) {
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
        exit(0);
    } else {
        int status = 0, options = 0;
        waitpid(childPid, &status, options);
        if (status != 0) {
            throw std::runtime_error("Error while invoking linker");
        }
    }

    return 0;
}

int aotrc::compiler::LibLinker::linkStatic(const std::string &objectFilePath, const std::string &outputFileName,
                                           bool removeOriginal) {

    if (this->linkerPath.empty()) {
        throw std::runtime_error("Must set archiver path via command line argument or AR_PATH environment variable");
    }

    std::vector<const char *> args = {this->archiverPath.c_str(), "rcs", outputFileName.c_str(), objectFilePath.c_str(), nullptr};

    // Use the linker to link the original file into a shared lib
    pid_t childPid = fork();
    if (childPid == 0) {
        int ret = execvp(args[0], (char * const *) args.data());
        if (ret) {
            std::cerr << "Error while invoking archiver. errno: " << errno << std::endl;
        }

        // If the flag is set, then remove the original object file
        if (removeOriginal) {
            ret = remove(objectFilePath.c_str());
            if (ret) {
                std::cerr << "Error while deleting original file. Errno: " << errno << std::endl;
            }
        }
        exit(0);
    } else {
        int status = 0, options = 0;
        waitpid(childPid, &status, options);
        if (status != 0) {
            throw std::runtime_error("Error while invoking archiver");
        }
    }

    return 0;
}


