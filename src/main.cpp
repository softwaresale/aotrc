#include <llvm/Support/InitLLVM.h>

#include "args_parser.h"
#include "input/aotrc_input_parser.h"
#include "compiler/program.h"
#include "compiler/compiler.h"

#if 0
static std::string getOutputFileName(const std::string &moduleName, aotrc::OutputType outputType) {
    std::string filename = moduleName + '.';
    switch (outputType) {
        case aotrc::OutputType::ASM:
            filename += 's';
            break;
        case aotrc::OutputType::OBJ:
            filename += 'o';
            break;
        case aotrc::OutputType::IR:
            filename += "ir";
            break;
        default:
            break;
    }

    return filename;
}
#endif

int main(int argc, char **argv) {
    llvm::InitLLVM initLLVM(argc, argv);
    aotrc::ArgsParser argsParser(argc, argv);

    if (argsParser.hasHelp()) {
        argsParser.displayHelp();
        return 0;
    }

    aotrc::input::AotrcInputParser inputFileParser(argsParser.getInputFilePaths()[0]);
    aotrc::compiler::Compiler compiler;

    // just take the first regex def
    auto moduleName = inputFileParser.getModules().begin()->first;
    auto firstRegexDef = inputFileParser.getModules().begin()->second[0];
    compiler.compileRegex(moduleName, firstRegexDef.label, firstRegexDef.pattern);

    compiler.emitObjectFile(moduleName);
    compiler.emitHeaderFile(moduleName);

    return 0;
}
