%{ /*** C/C++ Declarations ***/

#include <stdio.h>
#include <vector>
#include <cstring>
%}

/*** yacc/bison Declarations ***/

%code requires{
#include <memory>
#include "waspcore/TreeNodePool.h"
#include "waspcore/wasp_node.h"
#include "waspcore/decl.h"
#include "waspcore/utils.h"
}

/* Require biHIT 3 or later */

%output "HITParser.cpp"
/* add debug output code to generated parser. disable this for release
 * versions. */
 // %debug

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
%define api.parser.class {HITParser}

/* keep track of the current position within the input */
%locations
%initial-action
{
    // initialize the initial location object
    @$.begin.filename = @$.end.filename = &interpreter.stream_name();
    @$.begin.line = @$.end.line = interpreter.start_line();
    @$.begin.column = @$.end.column = interpreter.start_column();
    lexer = std::make_shared<HITLexerImpl>(interpreter,&input_stream);
};

/* The interpreter is passed by reference to the parser and to the HITLexer. This
 * provides a simple but effective pure interface, not relying on global
 * variables. */
%parse-param { class AbstractInterpreter& interpreter }
             {std::istream &input_stream}
             {std::shared_ptr<class HITLexerImpl> lexer}
/* verbose error messages */
%define parse.error verbose

 /*** BEGIN EXAMPLE - Change the HIT grammar's tokens below ***/

//%define api.value.type {struct YYSTYPE}
%union{
    std::size_t token_index;
    std::size_t node_index;
    std::vector<size_t>* node_indices;
    // Objects have children and the child's type field's value is promoted
    // to the parent object's name. The pairing indicates the index to
    // the 'type' field in the vector. If no type field exist
    // the pair.first (index) = pair.second (vector>.size()
    std::pair<size_t, std::vector<size_t>*>* object_children;
}


%token                  END          0  "end of file"
%token                  EOL             "end of line"
%token <token_index>   LBRACKET         "["
%token <token_index>   RBRACKET         "]"
%token                 '{'
%token                 '}'
%token <token_index>   ASSIGN           "="
%token <token_index>   SEMICOLON        ";"
%token <token_index>   DOT_SLASH        "subblock indicator ./"
%token <token_index>   QUOTE            "'"
%token <token_index>   INTEGER         "integer"
%token <token_index>   REAL          "real"
%token <token_index>   STRING          "string"
%token <token_index>   OBJCT_STRING    "object name"
%token <token_index>   PARAM_STRING    "parameter name"
%token <token_index>   VALUE_STRING    "value string"
%token <token_index>   ARRAY_STRING    "array string"
%token <token_index>   QSTRING          "quoted string"
%token <token_index>   COMMENT           "comment"
%token <token_index>   OBJECT_TERM  "block terminator"
%type <token_index>   SECTION_NAME   "section name"
%type <token_index>   VALUE "value"
%type <node_index>  integer real string lbracket rbracket quote assign semicolon
%type <node_index>  object_term section_name field_name
%type <node_index>  object
%type <node_index>  unquoted_string double_quoted_string
%type <node_index>  keyedvalue dot_slash
%type <node_index>  comment value primitive include include_file
%type <node_index> object_member array_member path
%type <node_indices> array_members array double_quoted_strings
%type <node_indices> object_decl
%type <object_children> object_members
%token <token_index>    FILE              "file include"
 //%type <node_indices> last_object
%destructor { delete $$; } object_decl
%destructor { delete $$; } array_members array double_quoted_strings
%destructor { delete $$->second; delete $$; } object_members
%{

#include "HITInterpreter.h"
#include "HITLexer.h"
#include "HITConfig.h"

// Obtain the HIT name for an object_term
// I.e., 
// [name]
//     type=foo
//     ...
// has a rename occur equivalent to [foo_type].
// @return true, iff the name conversion occurs
bool hit_get_name(std::string& name,
                            size_t object_decl_i
                            , wasp::AbstractInterpreter & interpreter
                            , std::pair<size_t, std::vector<size_t>*>* object_members)
{
// if type promotion is disabled, then just return early before any checks
#if DISABLE_HIT_TYPE_PROMOTION
    return false;
#endif
    bool has_type = object_members->first != object_members->second->size();
    auto name_i = object_decl_i;
    if( has_type )
    { // update/promote
        // should be type = value
        name_i = object_members->second->at(object_members->first);
        // acquire the data from the value
        //TODO - conduct checks
        name_i = interpreter.child_index_at(name_i,interpreter.child_count(name_i)-1);
        name = interpreter.data(name_i) + "_type";
    }
    return has_type; 
}

size_t push_object(wasp::AbstractInterpreter & interpreter,
                   std::vector<size_t>& object_decl, 
                   std::pair<size_t, std::vector<size_t>*>* object_members,  
                   size_t object_term)
{
    std::vector<size_t> &child_indices = object_decl;
    size_t object_decl_i = child_indices.rbegin()[1];
    std::string name = interpreter.data(object_decl_i).c_str();

    // handle 'x/y/z' names becoming tree hierarchy
    std::vector<std::string> names = wasp::split("/", name);
    if (object_members != nullptr && !object_members->second->empty())
    {
        for( size_t child_i: *object_members->second ) child_indices.push_back(child_i);

        // update name to <type>_type if type is present
        hit_get_name(names.back(), object_decl_i
                                  ,interpreter
                                  ,object_members);
    }
    child_indices.push_back(object_term);

    
    size_t result_index = 0;

    // if '///' or some illegal mess, use the full name
    // so we still have a node on the tree
    if (names.empty() ) names.push_back(name);

    // push children first and add children
    // to new parent
    while( !names.empty() )
    {
        // skip empty names
        if (names.back().empty())
        {
            names.pop_back();
            continue;
        }
        result_index = interpreter.push_parent(wasp::OBJECT
                                    ,names.back().c_str()
                                    ,child_indices);        
        child_indices.clear();
        child_indices.push_back(result_index);
        names.pop_back();
    }
    
    return result_index;
}

size_t push_keyed_value_or_array(wasp::AbstractInterpreter & interpreter,
                                 std::vector<size_t>       & child_indices)
{
    // keyed values pass in exactly three children and arrays pass in more

    wasp_check(child_indices.size() >= 3);

    std::string name = interpreter.data(child_indices.front()).c_str();

    // split 'x/y/z' names into vector to iterate and build tree hierarchy

    std::vector<std::string> names = wasp::split("/", name);

    // for something illegal like '///', use full name so tree gets a node

    if (names.empty())
    {
        names.push_back(name);
    }

    // add keyed value or array then build hierarchy out of parent objects

    for(size_t result_index = 0; !names.empty(); names.pop_back())
    {
        // skip over empty levels in tree caused by consecutive delimiters

        if (names.back().empty())
        {
            continue;
        }

        // exactly three children is for a keyed value at the lowest level

        if (child_indices.size() == 3)
        {
            result_index = interpreter.push_parent(wasp::KEYED_VALUE    ,
                                                   names.back().c_str() ,
                                                   child_indices        );
        }

        // more than three children indicates an array at the lowest level

        else if (child_indices.size() > 3)
        {
            result_index = interpreter.push_parent(wasp::ARRAY          ,
                                                   names.back().c_str() ,
                                                   child_indices        );
        }

        // any other count (i.e., one) is for a higher level parent object

        else
        {
            wasp_check(child_indices.size() == 1);

            result_index = interpreter.push_parent(wasp::OBJECT         ,
                                                   names.back().c_str() ,
                                                   child_indices        );
        }

        // clear the vector of child indices and add only the result index

        child_indices.clear();

        child_indices.push_back(result_index);
    }

    // return result index from top level left in the child indices vector

    return child_indices.back();
}

/* this "connects" the HIT parser in the interpreter to the flex HITLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex

%}

%% /*** Grammar Rules ***/

 /*** BEGIN - Change the HIT grammar rules below ***/

semicolon : SEMICOLON
    {
        size_t token_index = ($1);$$ = interpreter.push_leaf(wasp::SEMICOLON,";",token_index);
    }
assign : ASSIGN
    {
        size_t assign_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::ASSIGN,"="
                         ,assign_token_index);
    }

object_term : OBJECT_TERM
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::OBJECT_TERM,"term"
                         ,token_index);
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
dot_slash : DOT_SLASH
    {

        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DOT_SLASH,"./"
                         ,token_index);
    }
include : FILE
        {
            auto token_index = $1;
            $$ = interpreter.push_leaf(wasp::FILE,"decl",token_index);
        }    
object_decl : lbracket section_name rbracket {
        size_t lbracket_index = ($lbracket);
        size_t decl_index = ($section_name);
        size_t rbracket_index = ($rbracket);
        $$ = new std::vector<size_t>{lbracket_index
                                                   ,decl_index
                                                   ,rbracket_index};
    }| lbracket  dot_slash section_name rbracket
    {
        size_t lbracket_index = ($lbracket);
        size_t dot_slash_index = ($dot_slash);
        size_t decl_index = ($section_name);
        size_t rbracket_index = ($rbracket);


        $$ = new std::vector<size_t>{lbracket_index
                ,dot_slash_index
                ,decl_index
                ,rbracket_index};
    }

object_member : primitive | keyedvalue | comment
                | object | include_file

object_members : object_member
    {
        size_t node_index = ($1);
        auto indices = new std::vector<size_t>();
        indices->push_back(node_index);
        if( std::strcmp("type",interpreter.name(node_index)) == 0 )
        {
            // -1 because we just pushed the 'type' node
            $$ = new std::pair<size_t,std::vector<size_t>*>
                    (indices->size()-1, indices);
        }
        else{
            $$ = new std::pair<size_t,std::vector<size_t>*>
                    (indices->size(), indices);
        }
    }| object_members object_member
    {

        // only if the type has not already be assigned
        // and the new object is named type
        bool type_not_present = $1->first == $1->second->size();
        if( type_not_present
            && std::strcmp("type",interpreter.name($object_member)) == 0 )
        {
            $1->first = $1->second->size();
        }else if (type_not_present){
            $1->first = $1->second->size()+1;
        }
        $1->second->push_back(($object_member));
        $$ = $1;
    }


object : object_decl object_term
        { // empty object        
        $$ = push_object(interpreter, *$object_decl, nullptr, $object_term);
        delete $1;
        }
        | object_decl object_members object_term
        {
        $$ = push_object(interpreter, *$object_decl, $object_members, $object_term);        
        delete $1;
        delete $2->second;
        delete $2;
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
        $$ = interpreter.push_leaf(wasp::STRING,"string"
                         ,token_index);
    }
double_quoted_string : QSTRING
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::VALUE, "value", token_index);
    }
double_quoted_strings : double_quoted_string
    {
        size_t qstring_index = ($1);
        $$ = new std::vector<size_t>();
        $$->push_back(qstring_index);
    }
    | double_quoted_strings double_quoted_string
    {
        size_t qstring_index = ($2);
        $1->push_back(qstring_index);
        $$ = $1;
    }
VALUE : INTEGER | REAL | VALUE_STRING | ARRAY_STRING
value : VALUE
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::VALUE,"value"
                         ,token_index);
    }
SECTION_NAME : OBJCT_STRING | INTEGER
section_name : SECTION_NAME
    {
        size_t decl_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DECL,"decl"
                         ,decl_token_index);
    }
field_name : PARAM_STRING
    {
        size_t decl_token_index = ($1);
        $$ = interpreter.push_leaf(wasp::DECL,"decl"
                         ,decl_token_index);
    }
quote : QUOTE
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::QUOTE,"'"
                         ,token_index);
    }
string : unquoted_string

primitive : integer
           | real
           | string

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

array_member : semicolon | value | double_quoted_string

array_members : array_member
    {
        size_t offset = ($1);
        $$ = new std::vector<size_t>();
        $$->push_back(offset);
    }| array_members array_member
    {
        $1->push_back(($array_member));
        $$ = $1;
    }

array : quote array_members quote
    {
        $array_members->insert($array_members->begin(),$1);
        $array_members->push_back(($3));
        $$ = $array_members;
    }
    |quote  quote
    {
        $$ = new std::vector<size_t>();
        $$->push_back(($1));
        $$->push_back(($2));
    }
    | array array
    {
        $$ = $1;
        $$->insert($$->end(), $2->begin(), $2->end());
        delete $2;
    }


keyedvalue : field_name assign value
    {
        size_t key_index = ($1);
        size_t assign_index = ($2);
        size_t value_index = ($3);

        std::vector<size_t> child_indices = {key_index, assign_index,value_index};

        $$ = push_keyed_value_or_array(interpreter, child_indices);
    }
    | field_name assign double_quoted_strings
    {
        size_t key_index = ($1);
        size_t assign_index = ($2);

        std::vector<size_t> child_indices = {key_index, assign_index};
        for( size_t child_i : *$double_quoted_strings ) child_indices.push_back(child_i);
        delete $double_quoted_strings;

        $$ = push_keyed_value_or_array(interpreter, child_indices);
    }
    | field_name assign array
    {

        size_t key_index = ($1);
        size_t assign_index = ($2);

        std::vector<size_t> child_indices = {key_index, assign_index};
        for( size_t child_i : *$array ) child_indices.push_back(child_i);
        delete $array;

        $$ = push_keyed_value_or_array(interpreter, child_indices);
    }

comment : COMMENT
    {
        size_t token_index = ($1);
        $$ = interpreter.push_leaf(wasp::COMMENT,"comment"
                         ,token_index);
    }

start   : /** empty **/
        | start comment{
            interpreter.push_staged_child($2);
        }
        | start keyedvalue{
            interpreter.push_staged_child($2);
        }
        | start object{
            interpreter.push_staged_child($2);
        }
        | start include_file
        {
            // assume the included content will be a child of the existing
            // staged content.
            interpreter.push_staged_child($2);
        }


 /*** END RULES - Change the HIT grammar rules above ***/

%% /*** Additional Code ***/
namespace wasp{
void HITParser::error(const HITParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
} // end of namespace
