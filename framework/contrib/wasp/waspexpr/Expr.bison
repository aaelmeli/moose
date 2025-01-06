%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <vector>
%}

/*** yacc/bison Declarations ***/

%code requires{
#include <memory>
#include "waspcore/TreeNodePool.h"
#include "waspcore/wasp_node.h"
#include "waspcore/decl.h"
}

/* Require biExpr 3 or later */

%output "ExprParser.cpp"
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
%define api.parser.class {ExprParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<ExprLexerImpl>(interpreter,&input_stream);
};

/* The interpreter is passed by reference to the parser and to the ExprLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class ExprLexerImpl> lexer}
/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the Expr grammar's tokens below ***/

//%define api.value.type {struct YYSTYPE}
%union{
    std::size_t token_index;
    std::size_t node_index;
    std::vector<size_t>* node_indices;
}


%token                  END          0  "end of expression"
%token                  EOL             "end of line"
%token <token_index>   LBRACKET         "["
%token <token_index>   RBRACKET         "]"
%token                 '{'
%token                 '}'
%token <token_index>   BANG             "!"
%token <token_index>   ASSIGN           "="
%token <token_index>   COMMA            ","
%token <token_index>    GTE             ">="
%token <token_index>    LTE             "<="
%token <token_index>    LT             "<"
%token <token_index>    GT             ">"
%token <token_index>    NEQ             "!="
%token <token_index>    EQ              "=="
%token <token_index>    AND             "&&"
%token <token_index>    OR              "||"
%token <token_index>   MULTIPLY          "*"
%token <token_index>   DIVIDE           "/"
%token <token_index>   PLUS             "+"
%token <token_index>   MINUS            "-"
%token <token_index>   EXPONENT         "^"
%token <token_index>   LPAREN           "("
%token <token_index>   RPAREN           ")"
%token <token_index>   INTEGER         "integer"
%token <token_index>   REAL          "real"
%token <token_index>   STRING          "string"
%token <token_index>   QSTRING          "quoted string"
%token <token_index>   COMMENT           "comment"

%type <token_index>   DECL "declarator"
%type <token_index>   VALUE "value"
%type <node_index>  integer real string
%type <node_index>  lbracket rbracket
%type <node_index>  assign comma
%type <node_index>  gt lt gte lte neq eq and or multiply divide plus minus
%type <node_index>  exponent lparen rparen bang
%type <node_index>  function function_name
//%type <token_index>   NUMERIC_VALUE "numeric value"
//%type <node_index>  numeric_value string_value
%type <node_index>  unquoted_string
%type <node_index>  primitive math_exp
%type <node_index>  keyedvalue
%type <node_index>  value decl
%type <node_indices> function_args



%{

#include "ExprInterpreter.h"
#include "ExprLexer.h"

/* this "connects" the Expr parser in the interpreter to the flex ExprLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the Expr grammar rules below ***/

bang     : BANG     {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::BANG      ,"!",token_index);}
gt       : GT       {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::GT      ,">",token_index);}
lt       : LT       {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::LT      ,"<",token_index);}
gte      : GTE      {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::GTE      ,">=",token_index);}
lte      : LTE      {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::LTE      ,"<=",token_index);}
neq      : NEQ      {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::NEQ      ,"!=",token_index);}
eq       : EQ       {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::EQ       ,"==",token_index);}
and      : AND      {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::WASP_AND      ,"&&",token_index);}
or       : OR       {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::WASP_OR       ,"||",token_index);}
multiply : MULTIPLY {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::MULTIPLY , "*",token_index);}
divide   : DIVIDE   {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::DIVIDE   ,"/" ,token_index);}
plus     : PLUS     {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::PLUS     ,"+" ,token_index);}
minus    : MINUS    {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::MINUS    ,"-" ,token_index);}
exponent : EXPONENT {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::EXPONENT ,"^" ,token_index);}
lparen   : LPAREN   {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::LPAREN   ,"(" ,token_index);}
rparen   : RPAREN   {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::RPAREN   ,")" ,token_index);}
comma    : COMMA    {size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::WASP_COMMA    ,"," ,token_index);}

%left AND OR ;
%left LT GT LTE GTE EQ NEQ ;
%left PLUS MINUS;
%left MULTIPLY DIVIDE;
%left UMINUS UNOT ;
%right EXPONENT;

function_name : DECL {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DECL,"name"
                         ,token_index);
    }
function_args : /*EMPTY*/ { $$ = nullptr;}
    | math_exp
    {
        size_t node_index = ($1);
        $$ = new std::vector<size_t>();
        $$->push_back(node_index);
    }
    | function_args comma math_exp
    {
        if( $1 == nullptr )
        {
            $$ = new std::vector<size_t>();
        }
        $$->push_back($2);
        $$->push_back($3);
    }

%nonassoc "constant" "name";
// TODO add array indices
function : value %prec "constant"
    |  function_name lparen function_args rparen %prec "name"
    {
        size_t name_index = ($1);
        size_t left_index = ($2);
        size_t right_index = ($4);
        std::vector<size_t> child_indices = {name_index
                                                   ,left_index};
        if( $function_args != nullptr )
        {
            child_indices.reserve(child_indices.size()+$function_args->size());
            for( std::size_t i = 0; i < $function_args->size(); ++i)
            {
                child_indices.push_back($function_args->at(i));
            }
            delete $function_args;
        }
        child_indices.push_back(right_index);
        const std::string & name = interpreter.data(name_index);
        $$ = interpreter.push_parent(wasp::FUNCTION
                                        ,name.c_str()
                                        ,child_indices);
     }
     | function_name lbracket math_exp rbracket
     {
         std::vector<size_t> child_indices = {$1,$2,$3,$4};

         const std::string& name = wasp::strip_quotes(interpreter.data($1));
         $$ = interpreter.push_parent(wasp::OBJECT
                                         , name.c_str()
                                         , child_indices);
     }
     | function_name lbracket math_exp rbracket assign math_exp
     {
         std::vector<size_t> child_indices = {$1,$2,$3,$4,$5,$6};

         const std::string & name = wasp::strip_quotes(interpreter.data($1));
         $$ = interpreter.push_parent(wasp::OBJECT
                                         , name.c_str()
                                         , child_indices);
     }
math_exp : function // constant value or function( cos(), sin())
    | minus math_exp  %prec UMINUS
    {
        size_t left_index = ($1);
        size_t op_index = ($2);
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index};
        $$ = interpreter.push_parent(wasp::UNARY_MINUS
                                        ,"exp"
                                        ,child_indices);
    }
   | bang math_exp %prec UNOT
   {
       size_t left_index = ($1);
       size_t op_index = ($2);
       std::vector<size_t> child_indices = {left_index
                                                  ,op_index};
       $$ = interpreter.push_parent(wasp::UNARY_NOT
                                       ,"exp"
                                       ,child_indices);
    }
    | lparen math_exp rparen
    {
        size_t left_index = ($1);
        size_t op_index = ($2);
        size_t right_index = ($3);
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index
                                                   ,right_index};
        $$ = interpreter.push_parent(wasp::PARENTHESIS
                                        ,"exp"
                                        ,child_indices);
    }
    | math_exp multiply math_exp %prec MULTIPLY
    {
        size_t left_index = ($1);
        size_t op_index = ($2);
        size_t right_index = ($3);
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index
                                                   ,right_index};
        $$ = interpreter.push_parent(wasp::MULTIPLY
                                        ,"exp"
                                        ,child_indices);
    }
   | math_exp divide math_exp %prec DIVIDE
   {
       size_t left_index = ($1);
       size_t op_index = ($2);
       size_t right_index = ($3);
       std::vector<size_t> child_indices = {left_index
                                                  ,op_index
                                                  ,right_index};
       $$ = interpreter.push_parent(wasp::DIVIDE
                                       ,"exp"
                                       ,child_indices);
   }
   | math_exp plus math_exp %prec PLUS
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::PLUS
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp minus math_exp %prec MINUS
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::MINUS
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp exponent math_exp %prec EXPONENT
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::EXPONENT
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp eq math_exp %prec EQ
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::EQ
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp neq math_exp %prec NEQ
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::NEQ
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp and math_exp %prec AND
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::WASP_AND
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp or math_exp %prec OR
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::WASP_OR
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp gte math_exp %prec GTE
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::GTE
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp gt math_exp %prec GT
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::GT
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp lte math_exp %prec LTE
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::LTE
                                      ,"exp"
                                      ,child_indices);
   }
   | math_exp lt math_exp %prec LT
   {
      size_t left_index = ($1);
      size_t op_index = ($2);
      size_t right_index = ($3);
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      $$ = interpreter.push_parent(wasp::LT
                                      ,"exp"
                                      ,child_indices);
   }
assign : ASSIGN
    {
        size_t assign_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::ASSIGN,"="
                         ,assign_token_index);
    }

lbracket : LBRACKET
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::LBRACKET,"["
                         ,token_index);
    }
rbracket : RBRACKET
    {

        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::RBRACKET,"]"
                         ,token_index);
    }

integer : INTEGER
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::INTEGER,"int"
                         ,token_index);
    }
real : REAL
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::REAL,"real"
                         ,token_index);
    }
unquoted_string : STRING
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::STRING,"str"
                         ,token_index);
    }
VALUE : INTEGER | REAL | STRING | QSTRING
value : VALUE
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::VALUE,"value"
                         ,token_index);
    }
DECL : STRING | QSTRING
decl : STRING
    {
        size_t decl_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DECL,"decl"
                         ,decl_token_index);
    }
string : unquoted_string

keyedvalue : decl assign math_exp
    {

        size_t key_index = ($1);
        size_t assign_index = ($2);
        size_t value_index = ($3);

        std::vector<size_t> child_indices = {key_index, assign_index,value_index};

        $$ = interpreter.push_parent(wasp::KEYED_VALUE
                                        ,interpreter.data(key_index).c_str()
                                        ,child_indices);
    }


start   : /** empty **/
        | keyedvalue{
            interpreter.push_staged_child($1);
        }
        | math_exp{
            interpreter.push_staged_child($1);
        }


 /*** END RULES - Change the Expr grammar rules above ***/

%% /*** Additional Code ***/
namespace wasp{
void ExprParser::error(const ExprParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<"***Error : "<<l<<": "<<m<<"."<<std::endl;
}
} // end of namespace
