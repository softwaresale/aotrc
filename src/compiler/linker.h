//
// Created by charlie on 3/26/22.
//

#ifndef AOTRC_LINKER_H
#define AOTRC_LINKER_H

#include <string>
#include <utility>
#include <vector>

namespace aotrc::compiler {

    /**
     * Responsible for linking shared and static libraries
     */
    class Linker {
    public:
        Linker(std::string ldPath, std::string arPath) : ldPath(std::move(ldPath)), arPath(std::move(arPath)) {}

        /**
         * Takes an object file and links a shared library.
         * @param objectFilePaths The path to the object file to link
         * @param outputPath The path to write the resulting library to
         * @return Exit code
         */
        int linkShared(const std::vector<std::string> &objectFilePaths, const std::string &outputPath);

        /**
         * Takes an object file and links a static library.
         * @param objectFilePaths The path to the object file to link
         * @param outputPath The path to write the resulting library to
         * @return Exit code
         */
        int linkStatic(const std::vector<std::string> &objectFilePaths, const std::string &outputPath);

    private:
        int executeProcess(std::vector<std::string> args);

        std::string ldPath;
        std::string arPath;
    };
}

#endif //AOTRC_LINKER_H
