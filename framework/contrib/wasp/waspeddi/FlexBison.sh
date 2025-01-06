flex -Caf -8 -v EDDILexer.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' EDDILexer.cpp 
bison --version
bison EDDIParser.bison --report=state --report=lookahead --report=solved  -g
sed -i '' 's/class EDDIParser/class WASP_PUBLIC EDDIParser/' EDDIParser.hpp
