#include "HITInterpreter.h"

namespace wasp
{
HITInterpreter::HITInterpreter(std::ostream& err)
    : Interpreter(err), m_lexer(nullptr)
{
}
HITInterpreter::~HITInterpreter()
{
}

bool HITInterpreter::parse(std::istream& in,
                              std::size_t   startLine,
                              std::size_t   startColumn)
{
    return parse_impl<HITLexerImpl, HITParser, HITInterpreter>(
        m_lexer, in, "input", startLine, startColumn);
}

}  // end of namespace
