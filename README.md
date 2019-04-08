# SystemC Simulation Engine

This directory contains a presentation (pdf) and source code used
to create the presentation.

In order to recreate the simulation used to create the presentation,
the following are required:

1. CMake
2. A compliant C++ compiler
3. An fully installed version of SystemC at least version 2.3.2 that supports CMake
4. Environment variable SYSTEMC pointing to the SystemC installation directory (NOT the source).

Under Linux/OS X, the commands would be:

```bash
    mkdir build && cd build
    cmake ..
    make run
```


