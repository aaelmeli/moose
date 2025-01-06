%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "JSONObjectLexer.h"

/* import the parser's token type into a local typedef */
typedef wasp::JSONObjectParser::token token;
typedef wasp::JSONObjectParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "JSONObjectFlexLexer" */
%option prefix="JSONObject"
%option outfile="JSONObjectLexer.cpp"
 /*%option yyclass="wasp::JSONObjectLexerImpl"*/

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
DOUBLE_QUOTED_STRING \"([^\\"]|\\\"|\\[^"])*\"
SINGLE_QUOTED_STRING \'(\\.|[^'\n])*\'


 /* The following paragraph suffices to track locations accurately. Each time
 * yylex is invoked, the begin position is moved onto the end position. */
%{
#define YY_USER_ACTION  yylloc->columns(yyleng);
%}

%% /*** Regular Expressions Part ***/

 /* code to place at the beginning of yylex() */
%{
    // reset location
    yylloc->step();
%}
 /*** BEGIN EXAMPLE - Change the wasp lexer rules below ***/
{DOUBLE}  {
    wasp::to_type(yylval->real,yytext);
    return token::DOUBLE;
}
{INTEGER}  {
    wasp::to_type(yylval->integer,yytext);
    return token::INTEGER;
}

 /* gobble up white-spaces */
[ \t\r]+ {
    yylloc->step();
}

 /* gobble up end-of-lines */
\n {
    yylloc->lines(yyleng); yylloc->step();
}

{DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING} {
    yylval->string = new std::string(yytext);
    return token::QSTRING;
}
{TOKEN_TRUE} {
    yylval->boolean = true;
    return token::TOKEN_TRUE;
}
{TOKEN_FALSE} {
    yylval->boolean = false;
    return token::TOKEN_FALSE;
}
{TOKEN_NULL} {    
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

JSONObjectLexerImpl::JSONObjectLexerImpl(
                std::istream* in,
                std::ostream* out)
    : JSONObjectFlexLexer(in, out)
{
}

JSONObjectLexerImpl::~JSONObjectLexerImpl()
{
}

void JSONObjectLexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
}

/* This implementation of JSONObjectFlexLexer::yylex() is required to fill the
 * vtable of the class JSONObjectFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the JSONObjectLexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int JSONObjectFlexLexer::yylex()
{
    std::cerr << "in JSONObjectFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int JSONObjectFlexLexer::yywrap()
{
    return 1;
}
