#ifndef WASP_EXPRINTERPRETER_H
#define WASP_EXPRINTERPRETER_H

#include "waspcore/TreeNodePool.h"
#include "waspcore/Interpreter.h"
#include "waspcore/wasp_bug.h"
#include "waspcore/Format.h"
#include "waspexpr/ExprParser.hpp"
#include "waspexpr/ExprContext.h"
#include <cmath>
#include <sstream>
#include <map>

#include "waspcore/decl.h"

namespace wasp
{
template<class S = TreeNodePool<>>
class WASP_PUBLIC ExprInterpreter : public Interpreter<S>
{
  public:
    ExprInterpreter(std::ostream& error_stream = std::cerr);
    ~ExprInterpreter();

    /**
     * @brief parse parser the given input stream
     * @param input the stream of the input data
     * @param startLine the first line of the input stream
     * @param startColumn the first column of the input stream
     * @return true, iff no input processing errors were encountered
     */
    bool parse(std::istream& input,
               std::size_t   m_start_line   = 1u,
               std::size_t   m_start_column = 1u);

  public:
    Result evaluate()
    {
        Context context;
        return evaluate(context);
    }

    Result evaluate(Context& context)
    {
        Result r;
        auto   root_view = this->root();
        wasp_check(root_view.is_null() == false);
        return r.evaluate(root_view, context);
    }
};
#include "waspexpr/ExprInterpreter.i.h"

}  // end of namespace
#endif
