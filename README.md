
# AOTRC - Ahead of Time Regex Compiler

AOTRC (short for Ahead of Time Regex Compiler) compiles regular expressions into machine-executable
code. Aotrc uses llvm to compile regexes directly into machine code that executes on bare-metal.
This approach differs from the conventional approach, which is where regexes are compiled into a
program that runs within the programming language.

## Project Status
AOTRC is still in its infancy. As might be able to tell from this project's git history, I
started it in Dec. 2021. Some features so far include:
- Quite a bit of basic regex syntax, including literals, character classes, and quantifiers.
- Anchored and (basic) unanchored matching. Anchored matching (or fullmatching, as it's usually
referred to in this project) return true if the entire input string matches the pattern.
Unanchored matching (or submatching) returns true if there is some submatch within the input
text.
- Support for C/C++

There is still quite a ways to go though.
- There is no capture group support
- There is little flexibility in how matching works. Submatching gives no information about
where the match is internally
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
    
    // fullmatch_<regex_name> is the match function generated for each regex
    int matches = fullmatch_email(input, input_length);
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
```

## Caveats
This tool has a few shortcomings that might make it sub-optimal for your use-case.
1. **Regexes must be known ahead of time** - because regexes are compiled ahead of time, you
cannot build regexes at runtime.
<!-- TODO there are plenty more lol -->

## How It Works
AOTRC compiles regular expressions into machine-readable executables. To get there, a number of
steps have to be taken by the user and the compiler. Here's how it works:

### Step 1: define regexes to compile
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

### Step 3: DFA => Match Function
At this point in the compilation process, we have a DFA that can be used to match a string
against a pattern. Now, this DFA has to be converted into machine code. To make that possible,
AOTRC creates a function that represents the DFA. Without getting too much into the details,
the match function represents DFA states as basic blocks. As the function steps through the
input text, the program switches to different states based on the encountered character. When
the program ends on an accept state, it returns true. Otherwise, it returns false.

Once a match function is created from a DFA, it can be compiled into machine code. This match
function is first built into LLVM Intermediate Representation (IR), and is then compiled into
actual machine code for the current target.

At this point, we finally have machine code! This machine code can be linked into another
executable or loaded dynamically into a program.
