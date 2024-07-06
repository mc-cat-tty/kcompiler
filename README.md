# Kaleidoscope Compiler

## Overview
TODO

## Setup
### Requirements
 - `clang` compiler toolchain
 - `LLVM 16` library (installable with `apt install llvm-16`, `brew install llvm@16`, etc.)

### Configuration
Replace `LLVM16_INCLUDE_PATH` variable, placed in `src/Makefile`, with the right path for your system.

Take a loook at `./Makefile`, `src/Makefile` and `test/Makefile` for further adjustments.

## Usage
```bash
make all
```
Running this command in the top-level directory will provide you `kcomp` compiler, able to translate Kaleidoscope sources into LLVM IR files.

```bash
make test ezfibo
```
Executes the test passed as argument.

If you wish to create new test, let assume named `foo`, keep into account that this test utility expects two files to be present in `test/` directory:
  - A Kaleidoscope source: `foo.k`
  - A C++ sources that provides at least the `main` function and wraps Kaleidoscope code to allow execution, input and output: `callfoo.cpp`