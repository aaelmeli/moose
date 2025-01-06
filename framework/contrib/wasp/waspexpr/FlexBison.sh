flex -Caf -v Expr.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' ExprLexer.cpp 
bison --version
bison Expr.bison --report=state --report=lookahead
sed -i '' 's/class ExprParser/class WASP_PUBLIC ExprParser/' ExprParser.hpp
#bison Expr.bison --report=state --report=lookahead --report=solved  -g
