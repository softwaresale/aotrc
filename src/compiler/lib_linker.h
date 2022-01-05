//
// Created by charlie on 12/17/21.
//

#ifndef _LIB_LINKER_H
#define _LIB_LINKER_H

#include <string>

namespace aotrc::compiler {
    /**
     * Class that links object files into libraries. This class is not a linker. Instead, it uses system binaries to
     * handle linking.
     */
    class LibLinker {
    public:
        LibLinker();
        explicit LibLinker(std::string linkerPath, std::string archiverPath);

        /**
         * Link a shared library from a single object file
         * @param objectFilePath Path to object file
         * @param outputFileName Path to the output shared library file
         * @param removeOriginal If true, delete the original object file
         * @return 0 if success, otherwise failure
         */
        int linkShared(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal = false);

        /**
         * Create a static library from a single object file
         * @param objectFilePath Path to object file
         * @param outputFileName Path to the output static library file
         * @param removeOriginal If true, delete the original object file
         * @return 0 if success, otherwise failure
         */
        int linkStatic(const std::string &objectFilePath, const std::string &outputFileName, bool removeOriginal = false);

    private:
        std::string linkerPath;
        std::string archiverPath;
    };
}

#endif //_LIB_LINKER_H
