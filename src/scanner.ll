%{ /* -*- C++ -*- */
# include <cerrno>
# include <climits>
# include <cstdlib>
# include <string>
# include <cmath>
# include "driver.hpp"
# include "parser.hpp"
%}

%option noyywrap nounput batch debug noinput

id      [a-zA-Z][a-zA-Z_0-9]*
fpnum   [0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?
fixnum  (0|[1-9][0-9]*)\.?[0-9]*
num     {fpnum}|{fixnum}
blank   [ \t]

%{
  // Codice eseguito ad ogni match con una regexp
  # define YY_USER_ACTION loc.columns(yyleng);
%}
%%
%{
  // La location è memorizzata nel driver ma è utile
  // potervi fare riferimento in modo più succinto
  // con una variabile locale
  yy::location& loc = drv.location;
  
  // Codice eseguito ogni volta che yylex viene chiamata
  loc.step ();
%}
{blank}+   loc.step ();
[\n]+      loc.lines (yyleng); loc.step ();

"-"      return yy::parser::make_MINUS     (loc);
"+"      return yy::parser::make_PLUS      (loc);
"*"      return yy::parser::make_STAR      (loc);
"/"      return yy::parser::make_SLASH     (loc);
"("      return yy::parser::make_LPAREN    (loc);
")"      return yy::parser::make_RPAREN    (loc);
";"      return yy::parser::make_SEMICOLON (loc);
","      return yy::parser::make_COMMA     (loc);
"?"      return yy::parser::make_QMARK     (loc);
":"      return yy::parser::make_COLON     (loc);
"<"      return yy::parser::make_LT        (loc);
"=="      return yy::parser::make_EQ       (loc);
"="      return yy::parser::make_ASSIGN    (loc);
"{"      return yy::parser::make_LBRACE    (loc);
"}"      return yy::parser::make_RBRACE    (loc);

{num}    { errno = 0;
           double n = strtod(yytext, NULL);
           if (! (n!=HUGE_VAL && n!=-HUGE_VAL && errno != ERANGE))
           throw yy::parser::syntax_error (loc, "Float value is out of range: "
                      + std::string(yytext));
           return yy::parser::make_NUMBER(n, loc);
         }
         
"def"    { return yy::parser::make_DEF(loc); }
"extern" { return yy::parser::make_EXTERN(loc); }
"var"    { return yy::parser::make_VAR(loc); }

{id}     { return yy::parser::make_IDENTIFIER (yytext, loc); }

.        { throw yy::parser::syntax_error
               (loc, "invalid character: " + std::string(yytext));
         }
         
<<EOF>>  { return yy::parser::make_END (loc); }
%%

void driver::scan_begin () {
  yy_flex_debug = trace_scanning;
  if (file.empty () || file == "-")
    yyin = stdin;
  else if (!(yyin = fopen (file.c_str (), "r")))
    {
      std::cerr << "cannot open " << file << ": " << strerror(errno) << '\n';
      exit (EXIT_FAILURE);
    }
}

void
driver::scan_end ()
{
  fclose (yyin);
}
