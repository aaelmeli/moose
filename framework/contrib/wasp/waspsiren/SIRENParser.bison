%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>
%}

/*** yacc/BISON Declarations ***/

%code requires{
#include <memory>
#include "waspcore/utils.h"
#include "waspcore/decl.h"
}

/* Require BISON 3 or later */

%output "SIRENParser.cpp"
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
%define api.namespace  {wasp}
%define api.location.file "../waspcore/location.hh"
/* %name-prefix "wasp" */

/* set the parser's class identifier */
%define api.parser.class {SIRENParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<SIRENLexerImpl>(interpreter,&input_stream);
    //    lexer->set_debug(true); // requires lexer '%option debug' enabled
};

/* The interpreter is passed by reference to the parser and to the SIRENLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class SIRENLexerImpl> lexer}

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
%token <token_index>   ANY            "//"
%token <token_index>   MINUS            "-"
%token <token_index>   LPAREN           "("
%token <token_index>   RPAREN           ")"
%token <token_index>   LBRACKET         "["
%token <token_index>   RBRACKET         "]"
%token <token_index>   LBRACE           "{"
%token <token_index>   RBRACE           "}"
%token <token_index>   COMMA            ","
%token <token_index>   COLON            ":"
%token <token_index>    GTE             ">="
%token <token_index>    LT             "<"
%token <token_index>    GT             ">"
%token <token_index>    LTE             "<="
%token <token_index>    NEQ             "!="
%token <token_index>    EQ              "="
%token <token_index>    AND             "&&"
%token <token_index>    PARENT          ".."
%token <token_index>    OR              "||"
%token <token_index>    BANG            "!"
%token <token_index>    MULTIPLY        "*"
%token <token_index>    DIVIDE          "division operator"
%token <token_index>    SEPARATOR       "path separator"
%token <token_index>    PLUS            "+"
%token <token_index>    EXPONENT        "^"
%token <token_index>    INTEGER         "integer"
%token <token_index>    DOUBLE          "double"
%token <token_index>    DECL            "decl"
%token <token_index>    QSTRING          "quoted string"



%type <token_index> PRIMITIVE ANY_STRING

%type <node_index>  eq neq gt lt gte lte and or unary_not
%type <node_index>  multiply_divide boolean_numeric_op boolean_logic_op
%type <node_index>  lparen rparen plus minus multiply divide exponent
%type <node_index>  lbracket rbracket
%type <node_index>  lbrace rbrace
%type <node_index>  decl
%type <node_index>  comma colon
%type <node_index>  exp value integer

%type <node_index>  keyedvalue
%type <node_index>  component
%type <node_index>  separator document_root
%type <node_index>  root_based_selection relative_selection any_selection
%type <node_index>  indices_selection parent_selection
%type <node_indices> tag key_declaration
%destructor { delete $$; } tag key_declaration
%{

#include "SIRENInterpreter.h"
#include "SIRENLexer.h"

/* this "connects" the BISON parser in the interpreter to the flex SIRENLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the wasp grammar rules below ***/
any_selection : ANY
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::ANY,"A",token_index);
    }
parent_selection : PARENT
    {
        auto token_index = ($1);
        // P - parent
        $$ = interpreter.push_leaf(wasp::PARENT,"P",token_index);
    }
comma : COMMA
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::WASP_COMMA,",",token_index);
    }
colon : COLON
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::COLON,":",token_index);
    }
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
 // indicates a step in the selection function
 // parent / child / grandchild
separator :  SEPARATOR
    {
        auto token_index = ($1);
        $$ = interpreter.push_leaf(wasp::SEPARATOR,"/",token_index);
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

ANY_STRING : DECL | QSTRING
PRIMITIVE : DECL | QSTRING | INTEGER | DOUBLE
integer : INTEGER {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::INTEGER,"int"
                         ,token_index);
    }
value : PRIMITIVE
{
    size_t token_index = ($1);
    $$ = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
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
decl : ANY_STRING
    {
        auto token_index = ($1);
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        $$ = interpreter.push_leaf(wasp::DECL
                                   ,quote_less_data.c_str()
                                   ,token_index);
    }

key_declaration : tag boolean_numeric_op
        {
             $$ = $1;
             $$->push_back($2);

        }
declaration : tag
tag :   decl
        {
             $$ = new std::vector<size_t>();
             $$->push_back($decl);
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
indices_selection : integer
            {
                // capture integer index
                size_t i_i = ($1);
                std::vector<size_t> child_indices = {i_i
                                                           };

                $$ = interpreter.push_parent(wasp::INDEX
                             ,"I" // I - index
                             ,child_indices);
            }
            | integer colon integer
            {
                // capture index range
                size_t si_i = ($1);
                size_t c_i = ($2);
                size_t ei_i = ($3);
                std::vector<size_t> child_indices = {si_i
                                                           ,c_i
                                                           ,ei_i
                                                           };

                $$ = interpreter.push_parent(wasp::INDEX
                            ,"I" // I - index
                            ,child_indices);
            }
            | integer colon integer colon integer
            {
                // capture strided index range
                size_t si_i = ($1);
                size_t c1_i = ($2);
                size_t ei_i = ($3);
                size_t c2_i = ($4);
                size_t stride_i = ($3);
                std::vector<size_t> child_indices = {si_i
                                                           ,c1_i
                                                           ,ei_i
                                                           ,c2_i
                                                           ,stride_i
                                                           };

                $$ = interpreter.push_parent(wasp::INDEX
                            ,"I" // I - index
                            ,child_indices);
            }
relative_selection : decl  // simple child
             | decl lbracket keyedvalue rbracket
             {
                 // capture condition predicated child selection
                 // obj[name = 'ted']
                 // obj[value <= 5]
                 size_t decl_i = ($1);
                 size_t lb_i = ($2);
                 size_t exp_i = ($3);
                 size_t rb_i = ($4);
                 std::vector<size_t> child_indices = {decl_i
                                                            ,lb_i
                                                            ,exp_i
                                                            ,rb_i
                                                            };

                 $$ = interpreter.push_parent(wasp::PREDICATED_CHILD
                             ,"cpcs" // condition predicated child selection
                             ,child_indices);
             }
             | decl lbracket indices_selection rbracket
             {
                 // capture index predicated child selection
                 // obj[1]
                 // obj[1:3]
                 // obj[1:10:2]
                 size_t decl_i = ($1);
                 size_t lb_i = ($2);
                 size_t indices_i = ($3);
                 size_t rb_i = ($4);
                 std::vector<size_t> child_indices = {decl_i
                                                            ,lb_i
                                                            ,indices_i
                                                            ,rb_i
                                                            };

                 $$ = interpreter.push_parent(wasp::PREDICATED_CHILD
                             ,"ipcs" // index predicated child selection
                             ,child_indices);
             }
             | parent_selection
             {
                 // capture parent selection
                 // obj/..
                 $$ = $1;
             }
             | relative_selection separator relative_selection
             {
                // capture consecutive selections
                // obj/child
                // obj[name='ted']/child
                // obj[name='ted']/.[1]
                 size_t left_i = ($1);
                 size_t sep_i = ($2);
                 size_t right_i = ($3);
                 std::vector<size_t> child_indices = {left_i
                                                            ,sep_i
                                                            ,right_i
                                                            };

                 $$ = interpreter.push_parent(wasp::OBJECT
                             ,"O" // O - object
                             ,child_indices);
             }
             |  relative_selection any_selection relative_selection
             {
                // capture named child selection
                // parent//child
                // obj[name='ted']//child
                // obj[name='ted']//.[1]
                 size_t left_i = ($1);
                 size_t any_i = ($2);
                 size_t right_i = ($3);
                 std::vector<size_t> child_indices = {left_i
                                                            ,any_i
                                                            ,right_i
                                                            };

                 $$ = interpreter.push_parent(wasp::ANY
                             ,"A" // A - any child
                             ,child_indices);
             }
             |  relative_selection any_selection
             {
                // capture named node's descendants
                // parent//
                // obj[name='ted']//
                // obj[1]//
                 size_t left_i = ($1);
                 size_t any_i = ($2);
                 std::vector<size_t> child_indices = {left_i
                                                            ,any_i
                                                            };

                 $$ = interpreter.push_parent(wasp::ANY
                             ,"A" // A - any child
                             ,child_indices);
             }
document_root :  SEPARATOR
    {
     auto token_index = ($1);
     $$ = interpreter.push_leaf(wasp::DOCUMENT_ROOT,"R",token_index);
    }
root_based_selection : separator
     {
         // capture root only selection
         size_t s_i = ($1);
         std::vector<size_t> child_indices = {s_i};
         $$ = interpreter.push_parent(wasp::DOCUMENT_ROOT
                                         ,"R" // R - root
                                         ,child_indices);
     }
     | separator relative_selection
     {
         // capture root based child selection
         size_t root_i = ($1);
         size_t child_i = ($2);
         std::vector<size_t> child_indices = {root_i
                                                    ,child_i
                                                    };

         $$ = interpreter.push_parent(wasp::DOCUMENT_ROOT
                                         ,"R" // R - root
                                         ,child_indices);
     }
     | any_selection
     | any_selection relative_selection
     {
         // capture any child of the root
         size_t any_i = ($1);
         size_t child_i = ($2);
         std::vector<size_t> child_indices = {any_i
                                                    ,child_i
                                                    };

         $$ = interpreter.push_parent(wasp::ANY
                                         ,"A" // A - any
                                         ,child_indices);
     }
start   : root_based_selection {interpreter.push_staged_child(($1)); }
        | relative_selection {interpreter.push_staged_child(($1)); }


 /*** END RULES - Change the wasp grammar rules above ***/

%% /*** Additional Code ***/

void wasp::SIRENParser::error(const SIRENParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
