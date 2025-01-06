#include "ExprInterpreter.h"

namespace wasp
{
ExprInterpreter::ExprInterpreter(std::ostream& err) : m_lexer(nullptr)
{
}
ExprInterpreter::~ExprInterpreter()
{
}
bool ExprInterpreter::parse(std::istream& in,
                            std::size_t   startLine,
                            std::size_t   startColumn)
{
    return parse_impl<ExprLexerImpl, ExprParser, ExprInterpreter>(
        m_lexer, in, "input", startLine, startColumn);
}

}  // end of namespace
