flex -Caf -v JSONObjectLexer.lex
sed -i '' 's/#include <FlexLexer.h>/#include \"waspcore\/FlexLexer.h\"/' JSONObjectLexer.cpp 
bison --version
bison JSONObjectParser.bison --report=state --report=lookahead --report=solved  -g
sed -i '' 's/class JSONObjectParser/class WASP_PUBLIC JSONObjectParser/' JSONObjectParser.hpp
