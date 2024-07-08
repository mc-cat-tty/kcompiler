%skeleton "lalr1.cc"
%require "3.2"
%defines

%define api.token.constructor
%define api.location.file none
%define api.value.type variant
%define parse.assert

%code requires {
  #include <string>
  #include <exception>
  class driver;
  class RootAST;
  class ExprAST;
  class NumberExprAST;
  class VariableExprAST;
  class CallExprAST;
  class FunctionAST;
  class SeqAST;
  class PrototypeAST;
  class BlockExprAST;
  class VarBindingAST;
  class GlobalVarAST;
  class AssignmentExprAST;
  class ForExprAST;
  class IfExprAST;
  class InitAST;
  class BinaryExprAST;
  class UnaryExprAST;
}

%param { driver& drv }

%locations

%define parse.trace
%define parse.error verbose

%code {
# include "driver.hpp"
}

%define api.token.prefix {TOK_}
%token
  END  0  "end of file"
  SEMICOLON  ";"
  COMMA      ","
  MINUS      "-"
  PLUS       "+"
  STAR       "*"
  SLASH      "/"
  LPAREN     "("
  RPAREN     ")"
  QMARK	     "?"
  COLON      ":"
  LT         "<"
  GT         ">"
  EQ         "=="
  ASSIGN     "="
  LBRACE     "{"
  RBRACE     "}"
  EXTERN     "extern"
  DEF        "def"
  VAR        "var"
  GLOBAL     "global"
  IF         "if"
  ELSE       "else"
  FOR        "for"
  NOT        "not"
  OR         "or"
  AND        "and"
  LSBRACKET  "["
  RSBRACKET  "]" 
;

%token <std::string> IDENTIFIER "id"
%token <double> NUMBER "number"
%type <ExprAST*> exp
%type <ExprAST*> idexp
%type <IfExprAST*> expif
%type <std::vector<ExprAST*>> optexp
%type <std::vector<ExprAST*>> explist
%type <RootAST*> program
%type <RootAST*> top
%type <FunctionAST*> definition
%type <PrototypeAST*> external
%type <PrototypeAST*> proto
%type <std::vector<std::string>> idseq
%type <BlockExprAST*> block
%type <std::vector<VarBindingAST*>> vardefs
%type <VarBindingAST*> binding

%type <GlobalVarAST*> globalvar
%type <AssignmentExprAST*> assignment
%type <ExprAST*> initexp
%type <ExprAST*> stmt
%type <SeqAST*> stmts
%type <InitAST*> init
%type <IfExprAST*> ifstmt
%type <ForExprAST*> forstmt
%type <BinaryExprAST*> relexp
%type <BinaryExprAST*> condexp

%%

%start startsymb;

startsymb:
  program               { drv.root = $1; }

program:
  %empty                { $$ = new SeqAST(nullptr, nullptr); }
|  top ";" program      { $$ = new SeqAST($1, $3); };

top:
  %empty                { $$ = nullptr; }
| globalvar             { $$ = $1; }
| external              { $$ = $1; }
| definition            { $$ = $1; };

globalvar:
  "global" "id"         { $$ = new GlobalVarAST($2); };

external:
  "extern" proto        { $$ = $2; };

definition:
  "def" proto block     { $$ = new FunctionAST($2, $3); $2->noemit(); };

proto:
  "id" "(" idseq ")"    { $$ = new PrototypeAST($1, $3); };

idseq:
  %empty                { $$ = std::vector<std::string>{}; }
| "id" idseq            { $2.insert($2.begin(),$1); $$ = $2; };

%left ":", "?";
%left "or";
%left "and";
%left "not";
%left "<" "==";
%left "+" "-";
%left "*" "/";

exp:
  exp "+" exp           { $$ = new BinaryExprAST('+', $1, $3); }
| exp "-" exp           { $$ = new BinaryExprAST('-', $1, $3); }
| exp "*" exp           { $$ = new BinaryExprAST('*', $1, $3); }
| exp "/" exp           { $$ = new BinaryExprAST('/', $1, $3); }
| idexp                 { $$ = $1; }
| "(" exp ")"           { $$ = $2; }
| "number"              { $$ = new NumberExprAST($1); }
| expif                 { $$ = $1; };

stmts:
  stmt                  { $$ = new SeqAST($1, nullptr); };
| stmt ";" stmts        { $$ = new SeqAST($1, $3); };

stmt:
  assignment            { $$ = $1; }
| block                 { $$ = $1; }
| ifstmt                { $$ = $1; }
| forstmt               { $$ = $1; }
| exp                   { $$ = $1; };

ifstmt:
  "if" "(" condexp ")" stmt               { $$ = new IfExprAST($3, $5, nullptr); }
| "if" "(" condexp ")" stmt "else" stmt   { $$ = new IfExprAST($3, $5, $7); };

forstmt:
  "for" "(" init ";" condexp ";" assignment ")" stmt  { $$ = new ForExprAST($3, $5, $7, $9); };

init:
  binding                       { $$ = $1; }
| assignment                    { $$ = $1; };

assignment:
  "id" "=" exp                  { $$ = new AssignmentExprAST($1, $3); }
| "id" "[" exp "]" "=" exp      { TODO };

block:
  "{" stmts "}"                 { $$ = new BlockExprAST({}, $2); }
| "{" vardefs ";" stmts "}"     { $$ = new BlockExprAST($2, $4); };
  
vardefs:
  binding                       { $$ = std::vector<VarBindingAST*>{$1}; }
| vardefs ";" binding           { $1.push_back($3); $$ = $1; };
                            
binding:
  "var" "id" initexp                                { $$ = new VarBindingAST($2, $3); }
| "var" "id" "[" "number" "]"                       { TODO }
| "var" "id" "[" "number" "]" "=" "{" explist "}"   { TODO };

initexp:
  %empty                        { $$ = nullptr; }
| "=" exp                       { $$ = $2; };

expif:
  condexp "?" exp ":" exp       { $$ = new IfExprAST($1, $3, $5); };

condexp:
  relexp                        { $$ = $1; }
| relexp "and" condexp          { $$ = new BinaryExprAST('&', $1, $3); }
| relexp "or" condexp           { $$ = new BinaryExprAST('|', $1, $3); }
| "not" condexp                 { $$ = new UnaryExprAST('!', $2); }
| "(" condexp ")"               { $$ = $2; };

relexp:
  exp "<" exp                   { $$ = new BinaryExprAST('<', $1, $3); }
| exp ">" exp                   { $$ = new BinaryExprAST('>', $1, $3); }
| exp "==" exp                  { $$ = new BinaryExprAST('=', $1, $3); };

idexp:
  "id"                          { $$ = new VariableExprAST($1); }
| "id" "(" optexp ")"           { $$ = new CallExprAST($1, $3); }
| "id" "[" optexp "]"           { TODO };

optexp:
  %empty                        { $$ = std::vector<ExprAST*>{}; }
| explist                       { $$ = $1; };

explist:
  exp                           { $$ = std::vector<ExprAST*>{$1}; }
| exp "," explist               { $3.insert($3.begin(), $1); $$ = $3; };
 
%%

void yy::parser::error(const location_type &l, const std::string &m) {
  std::cerr << l << ": " << m << '\n';
}
