
# AOTRC - Ahead of Time Regex Compiler

The goal of this project is to experiment with compiling regexes into machine code. Aotrc
uses llvm to compile regexes directly into machine code that executes on bare-metal. This
approach differs from the conventional approach, which is where regexes are compiled into a
program that runs within the programming language.