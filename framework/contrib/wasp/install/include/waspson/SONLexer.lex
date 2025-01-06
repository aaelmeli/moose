%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "SONLexer.h"
#include "SONInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::SONParser::token token;
typedef wasp::SONParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "SONFlexLexer" */
%option prefix="SON"
%option outfile="SONLexer.cpp"
 /*%option yyclass="wasp::SONLexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput

 /* enables the use of start condition stacks */
%option stack
%x file_import
%x subtraction
%s execution_unit

INTEGER \-?[0-9]+
EXPONENT [eE][\+\-]?{INTEGER}
DOUBLE {INTEGER}?\.{INTEGER}{EXPONENT}?|{INTEGER}\.({INTEGER}{EXPONENT}?)?|{INTEGER}{EXPONENT}|\-\.{INTEGER}{EXPONENT}?

STRING [A-Za-z_]([A-Za-z0-9_])*
FILLER_REPEAT [0-9]+[rR][' ']*({INTEGER}|{DOUBLE})
FILLER_ALTERNATE [0-9]+[pP][' ']*({INTEGER}|{DOUBLE})
FILLER_FILL [fF]({INTEGER}|{DOUBLE})
FILLER_LINEAR_INTERP [0-9]+[iI][' ']*({INTEGER}|{DOUBLE})[' ']+({INTEGER}|{DOUBLE})
FILLER_LOG_INTERP [0-9]+[lL][' ']*({INTEGER}|{DOUBLE})[' ']+({INTEGER}|{DOUBLE})
FILLER {FILLER_REPEAT}|{FILLER_ALTERNATE}|{FILLER_FILL}|{FILLER_LINEAR_INTERP}|{FILLER_LOG_INTERP}


 /* This string does not allow special characters '-','/'
 *  and should only occur in the context of reference
 */
LESSER_STRING [A-Za-z_][A-Za-z0-9_]*

TOKEN_TRUE true|TRUE|True|yes|YES|Yes
TOKEN_FALSE false|FALSE|False|no|NO|No
DOUBLE_QUOTED_STRING \"([^\"\n])*\"
SINGLE_QUOTED_STRING \'([^\\\'\n])*\'
QSTRING {DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING}
COMMENT \/\/[^\n]*|[%#][^\n]*
 /*
 * The 'execution unit' is a rebranded SCALE sequence construct
 * where the sequence started with the unit_start rule below
 * and terminated with unit_end rule below.
 * We reproduce it here to account for the input construct
 * while in transition...
 */
EXECUTION_UNIT_START ^=
EXECUTION_UNIT_END ^[Ee][Nn][Dd]
LTE <=
GTE >=
LT <
GT >
BANG !
EQ ==
EXPONENT_OP \^
ASSIGN =
NEQ \!=
AND &&
OR \|\|
LBRACKET \[
RBRACKET \]
LBRACE \{
RBRACE \}
LPAREN \(
RPAREN \)
COMMA ,
COLON :
FILE_IMPORT `import

 /* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng); file_offset+=yyleng;
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the wasp lexer rules below ***/

{EXECUTION_UNIT_START} {
    yy_push_state(execution_unit); // enter the 'unit' of execution
        capture_token(yylval,wasp::EXECUTION_UNIT_START);
    return token::EXECUTION_UNIT_START;
}
<execution_unit>{EXECUTION_UNIT_END} {
    yy_pop_state(); // pop the execution state
    capture_token(yylval,wasp::EXECUTION_UNIT_END);
    return token::EXECUTION_UNIT_END;
}
<subtraction>\- { // capture the subtraction, return the literal '-'
    yy_pop_state();
    capture_token(yylval,wasp::MINUS);
    return token::MINUS;
}

{FILE_IMPORT} {
    yy_push_state(file_import);
    capture_token(yylval,wasp::FILE);
    return token::FILE;
}
<file_import>{LPAREN} {
    capture_token(yylval,wasp::LPAREN);
    return token::LPAREN;
}
<file_import>{QSTRING} {
    capture_token(yylval,wasp::QUOTED_STRING);
    return token::QSTRING;
}
<file_import>{RPAREN} {
    yy_pop_state();
    capture_token(yylval,wasp::RPAREN);
    return token::RPAREN;
}

{FILLER} {
    capture_token(yylval,wasp::FILL_EXPR);
    return token::FILLER;
}
{COMMA}  {
    capture_token(yylval,wasp::WASP_COMMA);
    return token::COMMA;
}
{COLON}  {
    capture_token(yylval,wasp::COLON);
    return token::COLON;
}
{LTE} {
    capture_token(yylval,wasp::LTE);
    return token::LTE;
}
{GTE}  {
    capture_token(yylval,wasp::GTE);
    return token::GTE;
}
{LT} {
    capture_token(yylval,wasp::LT);
    return token::LT;
}
{GT}  {
    capture_token(yylval,wasp::GT);
    return token::GT;
}
{EQ} {
    capture_token(yylval,wasp::EQ);
    return token::EQ;
}
{BANG} {
    capture_token(yylval,wasp::BANG);
    return token::BANG;
}
{ASSIGN} {
    capture_token(yylval,wasp::ASSIGN);
    return token::ASSIGN;
}
{NEQ} {
    capture_token(yylval,wasp::NEQ);
    return token::NEQ;
}
{AND} {
    capture_token(yylval,wasp::WASP_AND);
    return token::AND;
}
{OR} {
    capture_token(yylval,wasp::WASP_OR);
    return token::OR;
}
{LBRACKET} {
    capture_token(yylval,wasp::LBRACKET);
    return token::LBRACKET;
}
{RBRACKET} {
    capture_token(yylval,wasp::RBRACKET);
    return token::RBRACKET;
}
{LBRACE} {
    capture_token(yylval,wasp::LBRACE);
    return token::LBRACE;
}
{RBRACE} {
    capture_token(yylval,wasp::RBRACE);
    return token::RBRACE;
}
{EXPONENT_OP} {
    capture_token(yylval,wasp::EXPONENT);
    return token::EXPONENT;
}
\* {
    capture_token(yylval,wasp::MULTIPLY);
    return token::MULTIPLY;
}
\/ {
  capture_token(yylval,wasp::DIVIDE);
  return token::DIVIDE;
}
\+ {
  capture_token(yylval,wasp::PLUS);
  return token::PLUS;
}
- {
    capture_token(yylval,wasp::MINUS);
    return token::MINUS;
}
\( {
  capture_token(yylval,wasp::LPAREN);
  return token::LPAREN;
}
\) {
 capture_token(yylval,wasp::RPAREN);
 return token::RPAREN;
}
 /* Cannot match this rule when in the subtraction state, it will override as a longer match */
<INITIAL,execution_unit>{INTEGER}/\- {
    yy_push_state(subtraction); // if we have a minus sign immediately following, subtraction is occurring
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}
<INITIAL,execution_unit>{INTEGER} {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}
 /* Cannot match this rule when in the subtraction state, it will override as a longer match */
<INITIAL,execution_unit>{DOUBLE}/\- {
   // if we have a minus sign immediately following, subtraction is occurring
    yy_push_state(subtraction);
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}

<INITIAL,execution_unit>{DOUBLE} {
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}
 /* gobble up white-spaces */
<*>[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

<*>{QSTRING} {
    capture_token(yylval,wasp::QUOTED_STRING);
    return token::QSTRING;
}
{TOKEN_TRUE} {
    capture_token(yylval,wasp::WASP_TRUE);
    return token::TOKEN_TRUE;
}
{TOKEN_FALSE} {
    capture_token(yylval,wasp::WASP_FALSE);
    return token::TOKEN_FALSE;
}

{STRING} {
    capture_token(yylval,wasp::STRING);
    return token::STRING;
}
<INITIAL,execution_unit>{STRING}/\- {
   // if we have a minus sign immediately following, subtraction is occurring
    yy_push_state(subtraction);
    capture_token(yylval,wasp::STRING);
    return token::STRING;
}
{COMMENT} {
    capture_token(yylval,wasp::COMMENT);
    return token::COMMENT;
}

 /* pass all other characters up to bison
  * NOTE: references do not support whitespace */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the wasp lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

SONLexerImpl::SONLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : SONFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

SONLexerImpl::~SONLexerImpl()
{
}

void SONLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void SONLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void SONLexerImpl::capture_token(
        wasp::SONParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
}

/* This implementation of SONFlexLexer::yylex() is required to fill the
 * vtable of the class SONFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the SONLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int SONFlexLexer::yylex()
{
    std::cerr << "in SONFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int SONFlexLexer::yywrap()
{
    return 1;
}
