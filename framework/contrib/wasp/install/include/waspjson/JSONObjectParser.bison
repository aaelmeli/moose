%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>
%}

/*** yacc/bison Declarations ***/

%code requires{
#include <memory>
#include <utility>
#include "waspcore/utils.h"
#include "waspcore/Object.h"
#include "waspcore/decl.h"
}

/* Require bison 3 or later */

%output "JSONObjectParser.cpp"
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
%define api.parser.class {JSONObjectParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.line = @$.end.line = 1;
    @$.begin.column = @$.end.column = 1;
    lexer = std::make_shared<JSONObjectLexerImpl>(&input_stream);
};

/* The interpreter is passed by reference to the parser and to the JSONObjectLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param {std::shared_ptr<wasp::DataObject>& root}
             {std::istream &input_stream}
             {std::ostream &error_stream}
             {std::shared_ptr<class JSONObjectLexerImpl> lexer}

/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the wasp grammar's tokens below ***/

%union {
    bool boolean;
    int integer;
    double real;
    std::string* string;
    Value* value;
    std::vector<Value*>* values;
    std::vector<std::pair<std::string,Value*>>* keyed_values;
}

%token                  END          0  "end of file"
%token                  EOL             "end of line"
%token <integer>    INTEGER         "integer"
%token <boolean>    TOKEN_TRUE            "true"
%token <boolean>    TOKEN_FALSE            "false"
%token              TOKEN_NULL            "null"
%token <real>       DOUBLE          "double"
%token <string>     QSTRING          "quoted string"

%type <string>  decl

%type <value>  primitive object array
%type <keyed_values> keyed_member object_members
%type <values>   array_members
%destructor { delete $$; } decl primitive object array
%destructor {
    for(size_t i = 0; i < $$->size(); ++i)
        delete (*$$)[i];
    delete $$;
}  array_members
%destructor {
    for(size_t i = 0; i < $$->size(); ++i)
        delete (*$$)[i].second;
    delete $$;
} keyed_member object_members
%{

#include "JSONObjectLexer.h"

/* this "connects" the bison parser in the interpreter to the flex JSONObjectLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/

primitive : TOKEN_NULL
        {
            $$ = new Value();
            wasp_ensure($$->is_null());
        }| TOKEN_TRUE
        {
            $$ = new Value($1);
        }| TOKEN_FALSE
        {
            $$ = new Value($1);
        }| INTEGER
        {
            $$ = new Value($1);
        }| DOUBLE
        {
            $$ = new Value($1);
        }| QSTRING
        {
            $$ = new Value(wasp::json_unescape_string(wasp::strip_quotes(*$1)));
            delete $1;
        }
decl : QSTRING


array :
    '[' ']'
    {
        $$ = new Value(DataArray());
    }
    | '[' END
    {
        error(@2, "array has unmatched left bracket!");
        YYERROR;
        $$ = nullptr;
    }
    | '[' array_members END
    {
        error(@1, " is an unmatched left bracket!");
        for(size_t i = 0; i < $2->size(); ++i) delete $2->at(i);
        delete $2;
        YYERROR;
        $$ = nullptr;
    }
    | '[' array_members ']'
    {
        $$ = new Value();
        DataArray *array = new DataArray();
        array->resize($2->size());
        for( size_t i = 0; i < $2->size();++i)
        {
            // move resources
            (*array)[i] = std::move(*$2->at(i));
            delete $2->at(i); // deallocate empty Value
        }
        delete $2;
        $$->assign(array);
    }
object : '{' '}'
    {
        $$ = new Value(DataObject());
    }
    |  '{' END
    {
        error(@2, "is an unmatched left brace!");
        YYERROR;
        $$ = nullptr;
    }
    |  '{' object_members END
    {
        error(@1, "is an unmatched left brace!");
        for(size_t i = 0; i < $2->size(); ++i) delete $2->at(i).second;
        delete $2;
        YYERROR;
        $$ = nullptr;
    }
    | '{' object_members '}'
    {
        $$ = new Value();
        DataObject *object = new DataObject();
        $$->assign(object);
        for( size_t i = 0; i < $2->size();++i)
        {
            // move resources
            (*object)[$2->at(i).first] = std::move(*$2->at(i).second);
            delete $2->at(i).second; // deallocate empty Value
        }
        delete $2;

    }

array_members :object
        {
            $$ = new std::vector<Value*>();
            $$->push_back($1);
        }
        | array_members ',' object
        {
            $$ = $1;
            $$->push_back($3);
        }
        | array
        {
            $$ = new std::vector<Value*>();
            $$->push_back($1);
        }
        | array_members ',' array
        {
           $$ = $1;
           $$->push_back($3);
        }
        | primitive
        {
            $$ = new std::vector<Value*>();
            $$->push_back($1);
        }
        | array_members ',' primitive
        {
            $$ = $1;
            $$->push_back($3);
        }
keyed_member : decl ':' primitive
        {
            $$ = new std::vector<std::pair<std::string,Value*>>();
            $$->push_back(std::make_pair(wasp::strip_quotes(*$1),$3));
            delete $1;
        }|decl ':' array
        {
            $$ = new std::vector<std::pair<std::string,Value*>>();
            $$->push_back(std::make_pair(wasp::strip_quotes(*$1),$3));
            delete $1;
        }|decl ':' object
        {
            $$ = new std::vector<std::pair<std::string,Value*>>();
            $$->push_back(std::make_pair(wasp::strip_quotes(*$1),$3));
            delete $1;
        }
object_members : keyed_member
        | object_members ',' keyed_member
        {
            $$ = $1;
            $$->push_back($3->front());
            delete $3;
        }
start   : /** empty **/
        | object{
            wasp_check( $1->is_object() );
            root.reset($1->to_object());
            $1->assign((DataObject*)nullptr);
            delete $1;
        }

 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

void wasp::JSONObjectParser::error(const JSONObjectParser::location_type& l,
                           const std::string& m)
{
    error_stream<<l<<": "<<m<<std::endl;
}
