#include "driver.hpp"
#include "parser.hpp"
#include "utils.hpp"


LLVMContext *context = new LLVMContext;
Module *module = new Module("Kaleidoscope", *context);
IRBuilder<> *builder = new IRBuilder(*context);

driver::driver(): trace_parsing(false), trace_scanning(false) {};

int driver::parse (const std::string &f) {
  file = f;                    // Input file
  location.initialize(&file);
  scan_begin();                // Input file opening
  yy::parser parser(*this);    // Parser instantiation
  parser.set_debug_level(trace_parsing);
  int res = parser.parse();    // Parser entry-point call
  scan_end();                  // Input file close
  return res;
}

void driver::codegen() {
  root->codegen(*this);
};


SeqAST::SeqAST(RootAST* first, RootAST* continuation):
  first(first), continuation(continuation) {};

Value *SeqAST::codegen(driver& drv) {
  Value *v = nullptr;
  
  if (first) v = first->codegen(drv);
  if (continuation) v = continuation->codegen(drv);
  return v;
};


NumberExprAST::NumberExprAST(double Val) : Val(Val) {};

lexval NumberExprAST::getLexVal() const {
  lexval lval = Val;
  return lval;
};

// Returns a constant value; uniqueness guaranteed by the context.
Value *NumberExprAST::codegen(driver& drv) {  
  return ConstantFP::get(*context, APFloat(Val));
};


VariableExprAST::VariableExprAST(const std::string &Name) : Name(Name) {};

lexval VariableExprAST::getLexVal() const {
  lexval lval = Name;
  return lval;
};

// Notice that NamedValues is the locally-defined symbol table
Value *VariableExprAST::codegen(driver& drv) {
  auto maybeSymbol = tryGetSymbol(drv, Name);
  if (not maybeSymbol) return nullptr;

  auto symbol = *maybeSymbol;
  if (auto *A = std::get_if<AllocaInst*>(&symbol)) return builder->CreateLoad((*A)->getAllocatedType(), *A, Name.c_str());
  if (auto *G = std::get_if<GlobalVariable*>(&symbol)) return builder->CreateLoad((*G)->getValueType(), *G, Name.c_str());

  return nullptr;
}


BinaryExprAST::BinaryExprAST(char Op, ExprAST* LHS, ExprAST* RHS) :
  Op(Op), LHS(LHS), RHS(RHS) {};

Value *BinaryExprAST::codegen(driver& drv) {
  Value *L = LHS->codegen(drv);
  Value *R = nullptr;
  if (RHS) R = RHS->codegen(drv);

  if (!L || (R && !R)) 
    return nullptr;
  switch (Op) {
  case '+':
    return builder->CreateFAdd(L,R,"addres");
  case '-':
    return builder->CreateFSub(L, R, "subres");
  case '*':
    return builder->CreateFMul(L, R, "mulres");
  case '/':
    return builder->CreateFDiv(L, R, "addres");
  case '<':
    return builder->CreateFCmpULT(L, R, "lttest");
  case '>':
    return builder->CreateFCmpULT(R, L, "lttest");
  case '=':
    return builder->CreateFCmpUEQ(L, R, "eqtest");
  case '!':  // Logical not
    return builder->CreateNot(L, "lnotres");
  case '&':
    return builder->CreateAnd(L, R, "landres");
  case '|':
    return builder->CreateOr(L, R, "lorres");
  default:
    using namespace std::string_literals;
    return logError(std::to_string(Op) + " operatore binario non supportato", drv);
  }
};


CallExprAST::CallExprAST(std::string Callee, std::vector<ExprAST*> Args) :
  Callee(Callee),  Args(std::move(Args)) {};

lexval CallExprAST::getLexVal() const {
  lexval lval = Callee;
  return lval;
};

Value* CallExprAST::codegen(driver& drv) {
  Function *CalleeF = module->getFunction(Callee);
  if (!CalleeF)  // Function existance check
    return logError("Funzione non definita", drv);

  if (CalleeF->arg_size() != Args.size())  // Params number check
    return logError("Numero di argomenti non corretto", drv);

  std::vector<Value *> ArgsV;
  for (auto arg : Args) {
    ArgsV.push_back(arg->codegen(drv));
    if (!ArgsV.back())
      return nullptr;
  }
  return builder->CreateCall(CalleeF, ArgsV, "calltmp");
}


IfExprAST::IfExprAST(ExprAST* Cond, ExprAST* TrueExp, ExprAST* FalseExp):
   Cond(Cond), TrueExp(TrueExp), FalseExp(FalseExp) {};
   
Value* IfExprAST::codegen(driver& drv) {
  Value* CondV = Cond->codegen(drv);
  if (!CondV)
      return nullptr;
  
  Function *function = builder->GetInsertBlock()->getParent();
  BasicBlock *TrueBB =  BasicBlock::Create(*context, "trueexp", function);
  BasicBlock *FalseBB;
  if (FalseExp) FalseBB = BasicBlock::Create(*context, "falseexp");
  BasicBlock *MergeBB = BasicBlock::Create(*context, "endcond");
  // False and Merge BB are not inserted yet because True BB could contains multiple blocks
  
  builder->CreateCondBr(CondV, TrueBB, FalseExp ? FalseBB : MergeBB);
  
  builder->SetInsertPoint(TrueBB);
  Value *TrueV = TrueExp->codegen(drv);
  if (not TrueV) return nullptr;
  builder->CreateBr(MergeBB);  // Inconditionally branch to MergeBB
  
  // TrueBB could have generated some blocks in the middle
  // so get a feedback about current block
  TrueBB = builder->GetInsertBlock();

  Value *FalseV;
  if (FalseExp) {
    function->insert(function->end(), FalseBB);
    builder->SetInsertPoint(FalseBB);
    FalseV = FalseExp->codegen(drv);
    if (!FalseV)
      return nullptr;
    
    builder->CreateBr(MergeBB);  // Inconditionally branch to MergeBB
    FalseBB = builder->GetInsertBlock();  // Get FalseBB position feedback
  }

  function->insert(function->end(), MergeBB);
  builder->SetInsertPoint(MergeBB);
  
  if (FalseExp) {
    PHINode *PN = builder->CreatePHI(Type::getDoubleTy(*context), 2, "condval");
    PN->addIncoming(TrueV, TrueBB);
    if (FalseExp) PN->addIncoming(FalseV, FalseBB);
    return PN;
  }
  
  return UndefValue::get(Type::getDoubleTy(*context));
};


BlockExprAST::BlockExprAST(std::vector<VarBindingAST*> Def, SeqAST* Seq) : 
  Def(std::move(Def)), Seq(Seq) {};

Value* BlockExprAST::codegen(driver& drv) {
  // BlockExprAST is in charge of managing the visibility of bindings and variables through the symbol table.
  // Allocations are placed in the function's entry block.

  // Current scope is saved before shadowing is applied.
  std::vector<AllocaInst*> AllocaTmp;

  for (auto &def : Def) {
    const auto &name = def->getName();
    AllocaInst *boundval = def->codegen(drv);

    if (!boundval) 
      return nullptr;

    AllocaTmp.push_back(drv.NamedValues[name]);
    drv.NamedValues[name] = boundval;
  };

  Value *blockvalue = Seq->codegen(drv);
  
  if (!blockvalue) {
    return logError("Invalid block sequence", drv);
  }

  if (isa<UndefValue>(blockvalue)) {
    logWarning("Uncomplete or invalid block expression. Expanding as undef.", drv);
  }

// Previous scope is restored
  for (int i=0; auto &def : Def) {
    drv.NamedValues[def->getName()] = AllocaTmp[i++];
  };

  return blockvalue;
};


VarBindingAST::VarBindingAST(const std::string Name, ExprAST* Val) :
  Name(Name), Val(Val) {};
   
std::string VarBindingAST::getName() const { 
  return Name;  
};

AllocaInst* VarBindingAST::codegen(driver& drv) {
  // Get current function to allocate memory in its activation record
  Function *fun = builder->GetInsertBlock()->getParent();

  // Bindings without rhs can exist in order to shadow global vars
  if (not Val) Val = new NumberExprAST(0);

  Value *BoundVal = Val->codegen(drv);  // Generate value
  if (!BoundVal) return nullptr;

  AllocaInst *Alloca = CreateEntryBlockAlloca(fun, Name);
  builder->CreateStore(BoundVal, Alloca);
  
  return Alloca;
};


PrototypeAST::PrototypeAST(std::string Name, std::vector<std::string> Args) :
  Name(Name), Args(std::move(Args)), emitcode(true) {};

lexval PrototypeAST::getLexVal() const {
  lexval lval = Name;
  return lval;	
};

const std::vector<std::string>& PrototypeAST::getArgs() const { 
   return Args;
};

// Prevents double code emission: see PrototypeAST and FunctionAST
void PrototypeAST::noemit() { 
   emitcode = false; 
};

Function *PrototypeAST::codegen(driver& drv) {
  // Define args vector and function type (retval, argsval).
  std::vector<Type*> Doubles(Args.size(), Type::getDoubleTy(*context));
  FunctionType *FT = FunctionType::get(Type::getDoubleTy(*context), Doubles, false);
  Function *F = Function::Create(FT, Function::ExternalLinkage, Name, *module);

  unsigned Idx = 0;
  for (auto &Arg : F->args())
    Arg.setName(Args[Idx++]);

  if (emitcode) {
    F->print(errs());
    fprintf(stderr, "\n");
  };
  
  return F;
}


FunctionAST::FunctionAST(PrototypeAST* Proto, ExprAST* Body) :
  Proto(Proto), Body(Body) {};

Function *FunctionAST::codegen(driver& drv) {
  Function *function = 
    module->getFunction(std::get<std::string>(Proto->getLexVal()));

  if (!function)
    function = Proto->codegen(drv);
  else
    return nullptr;

  if (!function)
    return nullptr;  


  BasicBlock *BB = BasicBlock::Create(*context, "entry", function);
  builder->SetInsertPoint(BB);
  
  for (auto &Arg : function->args()) {
    AllocaInst *Alloca = CreateEntryBlockAlloca(function, Arg.getName());
    builder->CreateStore(&Arg, Alloca);
    drv.NamedValues[std::string(Arg.getName())] = Alloca;
  } 
  
  if (Value *RetVal = Body->codegen(drv)) {
    // If body generation is good, get return value and add a return instruction
    builder->CreateRet(RetVal);

    // Consistency control
    verifyFunction(*function);

    // Emit code
    function->print(errs());
    fprintf(stderr, "\n");
    return function;
  }

  // If some error occurred in function's emission
  function->eraseFromParent();
  return nullptr;
};


GlobalVariable* GlobalVarAST::codegen(driver &drv) {
  auto *globalVar = new GlobalVariable(
    *module,
    Type::getDoubleTy(*context),
    false,  // Not constant
    GlobalValue::LinkageTypes::CommonLinkage,
    ConstantFP::getZero(Type::getDoubleTy(*context)),
    name
  );

  globalVar->print(errs()); errs() << "\n";

  return globalVar;
};


Value* AssignmentExprAST::codegen(driver &drv) {
  auto maybeSymbol = tryGetSymbol(drv, name);
  if (not maybeSymbol) return nullptr;
  auto symbol = *maybeSymbol;

  Value *v = val->codegen(drv);
  if (not v) return nullptr;

  if (auto *A = std::get_if<AllocaInst*>(&symbol)) {
    builder->CreateStore(v, *A);
    return v;
  }

  if (auto *G = std::get_if<GlobalVariable*>(&symbol)) {
    builder->CreateStore(v, *G);
    return v;
  }

  return nullptr;
}

Value* ForExprAST::codegen(driver &drv) {
  auto *function = builder->GetInsertBlock()->getParent();
  
  auto addBlock = [&function] (BasicBlock *bb) -> void {
    function->insert(function->end(), bb);
    builder->SetInsertPoint(bb);
  };
  auto _logError = std::bind(logError, std::placeholders::_1, drv);

  // Loop building blocks
  auto *preheaderBB = BasicBlock::Create(*context, "preheader");
  auto *headerBB = BasicBlock::Create(*context, "header");
  auto *bodyBB = BasicBlock::Create(*context, "body");
  auto *latchBB = BasicBlock::Create(*context, "latch");
  auto *exitBB = BasicBlock::Create(*context, "exit");

  builder->CreateBr(preheaderBB);

  addBlock(preheaderBB);
  auto initRes = init->codegen(drv);
  if (not initRes) return _logError("Error while creating preheader");
  builder->CreateBr(headerBB);

  // Freeze scope
  AllocaInst *prevInitScope = nullptr;
  const std::string &initName = init->getName();
  auto bindAlloca = dyn_cast<AllocaInst>(initRes);
  const bool isInitBound = bindAlloca != nullptr;
  if (isInitBound) {
    prevInitScope = drv.NamedValues[initName];
    drv.NamedValues[initName] = bindAlloca;
  }

  addBlock(headerBB);
  Value *condVal = cond->codegen(drv);
  if (not condVal) return _logError("Error while creating condition expression");
  builder->CreateCondBr(condVal, bodyBB, exitBB);

  addBlock(bodyBB);
  if (not body->codegen(drv)) return _logError("Error while generating body");
  builder->CreateBr(latchBB);

  addBlock(latchBB);
  if (not assignment->codegen(drv))
    return _logError("Error while generating assignment");
  builder->CreateBr(headerBB);

  // Restore scope
  if (isInitBound) drv.NamedValues[initName] = prevInitScope;

  addBlock(exitBB);

  return UndefValue::get(Type::getDoubleTy(*context));
}