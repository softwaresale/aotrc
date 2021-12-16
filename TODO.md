
# TODO - Things to do for this project
In no particular order:
- [ ] **Full CLI app** - Need to get a nice cli app going for testing and usage
- [ ] **Regex file format** - create a custom file format type defines modules and regexes. With
kind of file format, we can specify multiple modules, multiple regexes, and options for both.
- [ ] **Multiple compilation types** - Right now, aotrc only does full-match matching. We should
also add support for sub-matching, which is if a string contains a substring matched by this regex.
In order to make this happen, we will likely need to use NFAs.
- [ ] **Capture group support** - right now, aotrc only does matching. It would be cool to add
optional capture group support
- [ ] **Language bindings/Usage options** - Right now, we generate a raw object file to be
compiled into an existing project. To use this is other projects, we are going to need bindings
of sorts. A few good places to start would be C/C++ and Python.
- [ ] **Project improvements** - this project needs documentation and testing. I'm not quite sure
what unit tests will look like, but we'll figure it out.