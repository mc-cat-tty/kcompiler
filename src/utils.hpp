#include <iostream>
#include <variant>
#include <optional>

#include "driver.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/GlobalVariable.h"

extern LLVMContext *context;
extern Module *module;
extern IRBuilder<> *builder;


Value *LogErrorV(const std::string Str) {
  std::cerr << Str << std::endl;
  return nullptr;
}

static AllocaInst *CreateEntryBlockAlloca(Function *fun, StringRef VarName) {
  IRBuilder<> TmpB(&fun->getEntryBlock(), fun->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::getDoubleTy(*context), nullptr, VarName);
}

// Utility data type that models a symbol retrieved either from the global
// namespace or from the symbol table.
using MaybeSymbol = std::optional<std::variant<GlobalVariable*, AllocaInst*>>;

MaybeSymbol tryGetSymbol(driver &drv, const std::string &name) {
  AllocaInst *A = drv.NamedValues[name];
  GlobalVariable *G = module->getNamedGlobal(name);

  if (!A and !G) {
    LogErrorV("Variabile " + name + " non definita");
    return std::nullopt;
  }

  if (A) return A;
  if (G) return G;
}