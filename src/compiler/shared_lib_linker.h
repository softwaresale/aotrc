//
// Created by charlie on 12/17/21.
//

#ifndef _SHARED_LIB_LINKER_H
#define _SHARED_LIB_LINKER_H

#include <string>

namespace aotrc::compiler {
    class SharedLibLinker {
    public:
        SharedLibLinker();
        explicit SharedLibLinker(const std::string &linkerPath);

        int link(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal = false);

    private:
        std::string linkerPath;
    };
}

#endif //_SHARED_LIB_LINKER_H
