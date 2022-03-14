
# Roadmap - Larger Milestones
In no particular order:
- [ ] **Pattern fetch function** - The pattern is known at compile time, so create
a function type for fetching the pattern of a regex.
- [ ] **Test Suite** - A lot of things need correctness verification. Some things
that need to be tested include:
  - [ ] *(D|N)FA correctness* - Do the generated fas behave as they are expected to?
  - [ ] *Integration tests* - Do the compiled regexes behave correctly when compiled
  against a "real" program.
### Here - Release 0.1.0
- [ ] **Program transformation/Multiple Program Types** - Once we know that
everything is working properly, we should be able to create different kinds of
programs for different tasks. Some kinds of programs include:
  - [ ] *Sub-matching* - Rather than matching the entire string, just see if
  there's at least one substring that matches
  - [ ] *Capturing* - Add support for capture groups. This will involve keeping
  track of where capture groups begin and end in the subject string. (For starters,
  just return indices of where the capture happens. Down the line tho, it would
  be interesting to copy the capture group into a buffer).
  - [ ] *(Optional) Lookaround/Other high level features* - DFAs do not support
  look-ahead assertions or any kind of regex features that involve memory. However,
  these regex programs *have* memory. So, any regex features that require memory can
  be added.
### Here - Release 0.2.0
- [ ] **Benchmarking** - Compare the speed of this regex engine to others.
- [ ] **Cross Compilation** - LLVM Supports cross compilation, so that should be
added to this project as well.
- [ ] **Bindings** - Compilation just emits object files. This is great for C/C++,
but not excellent for other languages. The goal for this project is to be
accessible for all (major) programming languages. So, this project should be
compatible with those. Here are some languages to support:
  - [ ] *C/C++ (Dynamic)* - Static C/C++ support already exists because the
  functions can be compiled into a program. This is great when the same regex
  is called every time. However, the programmer may want to dynamically decide
  which regex should be run. So, there should be dynamic bindings that
  dynamically call symbols from a shared library.
  - [ ] *Python (Dynamic)* - A binding to python might be a good option. This
  will also involve dynamically calling symbols from a shared library.
  - [ ] *Rust (Static/Dynamic)* - Like C, a dynamic rust binding would be
  useful. However, it might also be nice for aotrc regexes to be compiled into
  a rust program.
  - [ ] *NodeJS (Dynamic)* - Aotrc in Node?
### Here - Release 1.0.0