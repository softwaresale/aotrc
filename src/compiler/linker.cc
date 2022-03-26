//
// Created by charlie on 3/26/22.
//

#include "linker.h"

#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>

int aotrc::compiler::Linker::linkShared(const std::vector<std::string> &objectFilePaths, const std::string &outputPath) {
    // Create the vector of arguments to pass to the linker
    std::vector<std::string> args = { this->ldPath, "-shared", "-o", outputPath };
    std::copy(objectFilePaths.cbegin(), objectFilePaths.cend(), std::back_inserter(args));

    return this->executeProcess(std::move(args));
}

int aotrc::compiler::Linker::linkStatic(const std::vector<std::string> &objectFilePaths, const std::string &outputPath) {
    // Create the vector of arguments to pass to the linker
    std::vector<std::string> args = { this->arPath, "-crs", outputPath };
    std::copy(objectFilePaths.cbegin(), objectFilePaths.cend(), std::back_inserter(args));

    return this->executeProcess(std::move(args));
}

int aotrc::compiler::Linker::executeProcess(std::vector<std::string> args) {
    // Execute
    pid_t child = fork();
    if (child == 0) {
        // Execute the process
        std::vector<const char *> rawArgs;
        std::transform(args.cbegin(), args.cend(), std::back_inserter(rawArgs),
                       [](const std::string &str) { return str.c_str(); });
        rawArgs.push_back(nullptr);
        char * const *rawArgsData = (char * const *) rawArgs.data();
        int ret = execv(args[0].c_str(), rawArgsData);
        if (ret) {
            perror("execv");
        }
        exit(ret);
    } else {
        int status = 0;
        waitpid(child, &status, 0);
        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}
