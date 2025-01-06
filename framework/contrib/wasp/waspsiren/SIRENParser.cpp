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
#line 1 "SIRENParser.bison"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#line 48 "SIRENParser.cpp"


#include "SIRENParser.hpp"

// Second part of user prologue.
#line 122 "SIRENParser.bison"


#include "SIRENInterpreter.h"
#include "SIRENLexer.h"

/* this "connects" the BISON parser in the interpreter to the flex SIRENLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex


#line 67 "SIRENParser.cpp"



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

#line 33 "SIRENParser.bison"
namespace wasp {
#line 161 "SIRENParser.cpp"

  /// Build a parser object.
  SIRENParser::SIRENParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class SIRENLexerImpl> lexer_yyarg)
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

  SIRENParser::~SIRENParser ()
  {}

  SIRENParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  SIRENParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  SIRENParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  SIRENParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}

  template <typename Base>
  SIRENParser::symbol_kind_type
  SIRENParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  SIRENParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  SIRENParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  SIRENParser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  SIRENParser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  SIRENParser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  SIRENParser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  SIRENParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  SIRENParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  SIRENParser::symbol_kind_type
  SIRENParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  SIRENParser::symbol_kind_type
  SIRENParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  SIRENParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  SIRENParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  SIRENParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  SIRENParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  SIRENParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  SIRENParser::symbol_kind_type
  SIRENParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  SIRENParser::stack_symbol_type::stack_symbol_type ()
  {}

  SIRENParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  SIRENParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  SIRENParser::stack_symbol_type&
  SIRENParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  SIRENParser::stack_symbol_type&
  SIRENParser::stack_symbol_type::operator= (stack_symbol_type& that)
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
  SIRENParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_key_declaration: // key_declaration
#line 121 "SIRENParser.bison"
                    { delete (yysym.value.node_indices); }
#line 369 "SIRENParser.cpp"
        break;

      case symbol_kind::S_tag: // tag
#line 121 "SIRENParser.bison"
                    { delete (yysym.value.node_indices); }
#line 375 "SIRENParser.cpp"
        break;

      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  SIRENParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
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
  SIRENParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  SIRENParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  SIRENParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  SIRENParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  SIRENParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  SIRENParser::debug_level_type
  SIRENParser::debug_level () const
  {
    return yydebug_;
  }

  void
  SIRENParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  SIRENParser::state_type
  SIRENParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  SIRENParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  SIRENParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  SIRENParser::operator() ()
  {
    return parse ();
  }

  int
  SIRENParser::parse ()
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
#line 43 "SIRENParser.bison"
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &interpreter.stream_name();
    yyla.location.begin.line = yyla.location.end.line = interpreter.start_line();
    yyla.location.begin.column = yyla.location.end.column = interpreter.start_column();
    lexer = std::make_shared<SIRENLexerImpl>(interpreter,&input_stream);
    //    lexer->set_debug(true); // requires lexer '%option debug' enabled
}

#line 522 "SIRENParser.cpp"


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
  case 2: // any_selection: "//"
#line 139 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::ANY,"A",token_index);
    }
#line 663 "SIRENParser.cpp"
    break;

  case 3: // parent_selection: ".."
#line 144 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        // P - parent
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::PARENT,"P",token_index);
    }
#line 673 "SIRENParser.cpp"
    break;

  case 4: // colon: ":"
#line 155 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::COLON,":",token_index);
    }
#line 682 "SIRENParser.cpp"
    break;

  case 5: // multiply: "*"
#line 160 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::MULTIPLY,"*",token_index);
    }
#line 691 "SIRENParser.cpp"
    break;

  case 6: // divide: "division operator"
#line 165 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DIVIDE,"/",token_index);
    }
#line 700 "SIRENParser.cpp"
    break;

  case 7: // separator: "path separator"
#line 172 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::SEPARATOR,"/",token_index);
    }
#line 709 "SIRENParser.cpp"
    break;

  case 8: // multiply_divide: multiply
#line 176 "SIRENParser.bison"
                  { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 715 "SIRENParser.cpp"
    break;

  case 9: // multiply_divide: divide
#line 176 "SIRENParser.bison"
                             { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 721 "SIRENParser.cpp"
    break;

  case 10: // plus: "+"
#line 178 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::PLUS,"+",token_index);
    }
#line 730 "SIRENParser.cpp"
    break;

  case 11: // minus: "-"
#line 183 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::MINUS,"-",token_index);
    }
#line 739 "SIRENParser.cpp"
    break;

  case 12: // exponent: "^"
#line 189 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::EXPONENT,"^",token_index);
    }
#line 748 "SIRENParser.cpp"
    break;

  case 13: // boolean_numeric_op: eq
#line 194 "SIRENParser.bison"
                     { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 754 "SIRENParser.cpp"
    break;

  case 14: // boolean_numeric_op: neq
#line 194 "SIRENParser.bison"
                          { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 760 "SIRENParser.cpp"
    break;

  case 15: // boolean_numeric_op: gt
#line 194 "SIRENParser.bison"
                                { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 766 "SIRENParser.cpp"
    break;

  case 16: // boolean_numeric_op: lt
#line 194 "SIRENParser.bison"
                                     { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 772 "SIRENParser.cpp"
    break;

  case 17: // boolean_numeric_op: gte
#line 194 "SIRENParser.bison"
                                          { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 778 "SIRENParser.cpp"
    break;

  case 18: // boolean_numeric_op: lte
#line 194 "SIRENParser.bison"
                                                { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 784 "SIRENParser.cpp"
    break;

  case 19: // eq: "="
#line 197 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::EQ,"==",token_index);
    }
#line 793 "SIRENParser.cpp"
    break;

  case 20: // neq: "!="
#line 202 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::NEQ,"!=",token_index);
    }
#line 802 "SIRENParser.cpp"
    break;

  case 21: // gte: ">="
#line 207 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::GTE,">=",token_index);
    }
#line 811 "SIRENParser.cpp"
    break;

  case 22: // gt: ">"
#line 212 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::GT,">",token_index);
    }
#line 820 "SIRENParser.cpp"
    break;

  case 23: // lte: "<="
#line 217 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LTE,"<=",token_index);
    }
#line 829 "SIRENParser.cpp"
    break;

  case 24: // lt: "<"
#line 222 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LT,"<",token_index);
    }
#line 838 "SIRENParser.cpp"
    break;

  case 25: // boolean_logic_op: and
#line 226 "SIRENParser.bison"
                   { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 844 "SIRENParser.cpp"
    break;

  case 26: // boolean_logic_op: or
#line 226 "SIRENParser.bison"
                         { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 850 "SIRENParser.cpp"
    break;

  case 27: // and: "&&"
#line 228 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_AND,"&&",token_index);
    }
#line 859 "SIRENParser.cpp"
    break;

  case 28: // or: "||"
#line 233 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_OR,"||",token_index);
    }
#line 868 "SIRENParser.cpp"
    break;

  case 29: // unary_not: "!"
#line 239 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::UNARY_NOT,"!",token_index);
    }
#line 877 "SIRENParser.cpp"
    break;

  case 30: // lparen: "("
#line 245 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LPAREN,"(",token_index);
    }
#line 886 "SIRENParser.cpp"
    break;

  case 31: // rparen: ")"
#line 250 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RPAREN,")",token_index);
    }
#line 895 "SIRENParser.cpp"
    break;

  case 32: // lbracket: "["
#line 265 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACKET,"[",token_index);
    }
#line 904 "SIRENParser.cpp"
    break;

  case 33: // rbracket: "]"
#line 270 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACKET,"]",token_index);
    }
#line 913 "SIRENParser.cpp"
    break;

  case 34: // ANY_STRING: "decl"
#line 275 "SIRENParser.bison"
             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 919 "SIRENParser.cpp"
    break;

  case 35: // ANY_STRING: "quoted string"
#line 275 "SIRENParser.bison"
                    { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 925 "SIRENParser.cpp"
    break;

  case 36: // PRIMITIVE: "decl"
#line 276 "SIRENParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 931 "SIRENParser.cpp"
    break;

  case 37: // PRIMITIVE: "quoted string"
#line 276 "SIRENParser.bison"
                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 937 "SIRENParser.cpp"
    break;

  case 38: // PRIMITIVE: "integer"
#line 276 "SIRENParser.bison"
                             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 943 "SIRENParser.cpp"
    break;

  case 39: // PRIMITIVE: "double"
#line 276 "SIRENParser.bison"
                                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 949 "SIRENParser.cpp"
    break;

  case 40: // integer: "integer"
#line 277 "SIRENParser.bison"
                  {
        size_t token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::INTEGER,"int"
                         ,token_index);
    }
#line 959 "SIRENParser.cpp"
    break;

  case 41: // value: PRIMITIVE
#line 283 "SIRENParser.bison"
{
    size_t token_index = ((yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
#line 969 "SIRENParser.cpp"
    break;

  case 42: // component: value
#line 299 "SIRENParser.bison"
            { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 975 "SIRENParser.cpp"
    break;

  case 43: // exp: component
#line 301 "SIRENParser.bison"
      { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 981 "SIRENParser.cpp"
    break;

  case 44: // exp: exp multiply_divide exp
#line 303 "SIRENParser.bison"
    {
          size_t left_i = ((yystack_[2].value.node_index));
          size_t op_i = ((yystack_[1].value.node_index));
          size_t right_i = ((yystack_[0].value.node_index));
          std::vector<size_t> child_indices = {left_i
                                                     ,op_i
                                                     ,right_i
                                                     };

          (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                          ,"value"
                                          ,child_indices);
    }
#line 999 "SIRENParser.cpp"
    break;

  case 45: // exp: exp plus exp
#line 317 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1017 "SIRENParser.cpp"
    break;

  case 46: // exp: exp minus exp
#line 331 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1035 "SIRENParser.cpp"
    break;

  case 47: // exp: exp exponent exp
#line 345 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1053 "SIRENParser.cpp"
    break;

  case 48: // exp: exp boolean_numeric_op exp
#line 359 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1071 "SIRENParser.cpp"
    break;

  case 49: // exp: exp boolean_logic_op exp
#line 374 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1089 "SIRENParser.cpp"
    break;

  case 50: // exp: lparen exp rparen
#line 388 "SIRENParser.bison"
    {
        size_t left_i = ((yystack_[2].value.node_index));
        size_t center_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {left_i
                                                   ,center_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(wasp::PARENTHESIS
                                        ,"value"
                                        ,child_indices);
    }
#line 1107 "SIRENParser.cpp"
    break;

  case 51: // exp: minus exp
#line 402 "SIRENParser.bison"
    {
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1123 "SIRENParser.cpp"
    break;

  case 52: // exp: unary_not exp
#line 414 "SIRENParser.bison"
    {
        size_t op_i = ((yystack_[1].value.node_index));
        size_t right_i = ((yystack_[0].value.node_index));
        std::vector<size_t> child_indices = {op_i
                                                   ,right_i
                                                   };

        (yylhs.value.node_index) = interpreter.push_parent(interpreter.type(op_i)
                                        ,"value"
                                        ,child_indices);
    }
#line 1139 "SIRENParser.cpp"
    break;

  case 53: // decl: ANY_STRING
#line 426 "SIRENParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DECL
                                   ,quote_less_data.c_str()
                                   ,token_index);
    }
#line 1152 "SIRENParser.cpp"
    break;

  case 54: // key_declaration: tag boolean_numeric_op
#line 436 "SIRENParser.bison"
        {
             (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
             (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));

        }
#line 1162 "SIRENParser.cpp"
    break;

  case 55: // tag: decl
#line 443 "SIRENParser.bison"
        {
             (yylhs.value.node_indices) = new std::vector<size_t>();
             (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1171 "SIRENParser.cpp"
    break;

  case 56: // keyedvalue: key_declaration exp
#line 450 "SIRENParser.bison"
    {
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
        std::string quote_less_data = interpreter.data((yystack_[1].value.node_indices)->front());
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_parent(wasp::KEYED_VALUE
                                     // use the data instead of the name
                                     // this provides the following tree
                                     // data
                                     //  |_ decl (data)
                                     //  |_ = (=)
                                     //  |_ value (1.2..blah)
                                    ,quote_less_data.c_str()
                                    ,*(yystack_[1].value.node_indices));
        delete (yystack_[1].value.node_indices);
    }
#line 1191 "SIRENParser.cpp"
    break;

  case 57: // indices_selection: integer
#line 466 "SIRENParser.bison"
            {
                // capture integer index
                size_t i_i = ((yystack_[0].value.node_index));
                std::vector<size_t> child_indices = {i_i
                                                           };

                (yylhs.value.node_index) = interpreter.push_parent(wasp::INDEX
                             ,"I" // I - index
                             ,child_indices);
            }
#line 1206 "SIRENParser.cpp"
    break;

  case 58: // indices_selection: integer colon integer
#line 477 "SIRENParser.bison"
            {
                // capture index range
                size_t si_i = ((yystack_[2].value.node_index));
                size_t c_i = ((yystack_[1].value.node_index));
                size_t ei_i = ((yystack_[0].value.node_index));
                std::vector<size_t> child_indices = {si_i
                                                           ,c_i
                                                           ,ei_i
                                                           };

                (yylhs.value.node_index) = interpreter.push_parent(wasp::INDEX
                            ,"I" // I - index
                            ,child_indices);
            }
#line 1225 "SIRENParser.cpp"
    break;

  case 59: // indices_selection: integer colon integer colon integer
#line 492 "SIRENParser.bison"
            {
                // capture strided index range
                size_t si_i = ((yystack_[4].value.node_index));
                size_t c1_i = ((yystack_[3].value.node_index));
                size_t ei_i = ((yystack_[2].value.node_index));
                size_t c2_i = ((yystack_[1].value.node_index));
                size_t stride_i = ((yystack_[2].value.node_index));
                std::vector<size_t> child_indices = {si_i
                                                           ,c1_i
                                                           ,ei_i
                                                           ,c2_i
                                                           ,stride_i
                                                           };

                (yylhs.value.node_index) = interpreter.push_parent(wasp::INDEX
                            ,"I" // I - index
                            ,child_indices);
            }
#line 1248 "SIRENParser.cpp"
    break;

  case 60: // relative_selection: decl
#line 510 "SIRENParser.bison"
                     { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 1254 "SIRENParser.cpp"
    break;

  case 61: // relative_selection: decl lbracket keyedvalue rbracket
#line 512 "SIRENParser.bison"
             {
                 // capture condition predicated child selection
                 // obj[name = 'ted']
                 // obj[value <= 5]
                 size_t decl_i = ((yystack_[3].value.node_index));
                 size_t lb_i = ((yystack_[2].value.node_index));
                 size_t exp_i = ((yystack_[1].value.node_index));
                 size_t rb_i = ((yystack_[0].value.node_index));
                 std::vector<size_t> child_indices = {decl_i
                                                            ,lb_i
                                                            ,exp_i
                                                            ,rb_i
                                                            };

                 (yylhs.value.node_index) = interpreter.push_parent(wasp::PREDICATED_CHILD
                             ,"cpcs" // condition predicated child selection
                             ,child_indices);
             }
#line 1277 "SIRENParser.cpp"
    break;

  case 62: // relative_selection: decl lbracket indices_selection rbracket
#line 531 "SIRENParser.bison"
             {
                 // capture index predicated child selection
                 // obj[1]
                 // obj[1:3]
                 // obj[1:10:2]
                 size_t decl_i = ((yystack_[3].value.node_index));
                 size_t lb_i = ((yystack_[2].value.node_index));
                 size_t indices_i = ((yystack_[1].value.node_index));
                 size_t rb_i = ((yystack_[0].value.node_index));
                 std::vector<size_t> child_indices = {decl_i
                                                            ,lb_i
                                                            ,indices_i
                                                            ,rb_i
                                                            };

                 (yylhs.value.node_index) = interpreter.push_parent(wasp::PREDICATED_CHILD
                             ,"ipcs" // index predicated child selection
                             ,child_indices);
             }
#line 1301 "SIRENParser.cpp"
    break;

  case 63: // relative_selection: parent_selection
#line 551 "SIRENParser.bison"
             {
                 // capture parent selection
                 // obj/..
                 (yylhs.value.node_index) = (yystack_[0].value.node_index);
             }
#line 1311 "SIRENParser.cpp"
    break;

  case 64: // relative_selection: relative_selection separator relative_selection
#line 557 "SIRENParser.bison"
             {
                // capture consecutive selections
                // obj/child
                // obj[name='ted']/child
                // obj[name='ted']/.[1]
                 size_t left_i = ((yystack_[2].value.node_index));
                 size_t sep_i = ((yystack_[1].value.node_index));
                 size_t right_i = ((yystack_[0].value.node_index));
                 std::vector<size_t> child_indices = {left_i
                                                            ,sep_i
                                                            ,right_i
                                                            };

                 (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                             ,"O" // O - object
                             ,child_indices);
             }
#line 1333 "SIRENParser.cpp"
    break;

  case 65: // relative_selection: relative_selection any_selection relative_selection
#line 575 "SIRENParser.bison"
             {
                // capture named child selection
                // parent//child
                // obj[name='ted']//child
                // obj[name='ted']//.[1]
                 size_t left_i = ((yystack_[2].value.node_index));
                 size_t any_i = ((yystack_[1].value.node_index));
                 size_t right_i = ((yystack_[0].value.node_index));
                 std::vector<size_t> child_indices = {left_i
                                                            ,any_i
                                                            ,right_i
                                                            };

                 (yylhs.value.node_index) = interpreter.push_parent(wasp::ANY
                             ,"A" // A - any child
                             ,child_indices);
             }
#line 1355 "SIRENParser.cpp"
    break;

  case 66: // relative_selection: relative_selection any_selection
#line 593 "SIRENParser.bison"
             {
                // capture named node's descendants
                // parent//
                // obj[name='ted']//
                // obj[1]//
                 size_t left_i = ((yystack_[1].value.node_index));
                 size_t any_i = ((yystack_[0].value.node_index));
                 std::vector<size_t> child_indices = {left_i
                                                            ,any_i
                                                            };

                 (yylhs.value.node_index) = interpreter.push_parent(wasp::ANY
                             ,"A" // A - any child
                             ,child_indices);
             }
#line 1375 "SIRENParser.cpp"
    break;

  case 67: // root_based_selection: separator
#line 614 "SIRENParser.bison"
     {
         // capture root only selection
         size_t s_i = ((yystack_[0].value.node_index));
         std::vector<size_t> child_indices = {s_i};
         (yylhs.value.node_index) = interpreter.push_parent(wasp::DOCUMENT_ROOT
                                         ,"R" // R - root
                                         ,child_indices);
     }
#line 1388 "SIRENParser.cpp"
    break;

  case 68: // root_based_selection: separator relative_selection
#line 623 "SIRENParser.bison"
     {
         // capture root based child selection
         size_t root_i = ((yystack_[1].value.node_index));
         size_t child_i = ((yystack_[0].value.node_index));
         std::vector<size_t> child_indices = {root_i
                                                    ,child_i
                                                    };

         (yylhs.value.node_index) = interpreter.push_parent(wasp::DOCUMENT_ROOT
                                         ,"R" // R - root
                                         ,child_indices);
     }
#line 1405 "SIRENParser.cpp"
    break;

  case 69: // root_based_selection: any_selection
#line 635 "SIRENParser.bison"
       { (yylhs.value.node_index) = (yystack_[0].value.node_index); }
#line 1411 "SIRENParser.cpp"
    break;

  case 70: // root_based_selection: any_selection relative_selection
#line 637 "SIRENParser.bison"
     {
         // capture any child of the root
         size_t any_i = ((yystack_[1].value.node_index));
         size_t child_i = ((yystack_[0].value.node_index));
         std::vector<size_t> child_indices = {any_i
                                                    ,child_i
                                                    };

         (yylhs.value.node_index) = interpreter.push_parent(wasp::ANY
                                         ,"A" // A - any
                                         ,child_indices);
     }
#line 1428 "SIRENParser.cpp"
    break;

  case 71: // start: root_based_selection
#line 649 "SIRENParser.bison"
                               {interpreter.push_staged_child(((yystack_[0].value.node_index))); }
#line 1434 "SIRENParser.cpp"
    break;

  case 72: // start: relative_selection
#line 650 "SIRENParser.bison"
                             {interpreter.push_staged_child(((yystack_[0].value.node_index))); }
#line 1440 "SIRENParser.cpp"
    break;


#line 1444 "SIRENParser.cpp"

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
  SIRENParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  SIRENParser::yytnamerr_ (const char *yystr)
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
  SIRENParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // SIRENParser::context.
  SIRENParser::context::context (const SIRENParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  SIRENParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
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
  SIRENParser::yy_syntax_error_arguments_ (const context& yyctx,
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
  SIRENParser::yysyntax_error_ (const context& yyctx) const
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


  const signed char SIRENParser::yypact_ninf_ = -45;

  const signed char SIRENParser::yytable_ninf_ = -1;

  const signed char
  SIRENParser::yypact_[] =
  {
       4,   -45,   -45,   -45,   -45,   -45,   -14,   -45,   -14,   -45,
      -6,     0,   -45,     6,     0,     0,   -45,   -20,   -14,   -14,
     -45,   -45,    14,   -45,     8,   101,     1,     1,     0,     0,
     -45,    -1,   -45,   -45,   -45,   -45,   -45,   -45,   -45,     8,
       8,     8,   -45,   -45,   -45,    65,   -45,   -45,   -45,   -45,
     -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,
     -45,   -45,    14,     5,     5,    49,   -45,   -45,   -45,   -45,
     -45,   -45,   -45,   -45,     8,     8,     8,     8,     8,     8,
     -45,   -45,    -1,   -45,   -45,     5,    24,    24,     5,    86,
      81,   -45
  };

  const signed char
  SIRENParser::yydefact_[] =
  {
       0,     2,     3,     7,    34,    35,    69,    63,    67,    53,
      60,    72,    71,     0,    70,    68,    32,     0,    66,     0,
       1,    40,    57,    55,     0,     0,     0,     0,    65,    64,
       4,     0,    11,    30,    29,    38,    39,    36,    37,     0,
       0,     0,    41,    42,    43,    56,    21,    24,    22,    23,
      20,    19,    54,    13,    14,    17,    15,    18,    16,    33,
      61,    62,    58,    51,    52,     0,    27,    28,     5,     6,
      10,    12,     8,     9,     0,     0,     0,     0,     0,     0,
      25,    26,     0,    31,    50,    44,    45,    46,    47,    48,
      49,    59
  };

  const signed char
  SIRENParser::yypgoto_[] =
  {
     -45,    29,   -45,   -30,   -45,   -45,    34,   -45,   -45,   -44,
     -45,    22,   -45,   -45,   -45,   -45,   -45,   -45,   -45,   -45,
     -45,   -45,   -45,   -45,   -45,    23,   -45,   -45,   -31,   -45,
     -45,   -17,    36,   -45,   -45,   -45,   -45,    -3,   -45,   -45
  };

  const signed char
  SIRENParser::yydefgoto_[] =
  {
       0,    18,     7,    31,    72,    73,    19,    74,    75,    39,
      77,    78,    53,    54,    55,    56,    57,    58,    79,    80,
      81,    40,    41,    84,    17,    60,     9,    42,    22,    43,
      44,    45,    10,    24,    25,    26,    27,    11,    12,    13
  };

  const signed char
  SIRENParser::yytable_[] =
  {
      62,    76,    16,    14,     1,    15,    20,     2,     1,    21,
      59,     4,     5,    32,    33,    28,    29,     4,     5,    76,
      76,    76,    63,    64,    65,     2,     3,    30,    21,     6,
       3,    34,    82,    71,     8,     4,     5,    35,    36,    37,
      38,    76,    76,    76,    76,    76,    76,    52,    68,    69,
      61,    91,    71,    23,    32,     0,    83,    85,    86,    87,
      88,    89,    90,    46,    47,    48,    49,    50,    51,    66,
      32,    67,     0,    68,    69,     0,    70,    71,     0,    46,
      47,    48,    49,    50,    51,    66,    32,    67,     0,    68,
      69,    32,    70,    71,     0,    46,    47,    48,    49,    50,
      51,     0,     0,     0,     0,    68,    69,     0,    70,    71,
      68,    69,     0,    70,    71,    46,    47,    48,    49,    50,
      51
  };

  const signed char
  SIRENParser::yycheck_[] =
  {
      31,    45,     8,     6,     4,     8,     0,    21,     4,    29,
       9,    31,    32,     5,     6,    18,    19,    31,    32,    63,
      64,    65,    39,    40,    41,    21,    26,    13,    29,     0,
      26,    23,    62,    28,     0,    31,    32,    29,    30,    31,
      32,    85,    86,    87,    88,    89,    90,    25,    24,    25,
      27,    82,    28,    17,     5,    -1,     7,    74,    75,    76,
      77,    78,    79,    14,    15,    16,    17,    18,    19,    20,
       5,    22,    -1,    24,    25,    -1,    27,    28,    -1,    14,
      15,    16,    17,    18,    19,    20,     5,    22,    -1,    24,
      25,     5,    27,    28,    -1,    14,    15,    16,    17,    18,
      19,    -1,    -1,    -1,    -1,    24,    25,    -1,    27,    28,
      24,    25,    -1,    27,    28,    14,    15,    16,    17,    18,
      19
  };

  const signed char
  SIRENParser::yystos_[] =
  {
       0,     4,    21,    26,    31,    32,    38,    39,    43,    63,
      69,    74,    75,    76,    74,    74,     8,    61,    38,    43,
       0,    29,    65,    69,    70,    71,    72,    73,    74,    74,
      13,    40,     5,     6,    23,    29,    30,    31,    32,    46,
      58,    59,    64,    66,    67,    68,    14,    15,    16,    17,
      18,    19,    48,    49,    50,    51,    52,    53,    54,     9,
      62,    62,    65,    68,    68,    68,    20,    22,    24,    25,
      27,    28,    41,    42,    44,    45,    46,    47,    48,    55,
      56,    57,    40,     7,    60,    68,    68,    68,    68,    68,
      68,    65
  };

  const signed char
  SIRENParser::yyr1_[] =
  {
       0,    37,    38,    39,    40,    41,    42,    43,    44,    44,
      45,    46,    47,    48,    48,    48,    48,    48,    48,    49,
      50,    51,    52,    53,    54,    55,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    63,    64,    64,    64,    64,
      65,    66,    67,    68,    68,    68,    68,    68,    68,    68,
      68,    68,    68,    69,    70,    71,    72,    73,    73,    73,
      74,    74,    74,    74,    74,    74,    74,    75,    75,    75,
      75,    76,    76
  };

  const signed char
  SIRENParser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     1,     2,     1,     2,     1,     3,     5,
       1,     4,     4,     1,     3,     3,     2,     1,     2,     1,
       2,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const SIRENParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"end of line\"",
  "\"//\"", "\"-\"", "\"(\"", "\")\"", "\"[\"", "\"]\"", "\"{\"", "\"}\"",
  "\",\"", "\":\"", "\">=\"", "\"<\"", "\">\"", "\"<=\"", "\"!=\"",
  "\"=\"", "\"&&\"", "\"..\"", "\"||\"", "\"!\"", "\"*\"",
  "\"division operator\"", "\"path separator\"", "\"+\"", "\"^\"",
  "\"integer\"", "\"double\"", "\"decl\"", "\"quoted string\"", "ASSIGN",
  "UMINUS", "UNOT", "\"exp\"", "$accept", "any_selection",
  "parent_selection", "colon", "multiply", "divide", "separator",
  "multiply_divide", "plus", "minus", "exponent", "boolean_numeric_op",
  "eq", "neq", "gte", "gt", "lte", "lt", "boolean_logic_op", "and", "or",
  "unary_not", "lparen", "rparen", "lbracket", "rbracket", "ANY_STRING",
  "PRIMITIVE", "integer", "value", "component", "exp", "decl",
  "key_declaration", "tag", "keyedvalue", "indices_selection",
  "relative_selection", "root_based_selection", "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  SIRENParser::yyrline_[] =
  {
       0,   138,   138,   143,   154,   159,   164,   171,   176,   176,
     177,   182,   188,   194,   194,   194,   194,   194,   194,   196,
     201,   206,   211,   216,   221,   226,   226,   227,   232,   238,
     244,   249,   264,   269,   275,   275,   276,   276,   276,   276,
     277,   282,   299,   301,   302,   316,   330,   344,   358,   373,
     387,   401,   413,   425,   435,   442,   449,   465,   476,   491,
     510,   511,   530,   550,   556,   574,   592,   613,   622,   635,
     636,   649,   650
  };

  void
  SIRENParser::yy_stack_print_ () const
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
  SIRENParser::yy_reduce_print_ (int yyrule) const
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

  SIRENParser::symbol_kind_type
  SIRENParser::yytranslate_ (int t)
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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36
    };
    // Last valid token kind.
    const int code_max = 291;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 33 "SIRENParser.bison"
} // wasp
#line 2041 "SIRENParser.cpp"

#line 655 "SIRENParser.bison"
 /*** Additional Code ***/

void wasp::SIRENParser::error(const SIRENParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
