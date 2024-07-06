#include <iostream>
#include "driver.hpp"

extern LLVMContext *context;
extern Module *module;
extern IRBuilder<> *builder;

int main (int argc, char *argv[]) {
  int res = 0;
  driver drv;
  int i = 1;
  while (i<argc) {
    if (argv[i] == std::string ("-p"))
      drv.trace_parsing = true; // Abilita tracce debug nel parser
    else if (argv[i] == std::string ("-s"))
      drv.trace_scanning = true;// Abilita tracce debug nello scanner
    else  if (!drv.parse(argv[i])) { // Parsing e creazione dell'AST
      drv.codegen();                 // Visita AST e generazione dell'IR (su stderr)
    } else
      res = 1;
    i++;
  };
  return res;
}
