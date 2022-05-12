
# AOTRC - Ahead of Time Regex Compiler

AOTRC (short for Ahead of Time Regex Compiler) compiles regular expressions into machine-executable
code. Aotrc uses llvm to compile regexes directly into machine code that executes on bare-metal.
This approach differs from the conventional approach, which is where regexes are compiled into a
program that runs within the programming language.

## Project Status
AOTRC is still in its infancy. As might be able to tell from this project's git history, I
started it in Dec. 2021. The project has undergone a number of rewrites, migrations, and more.
Some features so far include:
- Quite a bit of basic regex syntax, including literals, character classes, and quantifiers.
- Anchored and un-anchored matching. Anchored matching (or *full-matching*, as it's usually
referred to in this project) return true if the entire input string matches the pattern.
Un-anchored matching (or *sub-matching*) returns true if there is *some* submatch within the input
text. There is also a more useful version of sub-matching called *searching*, which actually describes
where in the subject string the match occurs.
- Support for C/C++

There is still quite a ways to go though.
- There is no capture group support
- And there is likely plenty more issues that I don't quite know about yet.
- No other languages are currently supported other than C/C++

In addition, I need something to substantiate any claims I make about this project. A good first
step is to start benchmarking this engine against other popular regex engines.

## Goals
I am motivated to develop AOTRC for a couple of benefits that might arise from this program:

### 1. Create a fast regex engine
One of my main goals with this project is to create a very, very fast regular expression engine.

First, aotrc eliminates the need to compile regexes during runtime. Typically, a regex is parsed
and converted into a matching program at runtime. These runtime regex representations take up
memory and take time to compile, especially with complicated regexes. With AOTRC, the process
of compiling a regex is done at compile time, not during runtime. So although it may still take
a while to compile a regex, that time does not affect runtime performance. Also, any memory
performance issues will not affect the runtime of the application.

Next, turning a regex directly into machine code will hopefully run faster on bare metal rather
than as a program inside another program. Although this is admittedly an intuition, I would
imagine that there are performance gains from running as a machine executable. In addition, it
might be easier to optimize a regex when it is machine code rather than running it as a program
within a program.

### 2. Improve generalizability of regular expressions across languages
Although regexes are theoretically similar, their implementations across different languages
vary. There are a variety of regex engines that are used in different languages. PCRE and RE2
are engines that can be used in different languages. There are also engines designed for
specific languages like the Rust Regex Crate and Python's Regex module. Although these engines
all implement regular expressions, they are all implemented slightly differently, which means
that the same regex might operate differently depending on the engine it is run in.

Enter AOTRC. AOTRC aims to be a single regex engine that runs consistently across all
languages. By compiling directly to machine code, AOTRC regexes are independent of the language
they are used in. In theory, they should perform the same across all languages.

### 3. Promote regular expression reuse
Another goal of this project is to make it easier to reuse regular expressions. With this tool,
regular expressions that are used commonly can be compiled into libraries and distributed like
other software. That way, rather than writing your own regular expression, you can use a regular
expression that is made and distributed by others.

### 4. Improve regex engineering process
A static regex compiler could potentially provide analysis for a regex that cannot be easily
determined during runtime. It could also potentially give other insights.

## Using AOTRC
1. Build and install aotrc on your system
2. Create a file of regex definitions (see "how it works" for more information)
```yaml
# defs.yml (or whatever else you want to call the file)
validators:
  email: /[^@]+@\w+.\w+/ # a naive email validator
```
3. Compile the regexes
```shell
$ ls
defs.yml

$ aotrc defs.yml
$ ls  # see compilation results
defs.yml validators.h validators.o 
```
4. Link your own program against the regexs
```c
// main.c
#include <stdio.h>
#include <string.h>
#include "validators.h"

int main(void) {
    const char *input = "email@test.com";
    unsigned int input_length = strlen(input);
    
    // <regex_name>_full_match is the match function generated for each regex
    // This function determines that the string completely matches
    int matches = email_full_match(input, input_length);
    if (matches) {
        printf("Matches!\n");
    } else {
        printf("Does not match\n");
    }
    
    const char *invalid_input = "email@";
    unsigned int invalid_input_length = strlen(invalid_input);
    matches = email_full_match(invalid_input, invalid_input_length);
    if (matches) {
        printf("Matches!\n");
    } else {
        printf("Does not match\n");
    }
    
    return 0;
}
```
```shell
$ ls
defs.yml validators.h validators.o main.c
$ gcc main.c validators.o -o test
$ ./test
Matches!
Does not match
```

## Caveats
This tool has a few shortcomings that might make it sub-optimal for your use-case.
1. **Regexes must be known ahead of time** - because regexes are compiled ahead of time, you
cannot build regexes at runtime.
<!-- TODO there are plenty more lol -->

## How It Works
AOTRC compiles regular expressions into machine-readable executables. To get there, a number of
steps have to be taken by the user and the compiler. To get a more in-depth explanation for the
actual architecture of the compiler, see the relevant wiki page (TODO: actually make the page).
The overview of how the compiler works is:

### Step 1: Define regexes to compile
First, the regexes to be compiled must be defined by the user in the regex definition file,
which is just a yaml file. The definition file consists of *module definitions*. A module is
a collection of regular expressions. All regexes defined in the same module are compiled into
the same object file. Each module declaration contains *regex definitions*. Each regex
definition defines a regex pattern as well as some options related to that specific regex.

Here is what a regex definition file might look like:
```yaml
# regex.yml
my_module: # name of the module
  # form:
  # label: /pattern/ -- note the "//" is necessary
  # or
  # label:
  #   pattern: /[a-z]+/
  #   options:
  #     option1: true
  regex_definition_form_1: /[a-z]+/
  regex_definition_form_2:
    pattern: /[a-z]+/
    options:
      fullmatch: true

other:
  regex1: /[a-z]+/
```

This module defines two modules: `my_module` and `other`. `my_module` contains the regexes
`regex_definition_form_1` and `regex_definition_form_2`. These two regexes have the same pattern:
`[a-z]+`. `regex_definition_form_2` has the fullmatch option on. Regex options will be discussed
later.

### Step 2: Regex => NFA => DFA
Now that the regexes to compile are defined by the user, the compiler starts its first task of
constructing Non-Deterministic Finite Automata (NFA) out of the defined regexes. Each regex
is parsed via ANTLR's PCRE grammar and then turned into an NFA via Thompson Construction.

Although NFAs can match patterns, they have some characteristics that make them not ideal for
compilation. The main drawback is that NFAs can have multiple outgoing transitions that can be
taken, so their behavior can be unpredictable. So, they are converted into Deterministic Finite
Automata via Powerset Construction (also known as subset construction).

Now that all regexes are turned into DFAs, they can begin the next step.

### Step 3: DFA => High-level Intermediate Representation (hir)
At this point in the compilation process, we have a DFA that can be used to match a string
against a pattern. Now, this DFA has to be converted into machine code. To make that possible,
AOTRC creates a function that represents the DFA. A DFA, however, is nothing more than a
transition graph, which is not really a "program." Enter High-level Intermediate Representation,
or hir.

HIR is an instruction set that describes the high level moves of the graph. Its purpose is to
take a graph and convert it into a linear sequence of instructions that symbolize the process
of simulating a DFA. The HIR for the regex `/abcd/` might look something like this:
```text
DECLARE counter: SIZE = 0        ;; These variables are used internally
DECLARE cursor: CHAR = 0         ;; 

START_STATE 0                    ;; Begin DFA state 0
CHECK_END:                       ;; See if we are at the end of the subject string
	REJECT                   ;; We are, then reject (this is not an accept state)
CONSUME                          ;; We are not at the end, so take another char from the input string
GOTO {TEST_EDGE {[a,a]}} 1       ;; See if the consumed char matches a. If it does, go to state 1
REJECT                           ;; There are no more edges to take, so reject

START_STATE 1
CHECK_END:
	REJECT
CONSUME
GOTO {TEST_EDGE {[b,b]}} 2       ;; Like above, but go to state 2
REJECT

START_STATE 2
CHECK_END:
	REJECT
CONSUME
GOTO {TEST_EDGE {[c,c]}} 3
REJECT

START_STATE 3
CHECK_END:
	REJECT
CONSUME
GOTO {TEST_EDGE {[d,d]}} 4
REJECT

START_STATE 4 **                 ;; Here we have an accept state
CHECK_END:                       ;; If we end on an accept state...
	ACCEPT                   ;; ...then accept...
REJECT                           ;; ...otherwise, reject
```
The HIR instructions represent things like starting new states, consuming characters from the subject string,
and moving across edges described in the graph. The HIR representation for each program can be different based
on the kind of program we are compiling (e.g. sub-matching, searching).

### Step 4: HIR => LLVM IR
Once we have a HIR program representation for a DFA, we can then translate/compile it into LLVM IR. In fact, the main
reason for having HIR in the first place is to simply the process of generating LLVM IR. LLVM IR is an intermediate
representation that consists of instructions that are closer to the metal. This IR is what actually gets compiled into
the target machine code.