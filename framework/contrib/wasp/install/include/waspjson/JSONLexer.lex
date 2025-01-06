%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "JSONLexer.h"
#include "JSONInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::JSONParser::token token;
typedef wasp::JSONParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "JSONFlexLexer" */
%option prefix="JSON"
%option outfile="JSONLexer.cpp"
 /*%option yyclass="wasp::JSONLexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput

 /* enables the use of start condition stacks */
 /*%option stack*/
 /*%x exclusive*/
 /*%s inclusive*/

INTEGER \-?[0-9]+
EXPONENT [eE][\+\-]?{INTEGER}
DOUBLE {INTEGER}?\.{INTEGER}{EXPONENT}?|{INTEGER}\.({INTEGER}{EXPONENT}?)?|{INTEGER}{EXPONENT}


TOKEN_TRUE true
TOKEN_FALSE false
TOKEN_NULL null
DOUBLE_QUOTED_STRING \"(\\.|[^"\n])*\"
SINGLE_QUOTED_STRING \'(\\.|[^'\n])*\'
LBRACKET \[
RBRACKET \]
LBRACE \{
RBRACE \}
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
{DOUBLE}  {
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}
{INTEGER}  {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
}
{COMMA}  {
    capture_token(yylval,wasp::WASP_COMMA);
    return token::COMMA;
}
{COLON}  {
    capture_token(yylval,wasp::COLON);
    return token::COLON;
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
 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
    interpreter.push_line_offset(file_offset-yyleng);
}

{DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING} {
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
{TOKEN_NULL} {
    capture_token(yylval,wasp::WASP_NULL);
    return token::TOKEN_NULL;
}

 /* pass all other characters up to bison
  * NOTE: references do not support whitespace */
. {
    return static_cast<token_type>(*yytext);
}

 /*** END EXAMPLE - Change the wasp lexer rules above ***/

%% /*** Additional Code ***/

namespace wasp {

JSONLexerImpl::JSONLexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : JSONFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

JSONLexerImpl::~JSONLexerImpl()
{
}

void JSONLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void JSONLexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void JSONLexerImpl::capture_token(
        wasp::JSONParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
}

/* This implementation of JSONFlexLexer::yylex() is required to fill the
 * vtable of the class JSONFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the JSONLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int JSONFlexLexer::yylex()
{
    std::cerr << "in JSONFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int JSONFlexLexer::yywrap()
{
    return 1;
}
