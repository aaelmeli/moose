%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

%}

/*** yacc/bison Declarations ***/

%code requires{
#include <memory>
#include "waspcore/utils.h"
#include "waspcore/decl.h"
}

%output "DDIParser.cpp"
/* add debug output code to generated parser. disable this for release
 * versions. */
 /*%debug*/

/* start symbol is named "start" */
%start start

/* write out a header file containing the token defines */
%defines
%require "3.7"
/* use newer C++ skeleton file */
%skeleton "lalr1.cc"

/* namespace to enclose parser in */
/* %name-prefix "wasp" */
%define api.namespace {wasp}
%define api.location.file "../waspcore/location.hh"

/* set the parser's class identifier */
%define api.parser.class {DDIParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<DDILexerImpl>(interpreter,&input_stream);
};

/* The interpreter is passed by reference to the parser and to the DDILexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class DDILexerImpl> lexer}

/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the wasp grammar's tokens below ***/

%union {
        std::size_t token_index;
        std::size_t node_index;
        std::size_t stage_index;
        std::vector<size_t>* node_indices;
}

%token                  END          0  "end of file"
%token                  EOL             "end of line"

%token <token_index>   ASSIGN           "="
%token <token_index>   COMMA            ","

%token <token_index>    INTEGER         "integer"
%token <token_index>    DOUBLE          "double"
%token <token_index>    STRING          "string"
%token <token_index>    QSTRING          "quoted string"
%token <token_index>    COMMENT           "comment"


%type <token_index> PRIMITIVE ASSIGNMENT


%type <node_index>  assignment

%type <node_index>  decl
%type <node_index>  comma
%type <node_index>  value
%type <stage_index>  definition_section
%type <node_index>  comment

%type <node_indices>  value_list

%destructor { delete $$; } value_list

%{

#include "DDInterpreter.h"
#include "DDILexer.h"

#include "waspcore/wasp_bug.h"

/**
 * @brief adjust_interpreter_stages convenience method for committing staged parse trees
 * @param interpreter the interpreter to conduct the adjustments on
 * @param definition_name the name of the definition for which to adjust the interpreter's stage
 * @return true, iff the definition with the given name can be found as a direct child of
 * the current or ancenstral stage.
 */
bool adjust_interpreter_stages( wasp::AbstractInterpreter & interpreter
                               , const std::string & definition_name
                                , std::string & actual_name)
{
    wasp_check(interpreter.definition());
    int delta = interpreter.definition()->delta(definition_name, actual_name);
    if( -1 == delta )
    {
        return false;
    }
    else if( delta > 0 ){
        wasp_ensure( delta < static_cast<int>(interpreter.staged_count()) );
        while( delta > 0 ){
            interpreter.commit_staged(interpreter.staged_count()-1);
            --delta;
        }
    }
    return true;
}

/* this "connects" the bison parser in the interpreter to the flex DDILexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/

comma : COMMA
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_COMMA,",",token_index);
    }

PRIMITIVE :QSTRING | INTEGER | DOUBLE

value : PRIMITIVE
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}

decl : STRING
    {
        auto token_index = ($1);
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        $$ = interpreter.push_leaf(wasp::DECL
                                   ,"decl"
                                   ,token_index);
    }
ASSIGNMENT : ASSIGN
assignment : ASSIGNMENT {
             auto token_index = ($1);
             $$ = interpreter.push_leaf(wasp::ASSIGN,"=",token_index);
            }
value_list : value
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | value_list value
        {
            $$ = $1;
            $$->push_back($2);
        }
        | value_list comment
        {
            $$ = $1;
            $$->push_back($2);
        }
        | value_list comma
        {
            $$ = $1;
            $$->push_back($2);
        }
definition_section : decl  value_list
    {
        bool is_array = $2->size() > 1;
        $2->insert($2->begin(),$1);
        std::string quote_less_data = interpreter.data($1);
        quote_less_data = wasp::strip_quotes(quote_less_data);

        wasp_check(interpreter.definition());

        if( false == adjust_interpreter_stages(interpreter, quote_less_data
                                               , quote_less_data) )
        {
            error(@1, "'"+quote_less_data+"' is unknown.");
            interpreter.set_failed(true);
        }
        else{
        $$ = interpreter.push_staged(is_array ? wasp::ARRAY : wasp::KEYED_VALUE
                                     // use the data instead of the name
                                     // this provides the following tree
                                     // data
                                     //  |_ decl (data)
                                     //  |_ value (1.2..blah)
                                    ,quote_less_data.c_str()
                                    ,*$2);
        }
        delete $2;
    }
    | decl assignment value_list // keyed = value/values
    {
        bool is_array = $3->size() > 1;
        $3->insert($3->begin(),$2);
        $3->insert($3->begin(),$1);

        std::string quote_less_data = interpreter.data($1);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        if( false == adjust_interpreter_stages(interpreter, quote_less_data
                                               , quote_less_data) )
        {
            error(@1, "'"+quote_less_data+"' is unknown.");
            interpreter.set_failed(true);
        }
else{
        $$ = interpreter.push_staged(is_array ? wasp::ARRAY : wasp::KEYED_VALUE
                                     // use the data instead of the name
                                     // this provides the following tree
                                     // data
                                     //  |_ decl (data)
                                     //  |_ = (=)
                                     //  |_ value (1.2..blah)
                                    ,quote_less_data.c_str()
                                    ,*$3);
}
        delete $3;
    }
    | decl {

        std::string quote_less_data = interpreter.data($1);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        if( false == adjust_interpreter_stages(interpreter, quote_less_data
                                               , quote_less_data) )
        {
            error(@1, "'"+quote_less_data+"' is unknown.");
            interpreter.set_failed(true);
        }
        else{
        std::vector<size_t> child_indices = {$decl};
        $$ = interpreter.push_staged(wasp::OBJECT
                                    ,quote_less_data.c_str()
                                    ,child_indices);
        }
    }
    | decl comma {

        std::string quote_less_data = interpreter.data($1);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        if( false == adjust_interpreter_stages(interpreter, quote_less_data
                                               , quote_less_data) )
        {
            error(@1, "'"+quote_less_data+"' is unknown.");
            interpreter.set_failed(true);
        }
        else{
            std::vector<size_t> child_indices = {$decl,$comma};
            $$ = interpreter.push_staged(wasp::OBJECT
                                        ,quote_less_data.c_str()
                                    ,child_indices);
        }
    }


comment : COMMENT
        {
            auto token_index = ($1);
            $$ = interpreter.push_leaf(wasp::COMMENT,"comment",token_index);
        }

start   : /** empty **/
        | start comment{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start definition_section{
            if(interpreter.single_parse() )
            {
                lexer->rewind();
                YYACCEPT;
            }
        }



 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

void wasp::DDIParser::error(const DDIParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
