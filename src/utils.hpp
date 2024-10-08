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

Value *logWarning(const std::string msg, const driver& drv) {
  std::cout
    << drv.file.c_str()
    << ":" << std::to_string(drv.location.begin.line)
    << ":" << std::to_string(drv.location.begin.column)
    << ": " << msg
    << std::endl;

  return nullptr;
}

// Semantic error
Value *logError(const std::string msg, const driver& drv) {
  std::cout << "Error: ";
  logWarning(msg, drv);
  exit(1);
}

static AllocaInst *CreateEntryBlockAlloca(Function *fun, StringRef varName, Type *varType = Type::getDoubleTy(*context)) {
  IRBuilder<> TmpB(&fun->getEntryBlock(), fun->getEntryBlock().begin());
  return TmpB.CreateAlloca(varType, nullptr, varName);
}

// Utility data type that models a symbol retrieved either from the global
// namespace or from the symbol table.
using Symbol = std::variant<GlobalVariable*, AllocaInst*>;
using MaybeSymbol = std::optional<Symbol>;

MaybeSymbol tryGetSymbol(driver &drv, const std::string &name) {
  // Notice that NamedValues is the locally-defined symbol table
  AllocaInst *A = drv.NamedValues[name];
  GlobalVariable *G = module->getNamedGlobal(name);

  if (A) return A;
  if (G) return G;

  logWarning("Variabile " + name + " non definita", drv);
  return std::nullopt;
}

Value* toInt(Value *v) {
  Value *floatVal = builder->CreateFPTrunc(v, Type::getFloatTy(*context));
  return builder->CreateFPToSI(floatVal, Type::getInt32Ty(*context));
}

Type *getSymbolType(const Symbol &s) {
  return std::holds_alternative<AllocaInst*>(s) ?
    std::get<AllocaInst*>(s)->getAllocatedType() :
    std::get<GlobalVariable*>(s)->getValueType();
}