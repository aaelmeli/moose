// A Bison parser, made by GNU Bison 3.7.6.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 1 "Expr.bison"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <vector>

#line 47 "ExprParser.cpp"


#include "ExprParser.hpp"

// Second part of user prologue.
#line 118 "Expr.bison"


#include "ExprInterpreter.h"
#include "ExprLexer.h"

/* this "connects" the Expr parser in the interpreter to the flex ExprLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex


#line 66 "ExprParser.cpp"



#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 34 "Expr.bison"
namespace wasp {
#line 160 "ExprParser.cpp"

  /// Build a parser object.
  ExprParser::ExprParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class ExprLexerImpl> lexer_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      interpreter (interpreter_yyarg),
      input_stream (input_stream_yyarg),
      lexer (lexer_yyarg)
  {}

  ExprParser::~ExprParser ()
  {}

  ExprParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  ExprParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  ExprParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  ExprParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}

  template <typename Base>
  ExprParser::symbol_kind_type
  ExprParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  ExprParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  ExprParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  ExprParser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  ExprParser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  ExprParser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  ExprParser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  ExprParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  ExprParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  ExprParser::symbol_kind_type
  ExprParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  ExprParser::symbol_kind_type
  ExprParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  ExprParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  ExprParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  ExprParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  ExprParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  ExprParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  ExprParser::symbol_kind_type
  ExprParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  ExprParser::stack_symbol_type::stack_symbol_type ()
  {}

  ExprParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  ExprParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  ExprParser::stack_symbol_type&
  ExprParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  ExprParser::stack_symbol_type&
  ExprParser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  ExprParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YY_USE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  ExprParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  ExprParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  ExprParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  ExprParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  ExprParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  ExprParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  ExprParser::debug_level_type
  ExprParser::debug_level () const
  {
    return yydebug_;
  }

  void
  ExprParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  ExprParser::state_type
  ExprParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  ExprParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  ExprParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  ExprParser::operator() ()
  {
    return parse ();
  }

  int
  ExprParser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    // User initialization code.
#line 43 "Expr.bison"
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &interpreter.stream_name();
    yyla.location.begin.line = yyla.location.end.line = interpreter.start_line();
    yyla.location.begin.column = yyla.location.end.column = interpreter.start_column();
    lexer = std::make_shared<ExprLexerImpl>(interpreter,&input_stream);
}

#line 504 "ExprParser.cpp"


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;

      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // bang: "!"
#line 135 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::BANG      ,"!",token_index);}
#line 642 "ExprParser.cpp"
    break;

  case 3: // gt: ">"
#line 136 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::GT      ,">",token_index);}
#line 648 "ExprParser.cpp"
    break;

  case 4: // lt: "<"
#line 137 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::LT      ,"<",token_index);}
#line 654 "ExprParser.cpp"
    break;

  case 5: // gte: ">="
#line 138 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::GTE      ,">=",token_index);}
#line 660 "ExprParser.cpp"
    break;

  case 6: // lte: "<="
#line 139 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::LTE      ,"<=",token_index);}
#line 666 "ExprParser.cpp"
    break;

  case 7: // neq: "!="
#line 140 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::NEQ      ,"!=",token_index);}
#line 672 "ExprParser.cpp"
    break;

  case 8: // eq: "=="
#line 141 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::EQ       ,"==",token_index);}
#line 678 "ExprParser.cpp"
    break;

  case 9: // and: "&&"
#line 142 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_AND      ,"&&",token_index);}
#line 684 "ExprParser.cpp"
    break;

  case 10: // or: "||"
#line 143 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_OR       ,"||",token_index);}
#line 690 "ExprParser.cpp"
    break;

  case 11: // multiply: "*"
#line 144 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::MULTIPLY , "*",token_index);}
#line 696 "ExprParser.cpp"
    break;

  case 12: // divide: "/"
#line 145 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::DIVIDE   ,"/" ,token_index);}
#line 702 "ExprParser.cpp"
    break;

  case 13: // plus: "+"
#line 146 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::PLUS     ,"+" ,token_index);}
#line 708 "ExprParser.cpp"
    break;

  case 14: // minus: "-"
#line 147 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::MINUS    ,"-" ,token_index);}
#line 714 "ExprParser.cpp"
    break;

  case 15: // exponent: "^"
#line 148 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::EXPONENT ,"^" ,token_index);}
#line 720 "ExprParser.cpp"
    break;

  case 16: // lparen: "("
#line 149 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::LPAREN   ,"(" ,token_index);}
#line 726 "ExprParser.cpp"
    break;

  case 17: // rparen: ")"
#line 150 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::RPAREN   ,")" ,token_index);}
#line 732 "ExprParser.cpp"
    break;

  case 18: // comma: ","
#line 151 "Expr.bison"
                    {size_t token_index = ((yystack_[0].value.token_index));(yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_COMMA    ,"," ,token_index);}
#line 738 "ExprParser.cpp"
    break;

  case 19: // function_name: DECL
#line 160 "Expr.bison"
                     {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DECL,"name"
                         ,token_index);
    }
#line 748 "ExprParser.cpp"
    break;

  case 20: // function_args: %empty
#line 165 "Expr.bison"
                          { (yylhs.value.node_indices) = nullptr;}
#line 754 "ExprParser.cpp"
    break;

  case 21: // function_args: math_exp
#line 167 "Expr.bison"
    {
        size_t node_index = ((yystack_[0].value.node_index));
        (yylhs.value.node_indices) = new std::vector<size_t>();
        (yylhs.value.node_indices)->push_back(node_index);
    }
#line 764 "ExprParser.cpp"
    break;

  case 22: // function_args: function_args comma math_exp
#line 173 "Expr.bison"
    {
        if( (yystack_[2].value.node_indices) == nullptr )
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
        }
        (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
        (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    }
#line 777 "ExprParser.cpp"
    break;

  case 23: // function: value
#line 184 "Expr.bison"
           { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 783 "ExprParser.cpp"
    break;

  case 24: // function: function_name lparen function_args rparen
#line 186 "Expr.bison"
    {
        size_t name_index = ((yystack_[3].value.node_index));
        size_t left_index = ((yystack_[2].value.node_index));
        size_t right_index = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {name_index
                                                   ,left_index};
        if( (yystack_[1].value.node_indices) != nullptr )
        {
            child_indices.reserve(child_indices.size()+(yystack_[1].value.node_indices)->size());
            for( std::size_t i = 0; i < (yystack_[1].value.node_indices)->size(); ++i)
            {
                child_indices.push_back((yystack_[1].value.node_indices)->at(i));
            }
            delete (yystack_[1].value.node_indices);
        }
        child_indices.push_back(right_index);
        const std::string & name = interpreter.data(name_index);
        (yylhs.value.node_index) = interpreter.push_parent(wasp::FUNCTION
                                        ,name.c_str()
                                        ,child_indices);
     }
#line 809 "ExprParser.cpp"
    break;

  case 25: // function: function_name lbracket math_exp rbracket
#line 208 "Expr.bison"
     {
         std::vector<size_t> child_indices = {(yystack_[3].value.node_index),(yystack_[2].value.node_index),(yystack_[1].value.node_index),(yystack_[0].value.node_index)};

         const std::string& name = wasp::strip_quotes(interpreter.data((yystack_[3].value.node_index)));
         (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                         , name.c_str()
                                         , child_indices);
     }
#line 822 "ExprParser.cpp"
    break;

  case 26: // function: function_name lbracket math_exp rbracket assign math_exp
#line 217 "Expr.bison"
     {
         std::vector<size_t> child_indices = {(yystack_[5].value.node_index),(yystack_[4].value.node_index),(yystack_[3].value.node_index),(yystack_[2].value.node_index),(yystack_[1].value.node_index),(yystack_[0].value.node_index)};

         const std::string & name = wasp::strip_quotes(interpreter.data((yystack_[5].value.node_index)));
         (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                         , name.c_str()
                                         , child_indices);
     }
#line 835 "ExprParser.cpp"
    break;

  case 27: // math_exp: function
#line 225 "Expr.bison"
           { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 841 "ExprParser.cpp"
    break;

  case 28: // math_exp: minus math_exp
#line 227 "Expr.bison"
    {
        size_t left_index = ((yystack_[1].value.node_index));
        size_t op_index = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index};
        (yylhs.value.node_index) = interpreter.push_parent(wasp::UNARY_MINUS
                                        ,"exp"
                                        ,child_indices);
    }
#line 855 "ExprParser.cpp"
    break;

  case 29: // math_exp: bang math_exp
#line 237 "Expr.bison"
   {
       size_t left_index = ((yystack_[1].value.node_index));
       size_t op_index = ((yystack_[0].value.node_index));
       std::vector<size_t> child_indices = {left_index
                                                  ,op_index};
       (yylhs.value.node_index) = interpreter.push_parent(wasp::UNARY_NOT
                                       ,"exp"
                                       ,child_indices);
    }
#line 869 "ExprParser.cpp"
    break;

  case 30: // math_exp: lparen math_exp rparen
#line 247 "Expr.bison"
    {
        size_t left_index = ((yystack_[2].value.node_index));
        size_t op_index = ((yystack_[1].value.node_index));
        size_t right_index = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index
                                                   ,right_index};
        (yylhs.value.node_index) = interpreter.push_parent(wasp::PARENTHESIS
                                        ,"exp"
                                        ,child_indices);
    }
#line 885 "ExprParser.cpp"
    break;

  case 31: // math_exp: math_exp multiply math_exp
#line 259 "Expr.bison"
    {
        size_t left_index = ((yystack_[2].value.node_index));
        size_t op_index = ((yystack_[1].value.node_index));
        size_t right_index = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_index
                                                   ,op_index
                                                   ,right_index};
        (yylhs.value.node_index) = interpreter.push_parent(wasp::MULTIPLY
                                        ,"exp"
                                        ,child_indices);
    }
#line 901 "ExprParser.cpp"
    break;

  case 32: // math_exp: math_exp divide math_exp
#line 271 "Expr.bison"
   {
       size_t left_index = ((yystack_[2].value.node_index));
       size_t op_index = ((yystack_[1].value.node_index));
       size_t right_index = ((yystack_[0].value.node_index));
       std::vector<size_t> child_indices = {left_index
                                                  ,op_index
                                                  ,right_index};
       (yylhs.value.node_index) = interpreter.push_parent(wasp::DIVIDE
                                       ,"exp"
                                       ,child_indices);
   }
#line 917 "ExprParser.cpp"
    break;

  case 33: // math_exp: math_exp plus math_exp
#line 283 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::PLUS
                                      ,"exp"
                                      ,child_indices);
   }
#line 933 "ExprParser.cpp"
    break;

  case 34: // math_exp: math_exp minus math_exp
#line 295 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::MINUS
                                      ,"exp"
                                      ,child_indices);
   }
#line 949 "ExprParser.cpp"
    break;

  case 35: // math_exp: math_exp exponent math_exp
#line 307 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::EXPONENT
                                      ,"exp"
                                      ,child_indices);
   }
#line 965 "ExprParser.cpp"
    break;

  case 36: // math_exp: math_exp eq math_exp
#line 319 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::EQ
                                      ,"exp"
                                      ,child_indices);
   }
#line 981 "ExprParser.cpp"
    break;

  case 37: // math_exp: math_exp neq math_exp
#line 331 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::NEQ
                                      ,"exp"
                                      ,child_indices);
   }
#line 997 "ExprParser.cpp"
    break;

  case 38: // math_exp: math_exp and math_exp
#line 343 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::WASP_AND
                                      ,"exp"
                                      ,child_indices);
   }
#line 1013 "ExprParser.cpp"
    break;

  case 39: // math_exp: math_exp or math_exp
#line 355 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::WASP_OR
                                      ,"exp"
                                      ,child_indices);
   }
#line 1029 "ExprParser.cpp"
    break;

  case 40: // math_exp: math_exp gte math_exp
#line 367 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::GTE
                                      ,"exp"
                                      ,child_indices);
   }
#line 1045 "ExprParser.cpp"
    break;

  case 41: // math_exp: math_exp gt math_exp
#line 379 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::GT
                                      ,"exp"
                                      ,child_indices);
   }
#line 1061 "ExprParser.cpp"
    break;

  case 42: // math_exp: math_exp lte math_exp
#line 391 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::LTE
                                      ,"exp"
                                      ,child_indices);
   }
#line 1077 "ExprParser.cpp"
    break;

  case 43: // math_exp: math_exp lt math_exp
#line 403 "Expr.bison"
   {
      size_t left_index = ((yystack_[2].value.node_index));
      size_t op_index = ((yystack_[1].value.node_index));
      size_t right_index = ((yystack_[0].value.node_index));
      std::vector<size_t> child_indices = {left_index
                                                 ,op_index
                                                 ,right_index};
      (yylhs.value.node_index) = interpreter.push_parent(wasp::LT
                                      ,"exp"
                                      ,child_indices);
   }
#line 1093 "ExprParser.cpp"
    break;

  case 44: // assign: "="
#line 415 "Expr.bison"
    {
        size_t assign_token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::ASSIGN,"="
                         ,assign_token_index);
    }
#line 1103 "ExprParser.cpp"
    break;

  case 45: // lbracket: "["
#line 422 "Expr.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACKET,"["
                         ,token_index);
    }
#line 1113 "ExprParser.cpp"
    break;

  case 46: // rbracket: "]"
#line 428 "Expr.bison"
    {

        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACKET,"]"
                         ,token_index);
    }
#line 1124 "ExprParser.cpp"
    break;

  case 47: // VALUE: "integer"
#line 453 "Expr.bison"
        { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1130 "ExprParser.cpp"
    break;

  case 48: // VALUE: "real"
#line 453 "Expr.bison"
                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1136 "ExprParser.cpp"
    break;

  case 49: // VALUE: "string"
#line 453 "Expr.bison"
                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1142 "ExprParser.cpp"
    break;

  case 50: // VALUE: "quoted string"
#line 453 "Expr.bison"
                                  { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1148 "ExprParser.cpp"
    break;

  case 51: // value: VALUE
#line 455 "Expr.bison"
    {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::VALUE,"value"
                         ,token_index);
    }
#line 1158 "ExprParser.cpp"
    break;

  case 52: // DECL: "string"
#line 460 "Expr.bison"
       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1164 "ExprParser.cpp"
    break;

  case 53: // DECL: "quoted string"
#line 460 "Expr.bison"
                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 1170 "ExprParser.cpp"
    break;

  case 54: // decl: "string"
#line 462 "Expr.bison"
    {
        size_t decl_token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DECL,"decl"
                         ,decl_token_index);
    }
#line 1180 "ExprParser.cpp"
    break;

  case 55: // keyedvalue: decl assign math_exp
#line 470 "Expr.bison"
    {

        size_t key_index = ((yystack_[2].value.node_index));
        size_t assign_index = ((yystack_[1].value.node_index));
        size_t value_index = ((yystack_[0].value.node_index));

        std::vector<size_t> child_indices = {key_index, assign_index,value_index};

        (yylhs.value.node_index) = interpreter.push_parent(wasp::KEYED_VALUE
                                        ,interpreter.data(key_index).c_str()
                                        ,child_indices);
    }
#line 1197 "ExprParser.cpp"
    break;

  case 57: // start: keyedvalue
#line 485 "Expr.bison"
                    {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1205 "ExprParser.cpp"
    break;

  case 58: // start: math_exp
#line 488 "Expr.bison"
                  {
            interpreter.push_staged_child((yystack_[0].value.node_index));
        }
#line 1213 "ExprParser.cpp"
    break;


#line 1217 "ExprParser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  ExprParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  ExprParser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  ExprParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // ExprParser::context.
  ExprParser::context::context (const ExprParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  ExprParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        int yychecklim = yylast_ - yyn + 1;
        int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }



  int
  ExprParser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  ExprParser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char ExprParser::yypact_ninf_ = -57;

  const signed char ExprParser::yytable_ninf_ = -55;

  const signed char
  ExprParser::yypact_[] =
  {
      -3,   -57,   -57,   -57,   -57,   -57,     5,     3,    56,    56,
      56,     4,   -57,   106,   -57,   -57,   -57,    -5,   -57,    15,
       6,   -17,   -17,    91,   -57,    56,    56,   -57,   -57,   -57,
     -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,    56,
      56,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,   -57,    56,   -57,   -57,   -57,    90,   106,    76,
      54,    54,    54,    54,    54,    54,   119,   119,   -17,   -17,
      47,    47,   -17,   106,   -57,   -57,    56,   -57,    -5,   106,
      56,   106
  };

  const signed char
  ExprParser::yydefact_[] =
  {
      56,     2,    14,    16,    47,    48,    49,    50,     0,     0,
       0,     0,    27,    58,    51,    23,    19,     0,    57,     0,
      49,    29,    28,     0,    45,    20,     0,     5,     6,     4,
       3,     7,     8,     9,    10,    11,    12,    13,    15,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    44,     0,     1,    17,    30,     0,    21,     0,
      41,    43,    40,    42,    37,    36,    38,    39,    31,    32,
      33,    34,    35,    55,    18,    24,     0,    46,    25,    22,
       0,    26
  };

  const signed char
  ExprParser::yypgoto_[] =
  {
     -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,   -57,
     -57,   -57,   -57,   -10,   -57,     9,   -41,   -57,   -57,   -57,
     -57,    -8,   -56,   -57,   -57,   -57,   -57,   -57,   -57,   -57,
     -57
  };

  const signed char
  ExprParser::yydefgoto_[] =
  {
       0,     8,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,     9,    51,    10,    56,    76,    11,    57,
      12,    13,    53,    26,    78,    14,    15,    16,    17,    18,
      19
  };

  const signed char
  ExprParser::yytable_[] =
  {
      21,    22,    23,    50,    52,     1,    38,   -53,    24,   -52,
     -52,    50,    50,    50,   -54,    54,    75,    58,    59,     2,
      25,     3,    80,     4,     5,     6,     7,   -53,     3,   -52,
     -52,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,     0,    73,     0,     0,    50,    50,
      50,    50,    50,    50,    50,    50,    50,    50,    50,    50,
      50,    50,    50,    50,     1,     0,    35,    36,    79,    50,
      38,    50,    81,    35,    36,    37,     2,    38,     2,     0,
       3,    77,     4,     5,    20,     7,     0,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     2,    38,
      74,     0,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,     2,    38,    55,    55,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,     2,    38,
      27,    28,    29,    30,    31,    32,     0,     0,    35,    36,
      37,     2,    38
  };

  const signed char
  ExprParser::yycheck_[] =
  {
       8,     9,    10,    13,     9,     8,    23,     4,     4,     4,
       4,    21,    22,    23,     9,     0,    57,    25,    26,    22,
      11,    24,    78,    26,    27,    28,    29,    24,    24,    24,
      24,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    -1,    53,    -1,    -1,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,     8,    -1,    19,    20,    76,    79,
      23,    81,    80,    19,    20,    21,    22,    23,    22,    -1,
      24,     5,    26,    27,    28,    29,    -1,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      10,    -1,    11,    12,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    25,    25,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      11,    12,    13,    14,    15,    16,    -1,    -1,    19,    20,
      21,    22,    23
  };

  const signed char
  ExprParser::yystos_[] =
  {
       0,     8,    22,    24,    26,    27,    28,    29,    38,    50,
      52,    55,    57,    58,    62,    63,    64,    65,    66,    67,
      28,    58,    58,    58,     4,    52,    60,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    23,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,    51,     9,    59,     0,    25,    53,    56,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    10,    53,    54,     5,    61,    58,
      59,    58
  };

  const signed char
  ExprParser::yyr1_[] =
  {
       0,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    56,    56,    57,    57,    57,    57,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    59,    60,    61,    62,    62,    62,
      62,    63,    64,    64,    65,    66,    67,    67,    67
  };

  const signed char
  ExprParser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     3,     1,     4,     4,     6,     1,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     0,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const ExprParser::yytname_[] =
  {
  "\"end of expression\"", "error", "\"invalid token\"",
  "\"end of line\"", "\"[\"", "\"]\"", "'{'", "'}'", "\"!\"", "\"=\"",
  "\",\"", "\">=\"", "\"<=\"", "\"<\"", "\">\"", "\"!=\"", "\"==\"",
  "\"&&\"", "\"||\"", "\"*\"", "\"/\"", "\"+\"", "\"-\"", "\"^\"", "\"(\"",
  "\")\"", "\"integer\"", "\"real\"", "\"string\"", "\"quoted string\"",
  "\"comment\"", "\"declarator\"", "\"value\"", "UMINUS", "UNOT",
  "\"constant\"", "\"name\"", "$accept", "bang", "gt", "lt", "gte", "lte",
  "neq", "eq", "and", "or", "multiply", "divide", "plus", "minus",
  "exponent", "lparen", "rparen", "comma", "function_name",
  "function_args", "function", "math_exp", "assign", "lbracket",
  "rbracket", "VALUE", "value", "DECL", "decl", "keyedvalue", "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  ExprParser::yyrline_[] =
  {
       0,   135,   135,   136,   137,   138,   139,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   160,
     165,   166,   172,   184,   185,   207,   216,   225,   226,   236,
     246,   258,   270,   282,   294,   306,   318,   330,   342,   354,
     366,   378,   390,   402,   414,   421,   427,   453,   453,   453,
     453,   454,   460,   460,   461,   469,   484,   485,   488
  };

  void
  ExprParser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  ExprParser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  ExprParser::symbol_kind_type
  ExprParser::yytranslate_ (int t)
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     6,     2,     7,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36
    };
    // Last valid token kind.
    const int code_max = 289;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 34 "Expr.bison"
} // wasp
#line 1805 "ExprParser.cpp"

#line 495 "Expr.bison"
 /*** Additional Code ***/
namespace wasp{
void ExprParser::error(const ExprParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<"***Error : "<<l<<": "<<m<<"."<<std::endl;
}
} // end of namespace
