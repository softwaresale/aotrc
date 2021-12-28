//
// Created by charlie on 12/17/21.
//

#ifndef _LIB_LINKER_H
#define _LIB_LINKER_H

#include <string>

namespace aotrc::compiler {
    class LibLinker {
    public:
        LibLinker();
        explicit LibLinker(std::string linkerPath, std::string archiverPath);

        int linkShared(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal = false);
        int linkStatic(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal = false);

    private:
        std::string linkerPath;
        std::string archiverPath;
    };
}

#endif //_LIB_LINKER_H
