%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "SIRENLexer.h"
#include "SIRENInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::SIRENParser::token token;
typedef wasp::SIRENParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "SIRENFlexLexer" */
%option prefix="WASPSIREN"
%option outfile="SIRENLexer.cpp"
 /*%option yyclass="wasp::SIRENLexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 // %option debug

 /* no support for include files is planned */
%option yywrap nounput

 /* enables the use of start condition stacks */
 // %option stack

 // consume everthing,
 // escaped characters like '\/'
 // until control character
 // decl =
 // decl \n
 // decl /
 // this includes whitespace to provide for
 // declarators that are multi word
DECL ([0-9A-Za-z_\*?]((\.|[^\n/<>:\[=])*[A-Za-z_0-9?])?\*?)|\.

INTEGER [0-9]+([eE]\+?[0-9]+)?
EXPONENT [eE][\+\-]?{INTEGER}
DOUBLE {INTEGER}?\.{INTEGER}{EXPONENT}?|{INTEGER}\.({INTEGER}{EXPONENT}?)?|{INTEGER}\.?[eE]\-{INTEGER}


DOUBLE_QUOTED_STRING \"([^\"\n])*\"
SINGLE_QUOTED_STRING \'([^\\\'\n])*\'
QSTRING {DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING}

PARENT \.\.
ANY \/\/
LTE <=
GTE >=
LT <
GT >
BANG !
EXPONENT_OP \^
EQ =
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

{PARENT}  {
    capture_token(yylval,wasp::PARENT);
    return token::PARENT;
}
{ANY}  {
    capture_token(yylval,wasp::ANY);
    return token::ANY;
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
{LPAREN} {
    capture_token(yylval,wasp::LPAREN);
    return token::LPAREN;
}
{RPAREN} {
    capture_token(yylval,wasp::RPAREN);
    return token::RPAREN;
}
{EXPONENT_OP} {
    capture_token(yylval,wasp::EXPONENT);
    return token::EXPONENT;
}
  /* TODO add division operator*/
\/ {
  capture_token(yylval,wasp::SEPARATOR);
  return token::SEPARATOR;
}
\+ {
  capture_token(yylval,wasp::PLUS);
  return token::PLUS;
}
- {
    capture_token(yylval,wasp::MINUS);
    return token::MINUS;
}

{INTEGER} {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}

{DOUBLE} {
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}
 /* gobble up white-spaces */
[ \t\r]+ {
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


{DECL} {
    capture_token(yylval,wasp::DECL);
    return token::DECL;
}
\* {
    capture_token(yylval,wasp::MULTIPLY);
    return token::MULTIPLY;
}

 /* pass all other characters up to bison
  * NOTE: references do not support whitespace */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the wasp lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

SIRENLexerImpl::SIRENLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : WASPSIRENFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

SIRENLexerImpl::~SIRENLexerImpl()
{
}

void SIRENLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void SIRENLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void SIRENLexerImpl::capture_token(
        wasp::SIRENParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
}

/* This implementation of SIRENFlexLexer::yylex() is required to fill the
 * vtable of the class SIRENFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the SIRENLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int WASPSIRENFlexLexer::yylex()
{
    std::cerr << "in SIRENFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int WASPSIRENFlexLexer::yywrap()
{
    return 1;
}
