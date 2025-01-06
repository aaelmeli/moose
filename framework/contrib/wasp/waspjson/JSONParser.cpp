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
#line 1 "JSONParser.bison"
 /*** C/C++ Declarations ***/

#include <stdio.h>
#include <string>
#include <vector>

#line 48 "JSONParser.cpp"


#include "JSONParser.hpp"

// Second part of user prologue.
#line 100 "JSONParser.bison"


#include "JSONInterpreter.h"
#include "JSONLexer.h"

/* this "connects" the bison parser in the interpreter to the flex JSONLexer class
 * object. it defines the yylex() function call to pull the next token from the
 * current lexer object of the interpreter context. */
#undef yylex
#define yylex lexer->lex


#line 67 "JSONParser.cpp"



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

#line 34 "JSONParser.bison"
namespace wasp {
#line 161 "JSONParser.cpp"

  /// Build a parser object.
  JSONParser::JSONParser (class AbstractInterpreter& interpreter_yyarg, std::istream &input_stream_yyarg, std::shared_ptr<class JSONLexerImpl> lexer_yyarg)
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

  JSONParser::~JSONParser ()
  {}

  JSONParser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------------.
  | symbol kinds.  |
  `---------------*/

  // basic_symbol.
  template <typename Base>
  JSONParser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
    , location (that.location)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  JSONParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_MOVE_REF (location_type) l)
    : Base (t)
    , value ()
    , location (l)
  {}

  template <typename Base>
  JSONParser::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (semantic_type) v, YY_RVREF (location_type) l)
    : Base (t)
    , value (YY_MOVE (v))
    , location (YY_MOVE (l))
  {}

  template <typename Base>
  JSONParser::symbol_kind_type
  JSONParser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }

  template <typename Base>
  bool
  JSONParser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  JSONParser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
    location = YY_MOVE (s.location);
  }

  // by_kind.
  JSONParser::by_kind::by_kind ()
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  JSONParser::by_kind::by_kind (by_kind&& that)
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  JSONParser::by_kind::by_kind (const by_kind& that)
    : kind_ (that.kind_)
  {}

  JSONParser::by_kind::by_kind (token_kind_type t)
    : kind_ (yytranslate_ (t))
  {}

  void
  JSONParser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  JSONParser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  JSONParser::symbol_kind_type
  JSONParser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }

  JSONParser::symbol_kind_type
  JSONParser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  // by_state.
  JSONParser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  JSONParser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  JSONParser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  JSONParser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  JSONParser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  JSONParser::symbol_kind_type
  JSONParser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  JSONParser::stack_symbol_type::stack_symbol_type ()
  {}

  JSONParser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value), YY_MOVE (that.location))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  JSONParser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value), YY_MOVE (that.location))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  JSONParser::stack_symbol_type&
  JSONParser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    location = that.location;
    return *this;
  }

  JSONParser::stack_symbol_type&
  JSONParser::stack_symbol_type::operator= (stack_symbol_type& that)
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
  JSONParser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    switch (yysym.kind ())
    {
      case symbol_kind::S_declaration: // declaration
#line 98 "JSONParser.bison"
                    { delete (yysym.value.node_indices); }
#line 369 "JSONParser.cpp"
        break;

      case symbol_kind::S_array: // array
#line 98 "JSONParser.bison"
                    { delete (yysym.value.node_indices); }
#line 375 "JSONParser.cpp"
        break;

      case symbol_kind::S_object: // object
#line 98 "JSONParser.bison"
                    { delete (yysym.value.node_indices); }
#line 381 "JSONParser.cpp"
        break;

      case symbol_kind::S_array_members: // array_members
#line 98 "JSONParser.bison"
                    { delete (yysym.value.node_indices); }
#line 387 "JSONParser.cpp"
        break;

      case symbol_kind::S_object_members: // object_members
#line 98 "JSONParser.bison"
                    { delete (yysym.value.node_indices); }
#line 393 "JSONParser.cpp"
        break;

      default:
        break;
    }
  }

#if YYDEBUG
  template <typename Base>
  void
  JSONParser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
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
  JSONParser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  JSONParser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  JSONParser::yypop_ (int n)
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  JSONParser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  JSONParser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  JSONParser::debug_level_type
  JSONParser::debug_level () const
  {
    return yydebug_;
  }

  void
  JSONParser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  JSONParser::state_type
  JSONParser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  JSONParser::yy_pact_value_is_default_ (int yyvalue)
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  JSONParser::yy_table_value_is_error_ (int yyvalue)
  {
    return yyvalue == yytable_ninf_;
  }

  int
  JSONParser::operator() ()
  {
    return parse ();
  }

  int
  JSONParser::parse ()
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
#line 43 "JSONParser.bison"
{
    // initialize the initial location object
    yyla.location.begin.filename = yyla.location.end.filename = &interpreter.stream_name();
    yyla.location.begin.line = yyla.location.end.line = interpreter.start_line();
    yyla.location.begin.column = yyla.location.end.column = interpreter.start_column();
    lexer = std::make_shared<JSONLexerImpl>(interpreter,&input_stream);
}

#line 539 "JSONParser.cpp"


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
  case 2: // comma: ","
#line 117 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::WASP_COMMA,",",token_index);
    }
#line 680 "JSONParser.cpp"
    break;

  case 3: // BOOLEAN: "true"
#line 121 "JSONParser.bison"
          { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 686 "JSONParser.cpp"
    break;

  case 4: // BOOLEAN: "false"
#line 121 "JSONParser.bison"
                       { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 692 "JSONParser.cpp"
    break;

  case 5: // lbrace: "{"
#line 124 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACE,"{",token_index);
    }
#line 701 "JSONParser.cpp"
    break;

  case 6: // rbrace: "}"
#line 129 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACE,"}",token_index);
    }
#line 710 "JSONParser.cpp"
    break;

  case 7: // lbracket: "["
#line 134 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::LBRACKET,"[",token_index);
    }
#line 719 "JSONParser.cpp"
    break;

  case 8: // rbracket: "]"
#line 139 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::RBRACKET,"]",token_index);
    }
#line 728 "JSONParser.cpp"
    break;

  case 9: // ANY_STRING: "quoted string"
#line 144 "JSONParser.bison"
             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 734 "JSONParser.cpp"
    break;

  case 10: // PRIMITIVE: "quoted string"
#line 145 "JSONParser.bison"
            { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 740 "JSONParser.cpp"
    break;

  case 11: // PRIMITIVE: "integer"
#line 145 "JSONParser.bison"
                      { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 746 "JSONParser.cpp"
    break;

  case 12: // PRIMITIVE: "double"
#line 145 "JSONParser.bison"
                                { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 752 "JSONParser.cpp"
    break;

  case 13: // PRIMITIVE: BOOLEAN
#line 145 "JSONParser.bison"
                                         { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 758 "JSONParser.cpp"
    break;

  case 14: // PRIMITIVE: "null"
#line 145 "JSONParser.bison"
                                                   { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 764 "JSONParser.cpp"
    break;

  case 15: // primitive: PRIMITIVE
#line 148 "JSONParser.bison"
{
    size_t token_index = ((yystack_[0].value.token_index));
    (yylhs.value.node_index) = interpreter.push_leaf(wasp::VALUE,"value"
                     ,token_index);
}
#line 774 "JSONParser.cpp"
    break;

  case 16: // decl: ANY_STRING
#line 154 "JSONParser.bison"
    {
        auto token_index = ((yystack_[0].value.token_index));
        std::string quote_less_data = interpreter.token_data(token_index);
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_leaf(wasp::DECL
                                   ,"decl"
                                   ,token_index);
    }
#line 787 "JSONParser.cpp"
    break;

  case 17: // ASSIGNMENT: ":"
#line 162 "JSONParser.bison"
             { (yylhs.value.token_index) = (yystack_[0].value.token_index); }
#line 793 "JSONParser.cpp"
    break;

  case 18: // assignment: ASSIGNMENT
#line 163 "JSONParser.bison"
                        {
             auto token_index = ((yystack_[0].value.token_index));
             (yylhs.value.node_index) = interpreter.push_leaf(wasp::ASSIGN,":",token_index);
            }
#line 802 "JSONParser.cpp"
    break;

  case 19: // declaration: decl assignment
#line 168 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 812 "JSONParser.cpp"
    break;

  case 20: // array: lbracket rbracket
#line 176 "JSONParser.bison"
    {
        (yylhs.value.node_indices) = new std::vector<size_t>();
        (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
        (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    }
#line 822 "JSONParser.cpp"
    break;

  case 21: // array: lbracket "end of file"
#line 182 "JSONParser.bison"
    {
        error(yystack_[0].location, "array has unmatched left bracket!");
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 832 "JSONParser.cpp"
    break;

  case 22: // array: lbracket array_members "end of file"
#line 188 "JSONParser.bison"
    {
        std::string name = "object";
        auto last_component_type = interpreter.type((yystack_[1].value.node_indices)->back());
        if( (yystack_[1].value.node_indices)->size() ==0 ) error(yystack_[2].location, name+" has unmatched left bracket!");
        else if( last_component_type == wasp::OBJECT ) error(yystack_[2].location, name+" or one of its components has unmatched left bracket!");
        else error(yystack_[2].location, name+" has unmatched left bracket!");
        delete (yystack_[1].value.node_indices);
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 847 "JSONParser.cpp"
    break;

  case 23: // array: lbracket array_members rbracket
#line 199 "JSONParser.bison"
    {
            (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(),(yystack_[2].value.node_index));
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
            (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    }
#line 857 "JSONParser.cpp"
    break;

  case 24: // object: lbrace rbrace
#line 205 "JSONParser.bison"
    {
        (yylhs.value.node_indices) = new std::vector<size_t>();
        (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
        (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
    }
#line 867 "JSONParser.cpp"
    break;

  case 25: // object: lbrace "end of file"
#line 211 "JSONParser.bison"
    {
        error(yystack_[0].location, "object has unmatched left brace!");
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 877 "JSONParser.cpp"
    break;

  case 26: // object: lbrace object_members "end of file"
#line 217 "JSONParser.bison"
    {
        // TODO capture partial definition
        std::string name = "object";
        auto last_component_type = interpreter.type((yystack_[1].value.node_indices)->back());
        if( (yystack_[1].value.node_indices)->size() ==0 ) error(yystack_[2].location, name+" has unmatched left brace!");
        else if( last_component_type == wasp::OBJECT ) error(yystack_[2].location, name+" or one of its components has unmatched left brace!");
        else error(yystack_[2].location, name+" has unmatched left brace!");
        delete (yystack_[1].value.node_indices);
        YYERROR;
        (yylhs.value.node_indices) = nullptr;
    }
#line 893 "JSONParser.cpp"
    break;

  case 27: // object: lbrace object_members rbrace
#line 229 "JSONParser.bison"
    {
        (yystack_[1].value.node_indices)->insert((yystack_[1].value.node_indices)->begin(),(yystack_[2].value.node_index));
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
        (yylhs.value.node_indices) = (yystack_[1].value.node_indices);
    }
#line 903 "JSONParser.cpp"
    break;

  case 28: // keyed_primitive: declaration primitive
#line 237 "JSONParser.bison"
    {
        (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_index));
        std::string quote_less_data = interpreter.data((yystack_[1].value.node_indices)->front());
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_parent(wasp::KEYED_VALUE
                                    ,quote_less_data.c_str()
                                    ,*(yystack_[1].value.node_indices));
        delete (yystack_[1].value.node_indices);
    }
#line 917 "JSONParser.cpp"
    break;

  case 29: // keyed_object: declaration object
#line 247 "JSONParser.bison"
    {
        for( size_t i = 0; i < (yystack_[0].value.node_indices)->size(); ++i )
        {
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_indices)->at(i));
        }
        std::string quote_less_data = interpreter.data((yystack_[1].value.node_indices)->front());
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_parent(wasp::OBJECT
                                    ,quote_less_data.c_str()
                                    ,*(yystack_[1].value.node_indices));
        delete (yystack_[1].value.node_indices);
        delete (yystack_[0].value.node_indices);
    }
#line 935 "JSONParser.cpp"
    break;

  case 30: // keyed_array: declaration array
#line 261 "JSONParser.bison"
    {
        for( size_t i = 0; i < (yystack_[0].value.node_indices)->size(); ++i )
        {
            (yystack_[1].value.node_indices)->push_back((yystack_[0].value.node_indices)->at(i));
        }
        std::string quote_less_data = interpreter.data((yystack_[1].value.node_indices)->front());
        quote_less_data = wasp::strip_quotes(quote_less_data);
        (yylhs.value.node_index) = interpreter.push_parent(wasp::ARRAY
                                    ,quote_less_data.c_str()
                                    ,*(yystack_[1].value.node_indices));
        delete (yystack_[1].value.node_indices);
        delete (yystack_[0].value.node_indices);
    }
#line 953 "JSONParser.cpp"
    break;

  case 31: // array_members: object
#line 275 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(obj_i);
            delete (yystack_[0].value.node_indices);
        }
#line 966 "JSONParser.cpp"
    break;

  case 32: // array_members: array_members comma object
#line 284 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            size_t obj_i = interpreter.push_parent(wasp::OBJECT
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(obj_i);
            delete (yystack_[0].value.node_indices);
        }
#line 980 "JSONParser.cpp"
    break;

  case 33: // array_members: array
#line 294 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(arr_i);
            delete (yystack_[0].value.node_indices);
        }
#line 993 "JSONParser.cpp"
    break;

  case 34: // array_members: array_members comma array
#line 303 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            size_t arr_i = interpreter.push_parent(wasp::ARRAY
                                        ,"value"
                                        ,*(yystack_[0].value.node_indices));
            (yylhs.value.node_indices)->push_back(arr_i);
            delete (yystack_[0].value.node_indices);
        }
#line 1007 "JSONParser.cpp"
    break;

  case 35: // array_members: primitive
#line 313 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1016 "JSONParser.cpp"
    break;

  case 36: // array_members: array_members comma primitive
#line 318 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1026 "JSONParser.cpp"
    break;

  case 37: // object_members: keyed_object
#line 324 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1035 "JSONParser.cpp"
    break;

  case 38: // object_members: object_members comma keyed_object
#line 329 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1045 "JSONParser.cpp"
    break;

  case 39: // object_members: keyed_array
#line 335 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1054 "JSONParser.cpp"
    break;

  case 40: // object_members: object_members comma keyed_array
#line 340 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1064 "JSONParser.cpp"
    break;

  case 41: // object_members: keyed_primitive
#line 346 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = new std::vector<size_t>();
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1073 "JSONParser.cpp"
    break;

  case 42: // object_members: object_members comma keyed_primitive
#line 351 "JSONParser.bison"
        {
            (yylhs.value.node_indices) = (yystack_[2].value.node_indices);
            (yylhs.value.node_indices)->push_back((yystack_[1].value.node_index));
            (yylhs.value.node_indices)->push_back((yystack_[0].value.node_index));
        }
#line 1083 "JSONParser.cpp"
    break;

  case 44: // start: object
#line 357 "JSONParser.bison"
                {
            interpreter.staged_type(0) = wasp::OBJECT;
            interpreter.push_staged_child(*(yystack_[0].value.node_indices));
            delete (yystack_[0].value.node_indices);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 1094 "JSONParser.cpp"
    break;

  case 45: // start: array
#line 363 "JSONParser.bison"
               {
            interpreter.staged_type(0) = wasp::ARRAY;
            interpreter.push_staged_child(*(yystack_[0].value.node_indices));
            delete (yystack_[0].value.node_indices);
            if(interpreter.single_parse() ) {lexer->rewind();YYACCEPT;}
        }
#line 1105 "JSONParser.cpp"
    break;


#line 1109 "JSONParser.cpp"

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
  JSONParser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  JSONParser::yytnamerr_ (const char *yystr)
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
  JSONParser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // JSONParser::context.
  JSONParser::context::context (const JSONParser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  JSONParser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
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
  JSONParser::yy_syntax_error_arguments_ (const context& yyctx,
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
  JSONParser::yysyntax_error_ (const context& yyctx) const
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


  const signed char JSONParser::yypact_ninf_ = -13;

  const signed char JSONParser::yytable_ninf_ = -1;

  const signed char
  JSONParser::yypact_[] =
  {
       0,   -13,   -13,     5,     3,   -13,   -13,    22,   -13,   -13,
     -13,   -13,   -13,    14,    15,   -13,   -13,   -13,    24,   -13,
     -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,    33,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   -13,   -13,    20,   -13,   -13,    15,   -13,   -13,   -13,
     -13,   -13,   -13,   -13
  };

  const signed char
  JSONParser::yydefact_[] =
  {
      43,     7,     5,     0,     0,    45,    44,     0,    25,     6,
       9,    24,    16,     0,     0,    41,    37,    39,     0,    21,
       8,    11,     3,     4,    14,    12,    10,    13,    20,    15,
      35,    33,    31,     0,     1,    17,    18,    19,    28,    30,
      29,    26,     2,     0,    27,    22,     0,    23,    42,    38,
      40,    36,    34,    32
  };

  const signed char
  JSONParser::yypgoto_[] =
  {
     -13,     4,   -13,   -13,    18,   -13,     6,   -13,   -13,   -12,
     -13,   -13,   -13,   -13,    -4,    -3,     1,     2,     7,   -13,
     -13,   -13
  };

  const signed char
  JSONParser::yydefgoto_[] =
  {
       0,    43,    27,     3,    11,     4,    28,    12,    29,    30,
      13,    36,    37,    14,     5,     6,    15,    16,    17,    33,
      18,     7
  };

  const signed char
  JSONParser::yytable_[] =
  {
      31,    32,    38,    19,     1,     8,     2,     1,    20,     2,
      39,    40,     9,    21,    22,    23,    24,    25,    26,     1,
      10,     2,    34,    35,    41,    21,    22,    23,    24,    25,
      26,     9,    42,    45,    51,    10,    44,    46,    20,    47,
       0,    42,    52,    53,    48,    49,     0,     0,     0,     0,
      50
  };

  const signed char
  JSONParser::yycheck_[] =
  {
       4,     4,    14,     0,     4,     0,     6,     4,     5,     6,
      14,    14,     7,    10,    11,    12,    13,    14,    15,     4,
      15,     6,     0,     9,     0,    10,    11,    12,    13,    14,
      15,     7,     8,     0,    46,    15,    18,    33,     5,    33,
      -1,     8,    46,    46,    43,    43,    -1,    -1,    -1,    -1,
      43
  };

  const signed char
  JSONParser::yystos_[] =
  {
       0,     4,     6,    19,    21,    30,    31,    37,     0,     7,
      15,    20,    23,    26,    29,    32,    33,    34,    36,     0,
       5,    10,    11,    12,    13,    14,    15,    18,    22,    24,
      25,    30,    31,    35,     0,     9,    27,    28,    25,    30,
      31,     0,     8,    17,    20,     0,    17,    22,    32,    33,
      34,    25,    30,    31
  };

  const signed char
  JSONParser::yyr1_[] =
  {
       0,    16,    17,    18,    18,    19,    20,    21,    22,    23,
      24,    24,    24,    24,    24,    25,    26,    27,    28,    29,
      30,    30,    30,    30,    31,    31,    31,    31,    32,    33,
      34,    35,    35,    35,    35,    35,    35,    36,    36,    36,
      36,    36,    36,    37,    37,    37
  };

  const signed char
  JSONParser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     2,     3,     3,     2,     2,     3,     3,     2,     2,
       2,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     0,     1,     1
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const JSONParser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "\"end of line\"",
  "\"[\"", "\"]\"", "\"{\"", "\"}\"", "\",\"", "\":\"", "\"integer\"",
  "\"true\"", "\"false\"", "\"null\"", "\"double\"", "\"quoted string\"",
  "$accept", "comma", "BOOLEAN", "lbrace", "rbrace", "lbracket",
  "rbracket", "ANY_STRING", "PRIMITIVE", "primitive", "decl", "ASSIGNMENT",
  "assignment", "declaration", "array", "object", "keyed_primitive",
  "keyed_object", "keyed_array", "array_members", "object_members",
  "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const short
  JSONParser::yyrline_[] =
  {
       0,   116,   116,   121,   121,   123,   128,   133,   138,   144,
     145,   145,   145,   145,   145,   147,   153,   162,   163,   167,
     175,   181,   187,   198,   204,   210,   216,   228,   236,   246,
     260,   274,   283,   293,   302,   312,   317,   323,   328,   334,
     339,   345,   350,   356,   357,   363
  };

  void
  JSONParser::yy_stack_print_ () const
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
  JSONParser::yy_reduce_print_ (int yyrule) const
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

  JSONParser::symbol_kind_type
  JSONParser::yytranslate_ (int t)
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
      15
    };
    // Last valid token kind.
    const int code_max = 270;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return YY_CAST (symbol_kind_type, translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 34 "JSONParser.bison"
} // wasp
#line 1661 "JSONParser.cpp"

#line 372 "JSONParser.bison"
 /*** Additional Code ***/

void wasp::JSONParser::error(const JSONParser::location_type& l,
                           const std::string& m)
{
    interpreter.error_stream()<<l<<": "<<m<<std::endl;
}
