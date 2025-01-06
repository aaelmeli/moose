%top{
/* This disables inclusion of unistd.h, which is not available under Visual C++
 * on Win32. The C++ scanner uses STL streams instead. */
#define YY_NO_UNISTD_H

}
%{ /*** C/C++ Declarations ***/

#include <string>
#include <sstream>
#include "DDILexer.h"
#include "DDInterpreter.h"

/* import the parser's token type into a local typedef */
typedef wasp::DDIParser::token token;
typedef wasp::DDIParser::token_type token_type;

/* By default yylex returns int, we use token_type. Unfortunately yyterminate
 * by default returns 0, which is not of token_type. */
#define yyterminate() return token::END

%}

 /*** Flex Declarations and Options ***/

 /* enable c++ scanner class generation */
%option c++

 /* change the name of the scanner class. results in "DDIFlexLexer" */
%option prefix="DDI"
%option outfile="DDILexer.cpp"
 /*%option yyclass="wasp::DDILexerImpl"*/

 /* enable scanner to generate debug output. disable this for release
 * versions. */
 /*%option debug*/

 /* no support for include files is planned */
%option yywrap nounput

 /* enables the use of start condition stacks */
/*%option stack*/
/*%x exclusive*/
/*%s inclusive*/

FRAC \-?[0-9]+([eE]\+?[0-9]+)?
WHOLE [\+\-]?[0-9]+([eE]\+?[0-9]+)?
INTEGER {WHOLE}
EXPONENT [eE]{WHOLE}

DOUBLE {WHOLE}?\.{FRAC}?{EXPONENT}?|{WHOLE}\.({FRAC}?{EXPONENT}?)?|{WHOLE}\.?{EXPONENT}

STRING [A-Za-z_]([A-Za-z0-9_\.])*


DOUBLE_QUOTED_STRING \"([^\"\n])*\"
SINGLE_QUOTED_STRING \'([^\'\n])*\'
QSTRING {DOUBLE_QUOTED_STRING}|{SINGLE_QUOTED_STRING}
COMMENT #[^\n]*|%[^\n]*
COMMA ,
ASSIGN =

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

{COMMA}  {
    capture_token(yylval,wasp::WASP_COMMA);
    return token::COMMA;
}
{ASSIGN}  {
    capture_token(yylval,wasp::ASSIGN);
    return token::ASSIGN;
}
{DOUBLE} {
    capture_token(yylval,wasp::REAL);
    return token::DOUBLE;
}
{INTEGER} {
    capture_token(yylval,wasp::INTEGER);
    return token::INTEGER;
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

{STRING} {
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

DDILexerImpl::DDILexerImpl(
                AbstractInterpreter & interpreter,
                std::istream* in,
                std::ostream* out)
    : DDIFlexLexer(in, out)
    , interpreter(interpreter)
    , file_offset(0)
{
}

DDILexerImpl::~DDILexerImpl()
{
}

void DDILexerImpl::set_debug(bool b)
{
    yy_flex_debug = b;
}
void DDILexerImpl::rewind()
{
    yyin.seekg(-yyleng,std::ios_base::cur);
    yyless(0);
}
void DDILexerImpl::capture_token(
        wasp::DDIParser::semantic_type* yylval
        ,wasp::NODE type)
{
    std::size_t offset = file_offset - yyleng;
    yylval->token_index = interpreter.token_count();
    interpreter.push_token(yytext,type,offset);
}
}

/* This implementation of DDIFlexLexer::yylex() is required to fill the
 * vtable of the class DDIFlexLexer. We define the scanner's main yylex
 * function via YY_DECL to reside in the DDILexerImpl class instead. */

#ifdef yylex
#undef yylex
#endif

int DDIFlexLexer::yylex()
{
    std::cerr << "in DDIFlexLexer::yylex() !" << std::endl;
    return 0;
}

/* When the scanner receives an end-of-file indication from YY_INPUT, it then
 * checks the yywrap() function. If yywrap() returns false (zero), then it is
 * assumed that the function has gone ahead and set up `yyin' to point to
 * another input file, and scanning continues. If it returns true (non-zero),
 * then the scanner terminates, returning 0 to its caller. */

int DDIFlexLexer::yywrap()
{
    return 1;
}
