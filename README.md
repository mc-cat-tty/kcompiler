# Kaleidoscope Compiler

## Overview
Implementation of a Kaleidoscope compiler front-end. Grammars from L1 to L4 have been implemented:
 0. The base project has been provided by the course Lecturers. The following steps are built on top of that base
 1. Global variables and statements are introduced
 2. If and for expressions are introduced
 3. Greater than operator introduced, as well ass boolean expressions
 4. Arrays - both global and local - are introduced, with all the surrounding blocks: subscript operator for assignments and expressions

In addition to these features, also inline comments are allowed with the following syntax: `.... # comment`.

## Setup
### Requirements
 - `bison` compiler-compiler
 - `flex` lexer generator
 - `M4` macro expander
 - `clang` compiler toolchain
 - `LLVM 16` library (installable with `apt install llvm-16`, `brew install llvm@16`, etc.)

### Configuration
Replace `LLVM16_INCLUDE_PATH` variable, placed in `src/Makefile`, with the right path for your system.

## Usage
```bash
make all
```
Running this command in the top-level directory will provide you `kcomp` compiler, able to translate Kaleidoscope sources into LLVM IR files.

## Intermediate Test
> Partial test are tests used by me during the development of this project as partial steps towards the final version.

```bash
make intermediate_test ezfibo
```
Executes the test passed as argument.

If you wish to create new test, let assume named `foo`, keep into account that this test utility expects two files to be present in `test/` directory:
  - A Kaleidoscope source: `foo.k`
  - A C++ sources that provides at least the `int main(...)` function and wraps Kaleidoscope code to allow execution, input and output: `callfoo.cpp`

## Test
> Tests placed in the `test/` folder are used for the final examination; they constitute the working set of Kaleidoscope programs to deem grammar - from L1 to L4 - properly implemented.


The can be run as above through a `make` target:
```bash
make teset insort
```