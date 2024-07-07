#ifndef DRIVER_HPP
#define DRIVER_HPP

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

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <vector>
#include <variant>

#include "parser.hpp"

# define YY_DECL yy::parser::symbol_type yylex (driver& drv)
YY_DECL;

using namespace llvm;

class driver {
public:
  std::map<std::string, AllocaInst*> NamedValues;  // Symbol table
  RootAST* root;  // AST root
  std::string file;  // Input file
  bool trace_parsing;  // Parser debug tracing
  bool trace_scanning;  // Scanner debug tracing
  yy::location location;  //  Tokens' location

  driver();
  void scan_begin ();  // See scanner.ll
  void scan_end ();  // See scanner.ll
  int parse (const std::string& f);
  void codegen();
};

typedef std::variant<std::string,double> lexval;
const lexval NONE = 0.0;

class RootAST {
public:
  virtual ~RootAST() {};
  virtual lexval getLexVal() const { return NONE; };
  virtual Value *codegen(driver& drv) { return nullptr; };
};

class SeqAST : public RootAST {
private:
  RootAST* first;
  RootAST* continuation;

public:
  SeqAST(RootAST* first, RootAST* continuation);
  Value *codegen(driver& drv) override;
};


class ExprAST : public RootAST {};

class NumberExprAST : public ExprAST {
private:
  double Val;

public:
  NumberExprAST(double Val);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

class VariableExprAST : public ExprAST {
private:
  std::string Name;
  
public:
  VariableExprAST(const std::string &Name);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

class BinaryExprAST : public ExprAST {
private:
  char Op;
  ExprAST* LHS;
  ExprAST* RHS;

public:
  BinaryExprAST(char Op, ExprAST* LHS, ExprAST* RHS);
  Value *codegen(driver& drv) override;
};

class CallExprAST : public ExprAST {
private:
  std::string Callee;
  std::vector<ExprAST*> Args;  // Args sub-AST

public:
  CallExprAST(std::string Callee, std::vector<ExprAST*> Args);
  lexval getLexVal() const override;
  Value *codegen(driver& drv) override;
};

class IfExprAST : public ExprAST {
private:
  ExprAST* Cond;
  ExprAST* TrueExp;
  ExprAST* FalseExp;
public:
  IfExprAST(ExprAST* Cond, ExprAST* TrueExp, ExprAST* FalseExp);
  Value *codegen(driver& drv) override;
};

class BlockExprAST : public ExprAST {
private:
  std::vector<VarBindingAST*> Def;
  SeqAST* Seq;
public:
  BlockExprAST(std::vector<VarBindingAST*> Def, SeqAST* Seq);
  Value *codegen(driver& drv) override;
};

class VarBindingAST: public RootAST {
private:
  const std::string Name;
  ExprAST* Val;
public:
  VarBindingAST(const std::string Name, ExprAST* Val);
  AllocaInst *codegen(driver& drv) override;
  const std::string& getName() const;
};

/// Function prototype.
/// Made up of name, arguments number and names.
/// The type is implicitly double.
class PrototypeAST : public RootAST {
private:
  std::string Name;
  std::vector<std::string> Args;
  bool emitcode;

public:
  PrototypeAST(std::string Name, std::vector<std::string> Args);
  const std::vector<std::string> &getArgs() const;
  lexval getLexVal() const override;
  Function *codegen(driver& drv) override;
  void noemit();
};

/// Function definition.
class FunctionAST : public RootAST {
private:
  PrototypeAST* Proto;
  ExprAST* Body;
  bool external;
  
public:
  FunctionAST(PrototypeAST* Proto, ExprAST* Body);
  Function *codegen(driver& drv) override;
};

// Global variable declaration
class GlobalVarAST : public RootAST {
private:
  std::string name;

public:
  GlobalVarAST(const std::string &name) : name(name) {};
  GlobalVariable* codegen(driver &d) override;
  std::string getName() const { return name; };
};

class AssignmentExprAST : public ExprAST {
private:
  std::string name;
  ExprAST *val;

public:
  AssignmentExprAST(const std::string &name, ExprAST *val) :
    name(name), val(val) {};
  Value* codegen(driver &d) override;
  std::string getName() const { return name; };
};

#endif // ! DRIVER_HH
