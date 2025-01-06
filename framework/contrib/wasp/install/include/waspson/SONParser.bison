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

%output "SONParser.cpp"
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
%define api.parser.class {SONParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<SONLexerImpl>(interpreter,&input_stream);
};

/* The interpreter is passed by reference to the parser and to the SONLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class SONLexerImpl> lexer}

/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the wasp grammar's tokens below ***/

%union {
        std::size_t token_index;
        std::size_t node_index;
        std::vector<size_t>* node_indices;
}

%token                  END          0  "end of file"
%token                  EOL             "end of line"
//%token <token_index>   TICK             '`'
%token <token_index>   MINUS            "-"
%token <token_index>   LPAREN           "("
%token <token_index>   RPAREN           ")"
%token <token_index>   LBRACKET         "["
%token <token_index>   RBRACKET         "]"
%token <token_index>   LBRACE           "{"
%token <token_index>   RBRACE           "}"
%token <token_index>   ASSIGN           "="
%token <token_index>   COMMA            ","
%token <token_index>   COLON            ":"
%token <token_index>    GTE             ">="
%token <token_index>    LT             "<"
%token <token_index>    GT             ">"
%token <token_index>    LTE             "<="
%token <token_index>    NEQ             "!="
%token <token_index>    EQ              "=="
%token <token_index>    AND             "&&"
%token <token_index>    OR              "||"
%token <token_index>    BANG            "!"
%token <token_index>    FILLER          "filler"
%token <token_index>    MULTIPLY        "*"
%token <token_index>    DIVIDE          "/"
%token <token_index>    PLUS            "+"
%token <token_index>    EXPONENT        "^"
%token <token_index>    AMPERSAND       "&"
%token <token_index>    INTEGER         "integer"
%token <token_index>    TOKEN_TRUE            "true"
%token <token_index>    TOKEN_FALSE            "false"
%token <token_index>    DOUBLE          "double"
%token <token_index>    STRING          "string"
%token <token_index>    QSTRING          "quoted string"
%token <token_index>    COMMENT           "comment"
%token <token_index>    FILE              "file import"
%token <token_index>    EXECUTION_UNIT_START  "start of unit of execution"
%token <token_index>    EXECUTION_UNIT_END  "end of unit of execution"



%type <token_index> PRIMITIVE ASSIGNMENT ANY_STRING
%type <token_index> BOOLEAN

%type <node_index>  assignment
%type <node_index>  eq neq gt lt gte lte and or unary_not
%type <node_index>  multiply_divide boolean_numeric_op boolean_logic_op
%type <node_index>  lparen rparen plus minus multiply divide exponent
%type <node_index>  lbracket rbracket
%type <node_index>  lbrace rbrace
%type <node_index>  array 
%type <node_index>  block
%type <node_index>  comma
%type <node_index>  decl
%type <node_index>  import path import_file
%type <node_index>  exp value
%type <node_index>  filler
%type <node_index>  identifier
%type <node_index>  object
%type <node_index>  keyedvalue
%type <node_index>  comment 
%type <node_index>  component execution_unit_end execution_unit_start execution_unit_name

%type <node_index> execution_unit

%type <node_indices>  members declaration
%type <node_indices>  array_members tag key_declaration
%destructor { delete $$; } tag array_members members
%destructor { delete $$; } declaration key_declaration
%{

#include "SONInterpreter.h"
#include "SONLexer.h"

/* this "connects" the bison parser in the interpreter to the flex SONLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/
filler : FILLER
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::FILL_EXPR,"filler",token_index);
    }
comma : COMMA
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_COMMA,",",token_index);
    }
//tick :  TICK
//    {
//        auto token_index = ($1);
//        $$ = interpreter.push_leaf(wasp::WASP_TICK,"`",token_index);
//    }
multiply :  MULTIPLY
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::MULTIPLY,"*",token_index);
    }
divide :  DIVIDE
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DIVIDE,"/",token_index);
    }
multiply_divide : multiply | divide
plus :  PLUS
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::PLUS,"+",token_index);
    }
minus :  MINUS
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::MINUS,"-",token_index);
    }

exponent :  EXPONENT
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::EXPONENT,"^",token_index);
    }
BOOLEAN : TOKEN_TRUE | TOKEN_FALSE

boolean_numeric_op : eq | neq | gt | lt | gte | lte

eq : EQ
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::EQ,"==",token_index);
    }
neq : NEQ
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::NEQ,"!=",token_index);
    }
gte: GTE
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::GTE,">=",token_index);
    }
gt: GT
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::GT,">",token_index);
    }
lte: LTE
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LTE,"<=",token_index);
    }
lt: LT
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LT,"<",token_index);
    }
boolean_logic_op : and | or
and: AND
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_AND,"&&",token_index);
    }
or: OR
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_OR,"||",token_index);
    }

unary_not : BANG
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::UNARY_NOT,"!",token_index);
    }

lparen  : LPAREN
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LPAREN,"(",token_index);
    }
rparen  : RPAREN
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RPAREN,")",token_index);
    }
lbrace  : LBRACE
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACE,"{",token_index);
    }
rbrace  : RBRACE
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACE,"}",token_index);
    }
lbracket  : LBRACKET
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACKET,"[",token_index);
    }
rbracket  : RBRACKET
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACKET,"]",token_index);
    }

ANY_STRING : STRING | QSTRING
PRIMITIVE : STRING | QSTRING | INTEGER | DOUBLE | BOOLEAN

value : PRIMITIVE
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
%right OBJECT ARRAY;
%right ASSIGN;
%left  COMMA;
%left AND OR ;
%left LT GT LTE GTE EQ NEQ ;
%left PLUS MINUS;
%left MULTIPLY DIVIDE;
%left UMINUS UNOT ;

%right EXPONENT;


component : value

exp : component
    |exp multiply_divide exp %prec MULTIPLY
    {
          size_t left_i = ($1);
          size_t op_i = ($2);
          size_t right_i = ($3);
          std::vector<size_t> child_indices = {left_i
                                                     ,op_i
                                                     ,right_i
                                                     };

          $$ = interpreter.push_parent(interpreter.type(op_i)
                                          ,"value"
                                          ,child_indices);
    }
    | exp plus exp %prec PLUS
    {
        size_t left_i = ($1);
        size_t op_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
    | exp minus exp %prec MINUS
    {
        size_t left_i = ($1);
        size_t op_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
    | exp exponent exp %prec EXPONENT
    {
        size_t left_i = ($1);
        size_t op_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
    | exp boolean_numeric_op exp %prec EQ
    {
        size_t left_i = ($1);
        size_t op_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }

    | exp boolean_logic_op exp %prec AND
    {
        size_t left_i = ($1);
        size_t op_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
    | lparen exp rparen %prec "exp"
    {
        size_t left_i = ($1);
        size_t center_i = ($2);
        size_t right_i = ($3);
        std::vector<size_t> child_indices = {left_i
                                                   ,center_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(wasp::PARENTHESIS
                                        ,"value"
                                        ,child_indices);
    }
    | minus exp %prec UMINUS
    {
        size_t op_i = ($1);
        size_t right_i = ($2);
        std::vector<size_t> child_indices = {op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
    | unary_not exp %prec UNOT
    {
        size_t op_i = ($1);
        size_t right_i = ($2);
        std::vector<size_t> child_indices = {op_i
                                                   ,right_i
                                                   };

        $$ = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }

execution_unit_end : EXECUTION_UNIT_END
{
    auto token_index = ($1);
    $$ = interpreter.push_leaf(wasp::EXECUTION_UNIT_END,"uoe_end",token_index);
}
execution_unit_start : EXECUTION_UNIT_START
{
    auto token_index = ($1);
    $$ = interpreter.push_leaf(wasp::EXECUTION_UNIT_START,"uoe_start",token_index);
}

import : FILE
    {
        auto token_index = $1;
        $$ = interpreter.push_leaf(wasp::FILE,"decl",token_index);
    }
path :  QSTRING
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::VALUE, "path", token_index);
    }

import_file : import lparen path rparen
    {

        std::vector<size_t> child_indices = {$1,$2, $3, $4};
        $$ = interpreter.push_parent(wasp::FILE
                                        // import
                                        //  |_ decl (`import)
                                        //  |_ lparen ('(')
                                        //  |_ value (path/to/file)
                                        //  |_ rparen (')')
                                    ,"import"
                                    ,child_indices);
        bool loaded = interpreter.load_document($$, wasp::trim(interpreter.data($3)," "));
        if (!loaded)
        {
            interpreter.set_failed(true);
        }
    }

identifier : exp
    {
        interpreter.set_type($exp, wasp::IDENTIFIER);
        interpreter.set_name($exp, "id");
        $$ = $exp;
    }
decl : PRIMITIVE
    {
        auto token_index = ($1);
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        $$ = interpreter.push_leaf(wasp::DECL
                                   ,"decl"
                                   ,token_index);
    }
ASSIGNMENT : ASSIGN | COLON
assignment : ASSIGNMENT {
             auto token_index = ($1);
             $$ = interpreter.push_leaf(wasp::ASSIGN,"=",token_index);
            }
key_declaration : tag assignment
        {
             $$ = $1;
             $$->push_back($2);

        }
declaration : key_declaration | tag
tag :   decl
        {
             $$ = new std::vector<size_t>();
             $$->push_back($decl);
        }
        | decl lparen identifier rparen
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
            $$->push_back($2);
            $$->push_back($3);
            $$->push_back($4);
        }
array :
    declaration lbracket rbracket
    {
        $1->push_back($2);
        $1->push_back($3);
        $$ = interpreter.push_parent(wasp::ARRAY
                ,wasp::strip_quotes(interpreter.data($1->front())).c_str()
                ,*$1);
        delete $1;
    }
    | declaration lbracket END
    {
        $1->push_back($2);
        std::string name = interpreter.name($1->front());
        $$ = interpreter.push_parent(wasp::ARRAY
                                    , name.c_str()
                                        ,*$1);
        delete $1;
        error(@2, name+" has unmatched left bracket!");
        YYERROR;
    }
    | declaration lbracket array_members END
    {
        std::string name = interpreter.name($1->front());
        auto last_component_type = interpreter.type($1->back());
        // TODO capture partial definition
        if( $3->size() ==0 ) error(@2, name+" has unmatched left bracket!");
        else if( last_component_type == wasp::ARRAY ) error(@2, name+" or one of its components has unmatched left bracket!");
        else error(@2, name+" has unmatched left bracket!");

        delete $1;
        YYERROR;
    }
    | declaration lbracket execution_unit_end
    {
        // TODO capture partial definition
        std::string name = interpreter.name($1->front());
        error(@2, name+" has unmatched left bracket!");
        delete $1;
        YYERROR;
    }
    | declaration lbracket array_members execution_unit_end
    {
        std::string name = interpreter.name($1->front());
        auto last_component_type = interpreter.type($1->back());
        // TODO capture partial definition
        if( $3->size() ==0 ) error(@2, name+" has unmatched left bracket!");
        else if( last_component_type == wasp::ARRAY ) error(@2, name+" or one of its components has unmatched left bracket!");
        else error(@2, name+" has unmatched left bracket!");

        delete $3;
        delete $1;
        YYERROR;
    }
    |declaration lbracket array_members rbracket
    {
        $1->push_back($2);
        for( std::size_t i = 0; i < $3->size(); ++i )
        {
            $1->push_back( $3->at( i ) );
        }
        $1->push_back($4);
        $$ = interpreter.push_parent(wasp::ARRAY
            ,wasp::strip_quotes(interpreter.data($1->front())).c_str()
            ,*$1);
        delete $1;
        delete $3;

    }
object : declaration lbrace rbrace
    {
        $1->push_back($2);
        $1->push_back($3);
        $$ = interpreter.push_parent(wasp::OBJECT
            ,wasp::strip_quotes(interpreter.data($1->front())).c_str()
            ,*$1);
        delete $1;
    }
    | declaration lbrace END
    {
        $1->push_back($2);
        std::string name = interpreter.data($1->front());
        $$ = interpreter.push_parent(wasp::OBJECT
                                     , name.c_str()
                                     , *$1);
        delete $1;
        error(@2, name+" has unmatched left brace!");
        YYERROR;
    }
    | declaration lbrace members END
    {
        // TODO capture partial definition
        std::string name = interpreter.data($1->front());
        auto last_component_type = interpreter.type($1->back());
        if( $3->size() ==0 ) error(@2, name+" has unmatched left brace!");
        else if( last_component_type == wasp::OBJECT ) error(@2, name+" or one of its components has unmatched left brace!");
        else error(@2, name+" has unmatched left brace!");
        delete $3;
        delete $1;
        YYERROR;
    }
    | declaration lbrace execution_unit_end
    {
        $1->push_back($2);
        std::string name = interpreter.data($1->front());
        $$ = interpreter.push_parent( wasp::OBJECT
                                     , name.c_str()
                                     , *$1);
        error(@2, name+" has unmatched left brace!");
        delete $1;
        YYERROR;
    }
    | declaration lbrace members execution_unit_end
    {
        // TODO capture partial definition
        std::string name = interpreter.data($1->front());
        auto last_component_type = interpreter.type($1->back());
        if( $3->size() ==0 ) error(@2, name+" has unmatched left brace!");
        else if( last_component_type == wasp::OBJECT ) error(@2, name+" or one of its components has unmatched left brace!");
        else error(@2, name+" has unmatched left brace!");

        delete $3;
        delete $1;
        YYERROR;
    }
    | declaration lbrace members rbrace
    {
        $1->push_back($2);
        for( std::size_t i = 0; i < $3->size(); ++i )
        {
            $1->push_back( $3->at( i ) );
        }
        $1->push_back($4);
        $$ = interpreter.push_parent(wasp::OBJECT
                                    ,wasp::strip_quotes(interpreter.data($1->front())).c_str()
                                        ,*$1);
        delete $1;
        delete $3;
    }


keyedvalue : key_declaration exp
    {
        $1->push_back($2);
        std::string quote_less_data = interpreter.data($1->front());
        quote_less_data = wasp::strip_quotes(quote_less_data);
        $$ = interpreter.push_parent(wasp::KEYED_VALUE
                                     // use the data instead of the name
                                     // this provides the following tree
                                     // data
                                     //  |_ decl (data)
                                     //  |_ = (=)
                                     //  |_ value (1.2..blah)
                                    ,quote_less_data.c_str()
                                    ,*$1);
        delete $1;
    }

members :
        filler
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members filler
        {
            $$ = $1;
            $$->push_back($2);
        }
        |object
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members object
        {
            $$ = $1;
            $$->push_back($2);
        }
        | array
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members array
        {
            $$ = $1;
            $$->push_back($2);
        }
        | keyedvalue
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members keyedvalue
        {
            $$ = $1;
            $$->push_back($2);
        }
        | comment
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members comment
        {
            $$ = $1;
            $$->push_back($2);
        }
        | members comma
        {
            $$ = $1;
            $$->push_back($2);
        }
        | import_file
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | members import_file
        {
            $$ = $1;
            $$->push_back($2);
        }

array_members :
        exp
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members exp
        {
            $$ = $1;
            $$->push_back($2);
        }
        |
        filler
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members filler
        {
            $$ = $1;
            $$->push_back($2);
        }
        |object
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members object
        {
            $$ = $1;
            $$->push_back($2);
        }
        | array
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members array
        {
            $$ = $1;
            $$->push_back($2);
        }
        | keyedvalue
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members keyedvalue
        {
            $$ = $1;
            $$->push_back($2);
        }
        | comment
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members comment
        {
            $$ = $1;
            $$->push_back($2);
        }
        | array_members comma
        {
            $$ = $1;
            $$->push_back($2);
        }
        | import_file
        {
            $$ = new std::vector<size_t>();
            $$->push_back($1);
        }
        | array_members import_file
        {
            $$ = $1;
            $$->push_back($2);
        }
comment : COMMENT
        {
            auto token_index = ($1);
            $$ = interpreter.push_leaf(wasp::COMMENT,"comment",token_index);
        }
execution_unit_name : ANY_STRING
        {
            auto token_index = ($1);
            $$ = interpreter.push_leaf(wasp::DECL,"name",token_index);
        }
execution_unit : execution_unit_start execution_unit_name execution_unit_end
       { // account for an empty execution unit
            size_t start_i = ($1);
            size_t name_i = ($2);
            size_t end_i = ($3);
            std::vector<size_t> child_indices = {start_i
                                                       ,name_i
                                                       ,end_i
                                                       };

            $$ = interpreter.push_parent(wasp::EXECUTION_UNIT
                        ,wasp::strip_quotes(interpreter.data(name_i)).c_str()
                        ,child_indices);
       }
      | execution_unit_start execution_unit_name members execution_unit_end
      { // execution unit with members
            size_t start_i = ($1);
            size_t name_i = ($2);
            size_t end_i = ($4);
            std::vector<size_t> child_indices = {start_i
                                                       ,name_i
                                                       };
            for( std::size_t i = 0; i < $members->size(); ++ i )
            {
                child_indices.push_back( $members->at(i) );
            }
            child_indices.push_back(end_i);
            delete $members;
            $$ = interpreter.push_parent(wasp::EXECUTION_UNIT
                        ,wasp::strip_quotes(interpreter.data(name_i)).c_str()
                        ,child_indices);
       }
 // Blocks are a terminator-less construct, which pushes a stage/scope after which all
 // SON constructs (except execution_unit) are children
block : lbracket decl rbracket
      {
          // Check for prior staged blocks that may need to be committed.
          // Because blocks are the only SON construct that are staged (in addition to the root)
          // the stage count should be 1, if it is greater, the additional stages should be closed
          if (interpreter.staged_count() > 1)
          {
              // blocks cannot be nested, so there should only be 1 additional stage
              // root+prior block = 2 stages
              wasp_check(interpreter.staged_count() == 2);
              interpreter.commit_staged(interpreter.staged_count()-1);
          }

          size_t start_i = ($1);
          size_t name_i = ($2);
          size_t end_i = ($3);
          std::vector<size_t> child_indices = {start_i
                                                     ,name_i
                                                     ,end_i
                                                     };

          std::string quote_less_data = interpreter.data($2);
          quote_less_data = wasp::strip_quotes(quote_less_data);
          $$ = interpreter.push_staged(wasp::OBJECT
                                    ,quote_less_data.c_str()
                                    ,child_indices);

      }
start   : /** empty **/
        | start comment{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start array{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start keyedvalue{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start object{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}
        | start execution_unit{interpreter.push_staged_child(($2));if(interpreter.single_parse() ) {YYACCEPT;}}
        | start block{ /*Node staging occurs in block rule*/;if(interpreter.single_parse() ) {YYACCEPT;}}
        | start import_file{interpreter.push_staged_child(($2)); if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}}

 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

void wasp::SONParser::error(const SONParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
