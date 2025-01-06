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

%output "EDDIParser.cpp"
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
%define api.parser.class {EDDIParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<EDDILexerImpl>(interpreter,&input_stream);
};

/* The interpreter is passed by reference to the parser and to the EDDILexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class EDDILexerImpl> lexer}

/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the wasp grammar's tokens below ***/

%union {
        std::size_t token_index;
        std::size_t node_index;
        std::size_t stage_index;
}

%token                  END          0  "end of file"
%token                  EOL             "end of line"

%token <token_index>   MINUS            "-"
%token <token_index>   ASSIGN           "="
%token <token_index>   WASP_COMMA           ","
%token <token_index>   SEMICOLON            ";"

%token <token_index>    INTEGER         "integer"
%token <token_index>    DOUBLE          "double"
%token <token_index>    STRING          "string"
%token <token_index>    QSTRING          "quoted string"
%token <token_index>    COMMENT           "comment"
%token <token_index>    RBRACKET           "right bracket"
%token <token_index>    LBRACKET           "left bracket"
%token <token_index>    FSLASH             "forward slash"
%token <token_index>    FILE              "file include"
%token <token_index>    FILL_EXPR         "list expression"

%type <token_index> PRIMITIVE

%type <node_index>  decl lbracket rbracket fslash assign
%type <node_index>  comma semicolon fill_expr
%type <node_index>  value key_value part path decl_or_key_value
%type <stage_index>  command_part block
%type <node_index>  comment include include_file

%{

#include "EDDInterpreter.h"
#include "EDDINodeView.h"
#include "EDDILexer.h"

#include "waspcore/wasp_bug.h"

/* this "connects" the bison parser in the interpreter to the flex EDDILexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/
fill_expr : FILL_EXPR
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::FILL_EXPR,"value",token_index);
        }
include : FILE
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::FILE,"decl",token_index);
        }
comma : WASP_COMMA
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::WASP_COMMA,",",token_index);
        }
semicolon : SEMICOLON
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::TERM,";",token_index);
        }
assign : ASSIGN
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::ASSIGN,"=",token_index);
        }

fslash : FSLASH
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DIVIDE,"/",token_index);
    }
rbracket : RBRACKET
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACKET,"]",token_index);
    }
lbracket : LBRACKET
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACKET,"[",token_index);
    }

PRIMITIVE : QSTRING | INTEGER | DOUBLE | MINUS

value : PRIMITIVE
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
key_value : value
        | STRING
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}

path : STRING
        {
            size_t token_index = ($1);
            $$ = interpreter.push_leaf(wasp::VALUE,"path"
                             ,token_index);
        }
        | QSTRING
        {
            size_t token_index = ($1);
            $$ = interpreter.push_leaf(wasp::VALUE,"path"
                             ,token_index);
        }

include_file : include path
        {

            std::vector<size_t> child_indices = {$1,$2};
            $$ = interpreter.push_parent(wasp::FILE
                                         // include
                                         //  |_ decl (include)
                                         //  |_ value (path/to/file)
                                        ,"incl"
                                        ,child_indices);
            bool loaded = interpreter.load_document($$, wasp::trim(interpreter.data($2)," "));
            if (!loaded)
            {
                interpreter.set_failed(true);
            }
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
decl_or_key_value :  decl assign key_value
        {
            std::string quote_less_data = interpreter.data($1);
            quote_less_data = wasp::strip_quotes(quote_less_data);
            std::vector<size_t> child_indices = {$1,$2,$3};
            $$ = interpreter.push_parent(wasp::KEYED_VALUE
                                         // use the data instead of the name
                                         // this provides the following tree
                                         // data
                                         //  |_ decl (data)
                                         //  |_ = (=)
                                         //  |_ value (1.2..blah)
                                        ,quote_less_data.c_str()
                                        ,child_indices);
        } | decl
part : value | fslash | comment | fill_expr
        | decl_or_key_value | comma | semicolon 

command_part : part {
        std::ostringstream err;
        if (!interpreter.process_staged_node($$, "command_part",
                                            $1, @1, err))
        {
            error(@1, err.str());
        }
    }
    | include_file
    {
        // assume the included content will be a child of the existing
        // staged content.
        $$ = interpreter.push_staged_child($1);
    }

comment : COMMENT
        {
            auto token_index = ($1);
            $$ = interpreter.push_leaf(wasp::COMMENT,"comment",token_index);
        }
block : lbracket decl rbracket
    {
        // Block is top level parse construct
        // It closes/commits existing stages
        while (interpreter.staged_count() > 1)
        {
            interpreter.commit_staged(interpreter.staged_count() - 1);
        }
        std::string data = interpreter.data($2);
        int delta = interpreter.definition()->delta(data, data);
        if( -1 == delta ) // no adjustment, not a command
        {
            error(@2, "'"+data+"' is unknown.");
            interpreter.set_failed(true);
        }
        else
        {
            std::vector<size_t> child_indices = {$lbracket, $decl, $rbracket};
            // top level blocks are objects
            $$ = interpreter.push_staged(wasp::OBJECT
                                        ,data.c_str()
                                        ,child_indices);
        }
    }

start   : /** empty files are not syntax errors **/
       | start block{
           if(interpreter.single_parse() )
           {
               lexer->rewind();
               YYACCEPT;
           }
       }
        | start command_part{
            if(interpreter.single_parse() )
            {
                lexer->rewind();
                YYACCEPT;
            }
        }



 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

void wasp::EDDIParser::error(const EDDIParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
