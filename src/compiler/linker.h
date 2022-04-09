//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_LINKER_H
#define AOTRC_LINKER_H

#include <string>
#include <vector>

namespace aotrc::compiler {

    /**
     * Responsible for linking shared and static libraries
     */
    class Linker {
    public:
        Linker(const std::string &ldPath, const std::string &arPath) : ldPath(ldPath), arPath(arPath) {}

        int linkShared(const std::vector<std::string> &objectFilePaths, const std::string &outputPath);
        int linkStatic(const std::vector<std::string> &objectFilePaths, const std::string &outputPath);

    private:
        int executeProcess(std::vector<std::string> args);

        std::string ldPath;
        std::string arPath;
    };
}

#endif //AOTRC_LINKER_H
