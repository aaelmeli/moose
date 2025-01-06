%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "EDDILexer.h"
#include "EDDInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::EDDIParser::token token;
typedef wasp::EDDIParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "EDDIFlexLexer" */
%option prefix="EDDI"
%option outfile="EDDILexer.cpp"
 /*%option yyclass="wasp::EDDILexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput

 /* enables the use of start condition stacks */
%option stack
%x file_include
/*%s inclusive*/

FRAC \-?[0-9]+([eE]\+?[0-9]+)?
WHOLE [\+\-]?[0-9]+([eE]\+?[0-9]+)?
INTEGER {WHOLE}
EXPONENT [eE]{WHOLE}

DOUBLE {WHOLE}?\.{FRAC}?{EXPONENT}?|{WHOLE}\.({FRAC}?{EXPONENT}?)?|{WHOLE}\.?{EXPONENT}
NUMBER {DOUBLE}|{INTEGER}
STRING ([A-Za-z0-9_\.\-\+:])+


DOUBLE_QUOTED_STRING \"([^\"\n])*\"
SINGLE_QUOTED_STRING \'([^\'\n])*\'
QSTRING {DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING}
UNICODE [^\x00-\x7F]+
COMMENT !([^\n]|{UNICODE})*
MINUS -
COMMA ,
SEMICOLON ;
ASSIGN =
LBRACKET \[
RBRACKET \]
FSLASH \/
INCLUDE_PATH [^ \t\n][^\n!]*
FILL_EXPR <[^>]*>|{NUMBER}\*({NUMBER}|{STRING}) 


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

{FILL_EXPR} {
    capture_token(yylval,wasp::FILL_EXPR);
    return token::FILL_EXPR;
}
{MINUS} {
    capture_token(yylval,wasp::MINUS);
    return token::MINUS;
}
{COMMA} {
    capture_token(yylval,wasp::WASP_COMMA);
    return token::WASP_COMMA;
}
{SEMICOLON} {
    capture_token(yylval,wasp::TERM);
    return token::SEMICOLON;
}
{ASSIGN}  {
    capture_token(yylval,wasp::ASSIGN);
    return token::ASSIGN;
}
{LBRACKET}  {
    capture_token(yylval,wasp::LBRACKET);
    return token::LBRACKET;
}
{RBRACKET}  {
    capture_token(yylval,wasp::RBRACKET);
    return token::RBRACKET;
}
{FSLASH}  {
    capture_token(yylval,wasp::DIVIDE);
    return token::FSLASH;
}

{DOUBLE} {
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}
{INTEGER} {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
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
include {
    yy_push_state(file_include);
    capture_token(yylval,wasp::FILE);
    return token::FILE;
}
<file_include>{INCLUDE_PATH} {
    // file includes grab everyting starting after 'include' to
    // either a newline '\n' or a comment '!'
    yy_pop_state();
    capture_token(yylval,wasp::STRING);
    return token::STRING;
}
{STRING} {
    capture_token(yylval,wasp::STRING);
    return token::STRING;
}
{COMMENT} {
    capture_token(yylval,wasp::COMMENT);
    return token::COMMENT;
}

 /* gobble up white-spaces */
<*>[ \t\r]+|{UNICODE} {
    yylloc->step();
}

 /* pass all other characters up to bison*/
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the wasp lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

EDDILexerImpl::EDDILexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : EDDIFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

EDDILexerImpl::~EDDILexerImpl()
{
}

void EDDILexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void EDDILexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void EDDILexerImpl::capture_token(
        wasp::EDDIParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
}

/* This implementation of EDDIFlexLexer::yylex() is required to fill the
 * vtable of the class EDDIFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the EDDILexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int EDDIFlexLexer::yylex()
{
    std::cerr << "in EDDIFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int EDDIFlexLexer::yywrap()
{
    return 1;
}
