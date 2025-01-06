#ifndef WASP_WASP_NODE_H
#define WASP_WASP_NODE_H
namespace wasp
{
/**
 * @brief The NODE enum core node types used by wasp support processors
 */
enum NODE : unsigned char
{
    UNKNOWN,
    DOCUMENT_ROOT,
    DECL,
    TERM,
    INTEGER,
    REAL,
    KEY,
    ASSIGN,
    QUOTE,
    STRING,
    QUOTED_STRING,
    VALUE,
    COMMENT,
    KEYED_VALUE,
    OBJECT_TERM,
    OBJECT,
    SEMICOLON,  // ;
    WASP_COMMA  // ,
    ,
    COLON,
    PLUS  // +
    ,
    MINUS  // -
    ,
    MULTIPLY  // *
    ,
    DIVIDE  // /
    ,
    GTE  // >=
    ,
    GT  // >
    ,
    LTE  // <=
    ,
    LT  // >
    ,
    NEQ  // !=
    ,
    EQ  // ==
    ,
    WASP_AND  // && // disambiguated for cross-platform macro avoidance
    ,
    WASP_OR  // || // disambiguated for cross-platform macro avoidance
    ,
    BANG  // !
    ,
    UNARY_MINUS  // -x
    ,
    UNARY_NOT  // !x
    ,
    EXPONENT  // ^
    ,
    LPAREN  // (
    ,
    RPAREN  // )
    ,
    LBRACE  // {
    ,
    RBRACE  // }
    ,
    PARENTHESIS,
    FUNCTION,
    EXPRESSION,
    LBRACKET  // [
    ,
    RBRACKET  // ]
    ,
    DOT_SLASH,
    WASP_TRUE  // disambiguated for cross-platform macro avoidance
    ,
    WASP_FALSE  // disambiguated for cross-platform macro avoidance
    ,
    FILL_EXPR,
    WASP_TICK  // `
    ,
    IDENTIFIER,
    ARRAY  // sequence of data
    ,
    EXECUTION_UNIT_END,
    EXECUTION_UNIT_START,
    EXECUTION_UNIT,
    SEPARATOR,
    PARENT,
    PREDICATED_CHILD,
    INDEX,
    ANY,
    WASP_NULL  // disambiguated from C-NULL
    ,
    CONDITIONAL,
    FILE,
    REPEAT
};
}
#endif
