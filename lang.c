/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "lang.y"


 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int yylex();


void yyerror(char *s)
{
	fflush(stdout);
	fprintf(stderr, "%s\n", s);
}

/***************************************************************************/
/* Data structures for storing a program.                                */


typedef struct var	// a variable
{
	char *name;
	char *value;
	struct var *next;
} var;


typedef struct expr	// an expression
{
	int type;	// type of expression 
	var *var;   // variable linked to the expression (if there is one)
	struct expr *left, *right; // Left and right subexpression (if there is one)
    char *val; // Value of the expression (if there is one)
} expr;

typedef struct stmt	// command
{
	int type;	// type of statement  
	var *var;   // variable linked to the statement
	expr *expr; // expression linked to the statement
	struct stmt *left, *right; // Left and right sub-statements
    char *val; // Value of the expression
} stmt;

typedef struct strats
{
    char *name; // Name of strategy
    stmt *s; // Statement of strategy
    struct strats *next; // Next strategy
    var *defs; // Definitions of strategy
    int weight;
    int num_agents;
} strats;

typedef struct agt
{
	int id; //id of the agent
	strats *st; //strategie
	int lf; //life points remaining
	int spw; //spawn points remaining
	int lst;
} agt;

/****************************************************************************/
/* All data pertaining to the programme are accessible from these two vars. */

var *program_vars; // List of program constants (defined at the end)
strats *program_strats; // List of program strats
var *current_defs; // List of current definitions (at parsing)

int duration;
int rewardhh;
int rewardcc;
int rewardhc;
int rewardch;
int initial;
int meetings;
int spawn;
int intervals;
int life;
int mutation;

/****************************************************************************/
/* Functions for setting up data structures at parse time.                 */

// Makes an identifier for a constant of name s and value val
var* make_ident (char *s, char *val)
{
	var *v = malloc(sizeof(var));
	v->name = s;
	v->value = val;	
	v->next = NULL;
    program_vars = v;


    if(!strcmp(s,"duration")) duration = atoi(val);
    if(!strcmp(s,"rewardHH")) rewardhh = atoi(val);
    if(!strcmp(s,"rewardCC")) rewardcc = atoi(val);
    if(!strcmp(s,"rewardHC")) rewardhc = atoi(val);
    if(!strcmp(s,"rewardCH")) rewardch = atoi(val);
    if(!strcmp(s,"initial")) initial = atoi(val);
    if(!strcmp(s,"meetings")) meetings = atoi(val);
    if(!strcmp(s,"spawn")) spawn = atoi(val);
    if(!strcmp(s,"intervals")) intervals = atoi(val);
    if(!strcmp(s,"life")) life = atoi(val);
    if (!strcmp(s,"mutation")) mutation = atoi(val);

	return v;
}

// Similar function for definitions
var* make_def (char *s, expr *e)
{
	var *v = malloc(sizeof(var));
	v->name = s;
	v->value = e->val;	
	v->next = NULL;
    current_defs = v;
	return v;
}

// Finds the definition or constant corresponding to the identifier s
var* find_ident (char *s)
{
	var *v = current_defs;
	while (v && strcmp(v->name,s)) v = v->next;
	if (!v) { yyerror("undeclared variable"); exit(1); }
	return v;
}

// Makes an expression with the given parameters
expr* make_expr (int type, var *var, expr *left, expr *right, char *val)
{
	expr *e = malloc(sizeof(expr));
	e->type = type;
	e->var = var;
    e->val = val;
	e->left = left;
	e->right = right;
	return e;
}

// Makes a statement with the given parameters
stmt* make_stmt (int type, var *var, expr *expr,
			stmt *left, stmt *right)
{
	stmt *s = malloc(sizeof(stmt));
	s->type = type;
	s->var = var;
	s->expr = expr;
	s->left = left;
	s->right = right;
	return s;
}

// Makes a strategy with the given parameters 
strats* make_strat(stmt *s, char *name, strats *next, var *defs, int weight) {
    strats *st = malloc(sizeof(strats));
    st->s = s;
    st->name = name;
    st->next = next;
    st->defs = defs;
    st->weight = weight;
    st->num_agents = 0;
    return st;
}

strats* find_strat (char *s)
{
	strats *v = program_strats;
	while (v && strcmp(v->name,s)) v = v->next;
	if (!v) { yyerror("unexistent strategie"); exit(1); }
	return v;
}

agt* make_agt (int id, strats *strat, int life, int spawn, int last)
{
	agt *a = malloc(sizeof(agt));
	a->id = id;
	a->st = strat;
    a->lf = life;
	a->spw = spawn;
	a->lst = last;
	return a;
}


#line 261 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ASSIGN = 258,                  /* ASSIGN  */
    OR = 259,                      /* OR  */
    AND = 260,                     /* AND  */
    NOT = 261,                     /* NOT  */
    EQ = 262,                      /* EQ  */
    IF = 263,                      /* IF  */
    ELSE = 264,                    /* ELSE  */
    STRAT = 265,                   /* STRAT  */
    CONST = 266,                   /* CONST  */
    DEF = 267,                     /* DEF  */
    CHEAT = 268,                   /* CHEAT  */
    HONEST = 269,                  /* HONEST  */
    RET = 270,                     /* RET  */
    INF = 271,                     /* INF  */
    UNDEF = 274,                   /* UNDEF  */
    LAST = 275,                    /* LAST  */
    ADD = 276,                     /* ADD  */
    MINUS = 277,                   /* MINUS  */
    IDENT = 278,                   /* IDENT  */
    IDSTRAT = 279,                 /* IDSTRAT  */
    NUM = 280,                     /* NUM  */
    RAND = 281,                    /* RAND  */
    SUP = 282                      /* SUP  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif
/* Token kinds.  */
#define YYEMPTY -2
#define YYEOF 0
#define YYerror 256
#define YYUNDEF 257
#define ASSIGN 258
#define OR 259
#define AND 260
#define NOT 261
#define EQ 262
#define IF 263
#define ELSE 264
#define STRAT 265
#define CONST 266
#define DEF 267
#define CHEAT 268
#define HONEST 269
#define RET 270
#define INF 271
#define UNDEF 274
#define LAST 275
#define ADD 276
#define MINUS 277
#define IDENT 278
#define IDSTRAT 279
#define NUM 280
#define RAND 281
#define SUP 282

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 195 "lang.y"

	char *i;
	var *v;
	expr *e;
	stmt *s;
    strats *st;

#line 369 "y.tab.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ASSIGN = 3,                     /* ASSIGN  */
  YYSYMBOL_OR = 4,                         /* OR  */
  YYSYMBOL_AND = 5,                        /* AND  */
  YYSYMBOL_NOT = 6,                        /* NOT  */
  YYSYMBOL_EQ = 7,                         /* EQ  */
  YYSYMBOL_IF = 8,                         /* IF  */
  YYSYMBOL_ELSE = 9,                       /* ELSE  */
  YYSYMBOL_STRAT = 10,                     /* STRAT  */
  YYSYMBOL_CONST = 11,                     /* CONST  */
  YYSYMBOL_DEF = 12,                       /* DEF  */
  YYSYMBOL_CHEAT = 13,                     /* CHEAT  */
  YYSYMBOL_HONEST = 14,                    /* HONEST  */
  YYSYMBOL_RET = 15,                       /* RET  */
  YYSYMBOL_INF = 16,                       /* INF  */
  YYSYMBOL_17_INF_EQ = 17,                 /* INF-EQ  */
  YYSYMBOL_18_SUP_EQ = 18,                 /* SUP-EQ  */
  YYSYMBOL_UNDEF = 19,                     /* UNDEF  */
  YYSYMBOL_LAST = 20,                      /* LAST  */
  YYSYMBOL_ADD = 21,                       /* ADD  */
  YYSYMBOL_MINUS = 22,                     /* MINUS  */
  YYSYMBOL_IDENT = 23,                     /* IDENT  */
  YYSYMBOL_IDSTRAT = 24,                   /* IDSTRAT  */
  YYSYMBOL_NUM = 25,                       /* NUM  */
  YYSYMBOL_RAND = 26,                      /* RAND  */
  YYSYMBOL_27_ = 27,                       /* ';'  */
  YYSYMBOL_SUP = 28,                       /* SUP  */
  YYSYMBOL_29_ = 29,                       /* '('  */
  YYSYMBOL_30_ = 30,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_prog = 32,                      /* prog  */
  YYSYMBOL_declist = 33,                   /* declist  */
  YYSYMBOL_strats = 34,                    /* strats  */
  YYSYMBOL_defs = 35,                      /* defs  */
  YYSYMBOL_stmt = 36,                      /* stmt  */
  YYSYMBOL_expr = 37,                      /* expr  */
  YYSYMBOL_term = 38,                      /* term  */
  YYSYMBOL_arithmetic = 39,                /* arithmetic  */
  YYSYMBOL_ret = 40                        /* ret  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  5
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   125

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  10
/* YYNRULES -- Number of rules.  */
#define YYNRULES  40
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  79

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      29,    30,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    27,
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
      25,    26,    28
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   238,   238,   239,   241,   242,   244,   245,   246,   247,
     249,   250,   252,   253,   255,   257,   259,   263,   264,   265,
     266,   267,   268,   269,   270,   271,   272,   273,   275,   276,
     277,   278,   279,   281,   282,   283,   284,   286,   287,   288,
     289
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ASSIGN", "OR", "AND",
  "NOT", "EQ", "IF", "ELSE", "STRAT", "CONST", "DEF", "CHEAT", "HONEST",
  "RET", "INF", "INF-EQ", "SUP-EQ", "UNDEF", "LAST", "ADD", "MINUS",
  "IDENT", "IDSTRAT", "NUM", "RAND", "';'", "SUP", "'('", "')'", "$accept",
  "prog", "declist", "strats", "defs", "stmt", "expr", "term",
  "arithmetic", "ret", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-39)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      15,   -14,    36,     5,    16,   -39,    25,   -39,    26,    -4,
      27,    32,    29,    63,    40,   102,    64,    39,    43,    -4,
      67,   -39,    63,   -39,   -39,   -39,   -39,   -39,   -39,   -39,
      63,    57,   -39,     2,    68,   -39,   -39,   -39,   -39,   -39,
      87,    -2,    48,    39,    54,    -4,    80,    30,    63,    63,
      63,    63,    63,    63,    63,    72,   -18,   -18,    87,   -39,
     -39,   -39,   -39,    39,   -39,    86,    80,    80,    22,     0,
      66,    24,    -2,     2,    73,   -39,    -4,   -39,    39
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,    10,     1,     0,     4,     0,     0,
      10,     3,     0,     0,     0,     0,     0,     6,     0,     0,
       0,    10,     0,    28,    29,    30,    31,    34,    33,    25,
       0,     0,    26,    32,     0,    38,    39,    40,    37,    15,
       0,     0,     0,     7,     0,     0,    24,     0,     0,     0,
       0,     0,     0,     0,     0,    13,     0,     0,     0,    16,
      12,    10,     5,     8,    27,    17,    18,    19,    20,    22,
      23,    21,     0,    35,    36,    11,     0,    14,     9
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -39,   -39,   -39,   -39,    -7,   -19,   -21,   -38,   -13,   -39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,    11,     3,     9,    17,    31,    32,    33,    39
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      43,    46,    59,    19,    13,    27,    13,    28,    14,    47,
       4,    15,    55,    15,    45,     6,     7,    52,    53,    16,
      75,    16,    60,    56,    57,     1,    63,    65,    66,    67,
      68,    69,    70,    71,    48,    49,     5,    50,    51,    52,
      53,    52,    53,    73,    74,     8,    51,    52,    53,    10,
      54,    12,    54,    77,    76,    20,    18,    78,    54,    21,
      64,    48,    49,    34,    50,    13,    41,    40,    42,    22,
      44,    58,    15,    51,    52,    53,    23,    24,    61,    62,
      16,    72,    25,    26,    53,    54,    27,    50,    28,    29,
       0,    49,    30,    50,     0,    57,    51,    52,    53,     0,
      23,    24,    51,    52,    53,     0,    25,    26,    54,     0,
      27,     0,    28,     0,    54,    35,    36,     0,     0,     0,
       0,     0,    37,     0,     0,    38
};

static const yytype_int8 yycheck[] =
{
      19,    22,    40,    10,     8,    23,     8,    25,    12,    30,
      24,    15,    31,    15,    21,    10,    11,    17,    18,    23,
      58,    23,    41,    21,    22,    10,    45,    48,    49,    50,
      51,    52,    53,    54,     4,     5,     0,     7,    16,    17,
      18,    17,    18,    56,    57,    29,    16,    17,    18,    24,
      28,    25,    28,    72,    61,    23,    29,    76,    28,    30,
      30,     4,     5,    23,     7,     8,    27,     3,    25,     6,
       3,     3,    15,    16,    17,    18,    13,    14,    30,    25,
      23,     9,    19,    20,    18,    28,    23,     7,    25,    26,
      -1,     5,    29,     7,    -1,    22,    16,    17,    18,    -1,
      13,    14,    16,    17,    18,    -1,    19,    20,    28,    -1,
      23,    -1,    25,    -1,    28,    13,    14,    -1,    -1,    -1,
      -1,    -1,    20,    -1,    -1,    23
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    10,    32,    34,    24,     0,    10,    11,    29,    35,
      24,    33,    25,     8,    12,    15,    23,    36,    29,    35,
      23,    30,     6,    13,    14,    19,    20,    23,    25,    26,
      29,    37,    38,    39,    23,    13,    14,    20,    23,    40,
       3,    27,    25,    36,     3,    35,    37,    37,     4,     5,
       7,    16,    17,    18,    28,    36,    21,    22,     3,    38,
      36,    30,    25,    36,    30,    37,    37,    37,    37,    37,
      37,    37,     9,    39,    39,    38,    35,    36,    36
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    32,    33,    33,    34,    34,    34,    34,
      35,    35,    36,    36,    36,    36,    36,    37,    37,    37,
      37,    37,    37,    37,    37,    37,    37,    37,    38,    38,
      38,    38,    38,    39,    39,    39,    39,    40,    40,    40,
      40
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     3,     0,     4,     4,     5,     7,     8,
       0,     5,     3,     3,     5,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     2,     1,     1,     3,     1,     1,
       1,     1,     1,     1,     1,     3,     3,     1,     1,     1,
       1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* prog: strats  */
#line 238 "lang.y"
             { program_strats = (yyvsp[0].st); }
#line 1438 "y.tab.c"
    break;

  case 3: /* prog: strats CONST declist  */
#line 239 "lang.y"
                           { program_strats = (yyvsp[-2].st); }
#line 1444 "y.tab.c"
    break;

  case 4: /* declist: %empty  */
#line 241 "lang.y"
           { (yyval.v) = NULL; program_vars = NULL; }
#line 1450 "y.tab.c"
    break;

  case 5: /* declist: declist IDENT ASSIGN NUM  */
#line 242 "lang.y"
                                        { ((yyval.v) = make_ident((yyvsp[-2].i),(yyvsp[0].i)))->next = (yyvsp[-3].v); }
#line 1456 "y.tab.c"
    break;

  case 6: /* strats: STRAT IDSTRAT defs stmt  */
#line 244 "lang.y"
                                { (yyval.st) = make_strat((yyvsp[0].s),(yyvsp[-2].i),NULL,(yyvsp[-1].v),1); }
#line 1462 "y.tab.c"
    break;

  case 7: /* strats: strats STRAT IDSTRAT defs stmt  */
#line 245 "lang.y"
                                     { (yyval.st) = make_strat((yyvsp[0].s),(yyvsp[-2].i),(yyvsp[-4].st),(yyvsp[-1].v),1); }
#line 1468 "y.tab.c"
    break;

  case 8: /* strats: STRAT IDSTRAT '(' NUM ')' defs stmt  */
#line 246 "lang.y"
                                          { (yyval.st) = make_strat((yyvsp[0].s),(yyvsp[-5].i),NULL,(yyvsp[-1].v),atoi((yyvsp[-3].i))); }
#line 1474 "y.tab.c"
    break;

  case 9: /* strats: strats STRAT IDSTRAT '(' NUM ')' defs stmt  */
#line 247 "lang.y"
                                                 { (yyval.st) = make_strat((yyvsp[0].s),(yyvsp[-5].i),(yyvsp[-7].st),(yyvsp[-1].v),atoi((yyvsp[-3].i))); }
#line 1480 "y.tab.c"
    break;

  case 10: /* defs: %empty  */
#line 249 "lang.y"
      { (yyval.v) = NULL; current_defs = NULL; }
#line 1486 "y.tab.c"
    break;

  case 11: /* defs: defs DEF IDENT ASSIGN term  */
#line 250 "lang.y"
                                 { ((yyval.v) = make_def((yyvsp[-2].i),(yyvsp[0].e)))->next = (yyvsp[-4].v); }
#line 1492 "y.tab.c"
    break;

  case 12: /* stmt: stmt ';' stmt  */
#line 252 "lang.y"
                     { (yyval.s) = make_stmt(';',NULL,NULL,(yyvsp[-2].s),(yyvsp[0].s)); }
#line 1498 "y.tab.c"
    break;

  case 13: /* stmt: IF expr stmt  */
#line 254 "lang.y"
        { (yyval.s) = make_stmt(IF,NULL,(yyvsp[-1].e),(yyvsp[0].s),NULL); }
#line 1504 "y.tab.c"
    break;

  case 14: /* stmt: IF expr stmt ELSE stmt  */
#line 256 "lang.y"
        { (yyval.s) = make_stmt(ELSE,NULL,(yyvsp[-3].e),(yyvsp[-2].s),(yyvsp[0].s)); }
#line 1510 "y.tab.c"
    break;

  case 15: /* stmt: RET ret  */
#line 258 "lang.y"
        { (yyval.s) = make_stmt(RET,NULL,(yyvsp[0].e),NULL,NULL); }
#line 1516 "y.tab.c"
    break;

  case 16: /* stmt: IDENT ASSIGN term  */
#line 260 "lang.y"
        { (yyval.s) = make_stmt(ASSIGN,find_ident((yyvsp[-2].i)),(yyvsp[0].e),NULL,NULL); }
#line 1522 "y.tab.c"
    break;

  case 17: /* expr: expr OR expr  */
#line 263 "lang.y"
                        { (yyval.e) = make_expr(OR,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1528 "y.tab.c"
    break;

  case 18: /* expr: expr AND expr  */
#line 264 "lang.y"
                        { (yyval.e) = make_expr(AND,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1534 "y.tab.c"
    break;

  case 19: /* expr: expr EQ expr  */
#line 265 "lang.y"
                        { (yyval.e) = make_expr(EQ,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1540 "y.tab.c"
    break;

  case 20: /* expr: expr INF expr  */
#line 266 "lang.y"
                        { (yyval.e) = make_expr(INF,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1546 "y.tab.c"
    break;

  case 21: /* expr: expr SUP expr  */
#line 267 "lang.y"
                        { (yyval.e) = make_expr(SUP,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1552 "y.tab.c"
    break;

  case 22: /* expr: expr INF-EQ expr  */
#line 268 "lang.y"
                                { (yyval.e) = make_expr(INF-EQ,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1558 "y.tab.c"
    break;

  case 23: /* expr: expr SUP-EQ expr  */
#line 269 "lang.y"
                                { (yyval.e) = make_expr(SUP-EQ,NULL,(yyvsp[-2].e),(yyvsp[0].e), NULL); }
#line 1564 "y.tab.c"
    break;

  case 24: /* expr: NOT expr  */
#line 270 "lang.y"
                            { (yyval.e) = make_expr(NOT,NULL,(yyvsp[0].e),NULL, NULL); }
#line 1570 "y.tab.c"
    break;

  case 25: /* expr: RAND  */
#line 271 "lang.y"
                    { (yyval.e) = make_expr(RAND, NULL, NULL, NULL, (yyvsp[0].i)); }
#line 1576 "y.tab.c"
    break;

  case 26: /* expr: term  */
#line 272 "lang.y"
                    { (yyval.e) = (yyvsp[0].e); }
#line 1582 "y.tab.c"
    break;

  case 27: /* expr: '(' expr ')'  */
#line 273 "lang.y"
                        { (yyval.e) = (yyvsp[-1].e); }
#line 1588 "y.tab.c"
    break;

  case 28: /* term: CHEAT  */
#line 275 "lang.y"
                { (yyval.e) = make_expr(CHEAT, NULL, NULL, NULL, "Cheat"); }
#line 1594 "y.tab.c"
    break;

  case 29: /* term: HONEST  */
#line 276 "lang.y"
                { (yyval.e) = make_expr(HONEST, NULL, NULL, NULL, "Honest"); }
#line 1600 "y.tab.c"
    break;

  case 30: /* term: UNDEF  */
#line 277 "lang.y"
                { (yyval.e) = make_expr(UNDEF, NULL, NULL, NULL, "Undef"); }
#line 1606 "y.tab.c"
    break;

  case 31: /* term: LAST  */
#line 278 "lang.y"
                { (yyval.e) = make_expr(LAST, NULL, NULL, NULL, "last"); }
#line 1612 "y.tab.c"
    break;

  case 32: /* term: arithmetic  */
#line 279 "lang.y"
                 {(yyval.e) = (yyvsp[0].e);}
#line 1618 "y.tab.c"
    break;

  case 33: /* arithmetic: NUM  */
#line 281 "lang.y"
                       { (yyval.e) = make_expr(NUM, NULL, NULL, NULL, (yyvsp[0].i)); }
#line 1624 "y.tab.c"
    break;

  case 34: /* arithmetic: IDENT  */
#line 282 "lang.y"
                        { (yyval.e) = make_expr(0,find_ident((yyvsp[0].i)),NULL,NULL, NULL); }
#line 1630 "y.tab.c"
    break;

  case 35: /* arithmetic: arithmetic ADD arithmetic  */
#line 283 "lang.y"
                                { (yyval.e) = make_expr(ADD, NULL, (yyvsp[-2].e), (yyvsp[0].e), NULL); }
#line 1636 "y.tab.c"
    break;

  case 36: /* arithmetic: arithmetic MINUS arithmetic  */
#line 284 "lang.y"
                                        { (yyval.e) = make_expr(MINUS, NULL, (yyvsp[-2].e), (yyvsp[0].e), NULL); }
#line 1642 "y.tab.c"
    break;

  case 37: /* ret: IDENT  */
#line 286 "lang.y"
                { (yyval.e) = make_expr(0,find_ident((yyvsp[0].i)),NULL,NULL, NULL); }
#line 1648 "y.tab.c"
    break;

  case 38: /* ret: CHEAT  */
#line 287 "lang.y"
                { (yyval.e) = make_expr(CHEAT, NULL, NULL, NULL, NULL); }
#line 1654 "y.tab.c"
    break;

  case 39: /* ret: HONEST  */
#line 288 "lang.y"
                { (yyval.e) = make_expr(HONEST, NULL, NULL, NULL, NULL); }
#line 1660 "y.tab.c"
    break;

  case 40: /* ret: LAST  */
#line 289 "lang.y"
                { (yyval.e) = make_expr(LAST, NULL, NULL, NULL, "last"); }
#line 1666 "y.tab.c"
    break;


#line 1670 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 291 "lang.y"


#include "langlex.c"

/****************************************************************************/
/* programme interpreter      :                                             */

int random_int(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

int string_to_int(char* s) {
    int i;

    sscanf(s, "%*[^0123456789]%d", &i);

    return i;
}

int last = 'u';

int total_pts;
int pop_totale;
int pop;
int nb_libres;
int agt_morts;
char snum[100];
agt* agents;

/* Evaluates expression e as a term and returns the value as a string */
char *eval_term (expr *e)
{
	switch (e->type)
	{
		case CHEAT: return "Cheat";
        case HONEST: return "Honest";
        case UNDEF: return "Undef";
        case LAST:
            if(last == 'u') return "Undef";
            else if(last == 'h') return "Honest";
            else if(last == 'c') return "Cheat";
            break;
        case RAND: 
            sprintf(snum,"%d",random_int(0,string_to_int(e->val)));
            return snum;
        case NUM:  return e->val;
        case ADD: 
            sprintf(snum,"%d",atoi(eval_term(e->left)) + atoi(eval_term(e->right)));
            return snum;
        case MINUS: 
            sprintf(snum,"%d",atoi(eval_term(e->left)) - atoi(eval_term(e->right)));
            return snum;
		case 0: return e->var->value; 
        break;
	}
}

// Eval expression e as a condition, and returns the value as an int
int eval_cond (expr *e) {
    switch(e->type) {
		case CHEAT: return 'c';
        case HONEST: return 'h';
        case UNDEF: return 'u';
        case LAST:  return last;
        case RAND: return random_int(0,string_to_int(e->val));
        case NUM:  return atoi(e->val);
		case 0: 
            if(!strcmp(e->var->value,"Undef")) return 'u';
            else if(!strcmp(e->var->value, "Honest")) return 'h';
            else if(!strcmp(e->var->value, "Cheat")) return 'c';
            else if(!strcmp(e->var->value, "last")) return last;
            else return atoi(e->var->value); 
        case ADD: return atoi(eval_term(e->left)) + atoi(eval_term(e->right));
        case MINUS: return atoi(eval_term(e->left)) - atoi(eval_term(e->right));
		case OR: return eval_cond(e->left) || eval_cond(e->right);
		case AND: return eval_cond(e->left) && eval_cond(e->right);
		case EQ: return eval_cond(e->left) == eval_cond(e->right);
		case INF: return eval_cond(e->left) < eval_cond(e->right);
		case SUP: return eval_cond(e->left) > eval_cond(e->right);
		case INF-EQ: return eval_cond(e->left) <= eval_cond(e->right);
		case SUP-EQ: return eval_cond(e->left) >= eval_cond(e->right);
		case NOT: return !eval_cond(e->left);
        break;
    }
}

// Execute statement s
int execute (stmt *s)
{
	switch(s->type)
	{
		case ASSIGN:
			s->var->value = eval_term(s->expr);
            return 'u';
		case ';':
			execute(s->left);
			return execute(s->right);
		case IF:
			if (eval_cond(s->expr)) return execute(s->left);
            return 'u';
		case ELSE:
			if (eval_cond(s->expr)) return execute(s->left);
            else return execute(s->right);
        case RET: 
            return eval_cond(s->expr);
	}
}

// Find a random strategy from the list program_strats, taking weights into account
strats *random_strat(void) {
    strats *weighted_strats;
    strats *w;
	strats *v = program_strats;
    int num_strats = 0;
    int r;

    weighted_strats = w = make_strat(v->s,v->name,NULL,v->defs,v->weight); 

    for(int i = 1; i < v->weight; i++) {
            w->next = make_strat(v->s,v->name,NULL,v->defs,v->weight); 
            w = w->next;
            num_strats++;
    }

    v = v->next;

	while (v) {
        for(int i = 0; i < v->weight; i++) {
            w->next = make_strat(v->s,v->name,NULL,v->defs,v->weight); 
            w = w->next;
            num_strats++;
        }

        v = v->next;
    }
    
    r = random_int(0,num_strats);
    w = weighted_strats;

    while(r > 0) {
        r--;
        w = w->next;
    }

    return find_strat(w->name);
}

// Find a new strategy from dad's strategy
strats *new_strat(strats* old_st) {
    int r = random_int(0,100);
    if(r >= 0 && r < mutation) {
        return random_strat();
    } else {
        return old_st;
    }
}

// Updates points for agents a1 and a2
void a_jour_pts(agt *a1, agt *a2,int i, int j){
	a1->spw +=i;
	a1->lf -=1;
	a2->spw +=j;
	a2->lf -=1;
	total_pts+=i+j;
}

//Copy a2 in a1
void copy_agt(agt *a1,agt *a2){
	a1->st=a2->st;
	a1->lf=a2->lf;
	a1->spw=a2->spw;
	a1->lst=a2->lst;
}

//Adds the spawned agent at the end of agents
void ajt_agt(agt *a){ 
	agents[pop].st=new_strat(a->st);
    agents[pop].st->num_agents++;
	agents[pop].lf=life;
	agents[pop].spw=0;
	agents[pop].lst='u';
}

// Prints strategies distribution
void distrib_strat(void) {
	strats *v = program_strats;
	while (v ){
		printf("%f pourcents de %s \n", ((float)(v->num_agents))/((float)pop), v->name);
		v = v->next;
	}
}

// Meeting between agents a1 and a2
void step (agt *a1, agt *a2)
{
    int temp;

    last = a1->lst;
    int res1 = execute((a1->st)->s);
    last = a2->lst;
    int res2 = execute((a2->st)->s);
    a1->lst = res2;
    a2->lst = res1;
	switch (res1)
	{
		case 'h':
			switch(res2)
			{
				case 'h':
					a_jour_pts(a1,a2,rewardhh,rewardhh);
                break;
				
				case 'c':
					a_jour_pts(a1,a2,rewardhc,rewardch);
                break;
			}
        break;
		case 'c':
			switch(execute((a2->st)->s))
				{
					case 'h':
						a_jour_pts(a1,a2,rewardch,rewardhc);
                    break;
					
					case 'c':
						a_jour_pts(a1,a2,rewardcc,rewardcc);
                    break;
				}
        break;
	}
}

// Manages the creation of agents
void refresh(int r1){
	agt *a1=&agents[r1]; // agent number r1
	if (a1->spw >= spawn){ //spawn of an agent
		if (nb_libres>pop){ //case where agents is large enough to welcome a new agent
			ajt_agt(a1);
			pop_totale+=1;
			pop+=1;
		}
		else{ //when agents is too small, we duplicate its size
			agents=realloc(agents,2*pop*sizeof(agt));
			a1=&agents[r1];
			ajt_agt(a1);
			nb_libres = 2*nb_libres;
			pop_totale+=1;
			pop+=1;
		}
		a1->spw-=spawn; //a1 uses his spawn points
	}
}

void dead(int r1, int r2){
	agt *a1=&agents[r1]; // agent number r1
	agt *a2=&agents[r2]; // agent number r2
	if (a1->lf <= 0){ //death of an agent, we permutate it with the last agent of agents and decrease pop
        a1->st->num_agents -= 1; //decrease of the numbre of agent following the strategy
		copy_agt(a1,agents+(pop-1));
		pop-=1;
		agt_morts+=1;
		if (r2==pop){//if a2 was the last agent
			a2=&agents[r1];
		}
	}
	if (a2->lf <= 0){ 
        a2->st->num_agents -= 1; 
		copy_agt(a2,agents+(pop-1));
		pop-=1;
		agt_morts+=1;
	}
}

// Simulates meetings*initials random meetings
void meet(){
	int i;
	int j;
	for (i=0;i<meetings;i++){
		if(pop<=0 || pop==1){ //when the agents are estinguished
			break;
		}
		if (i>0){
			printf("\nIl reste %d agents \n",pop);
			printf("La moyenne de points generes par agents est %f \n", ((float)total_pts/(float)pop_totale));
			printf("La distribution des strategies est :\n");
        	distrib_strat(); //prints the current distribution of strategies
		}
		else {
			printf("\nIl y a initialement %d agents \n",pop);
			printf("La distribution initiale des strategies est :\n");
        	distrib_strat();
			printf("\nDEBUT DE LA SIMULATION\n");
		}
		for (j=0;j<intervals;j++){
			if(pop<=0 || pop==1){ //when the agents are estinguished
				break;
			}
			int r1 = random_int(0,pop-1); //choose a first random agent
			int r2=random_int(0,pop-2); //choose a second, different agent
			if (r2>=r1){
				r2+=1;
			}
            step(&agents[r1],&agents[r2]); //simulates one encounter
			refresh(r1); //update spawn
			refresh(r2);
			dead(r1,r2); //update life points
		}
	}
	if (pop<=0){
		printf("\nIl ne reste plus d'agents\n");
	}
	if (pop==1){
		printf("\nIl reste un seul agent\n");
	}
	if (pop>1){
		printf("\nIl reste au final %d agents \n",pop);
		printf("La moyenne de points generes par agents est %f \n", ((float)total_pts/(float)pop_totale));
		printf("La distribution finale des strategies est :\n");
        distrib_strat();
	}
}

// Create agent i
void create_agent(int i) {
    agents[i].st = random_strat();
    agents[i].st->num_agents += 1;
    agents[i].lf = life;
    agents[i].spw = 0;
    agents[i].lst = 'u';
}

// Initialize agents
void init() {
    int i;
    for(i = 0; i < nb_libres; i++) { //creation of initial agents
        create_agent(i);
    }
    meet(); //launch the meetings
}

/* Prints expression e */
void print_expr (expr *e)
{
	switch (e->type)
	{
		case OR: 
            print_expr(e->left); printf(" || "); print_expr(e->right);
            break;
		case AND: 
            print_expr(e->left); printf(" && "); print_expr(e->right);
            break;
		case EQ: 
            print_expr(e->left); printf(" == "); print_expr(e->right);
            break;
		case INF: 
            print_expr(e->left); printf(" < "); print_expr(e->right);
            break;
		case SUP: 
            print_expr(e->left); printf(" > "); print_expr(e->right);
            break;
		case ADD: 
            print_expr(e->left); printf(" + "); print_expr(e->right);
            break;
		case MINUS: 
            print_expr(e->left); printf(" - "); print_expr(e->right);
            break;
		case NOT: print_expr(e->left); break;
		case CHEAT: printf("Cheat"); break;
        case HONEST: printf("Honest"); break;
        case UNDEF: printf("Undef"); break;
        case LAST: printf("last"); break;
        case RAND: printf("%s", e->val); break;
        case NUM: printf("%s", e->val); break;
		case 0: printf("%s", e->var->name); break;
	}
}


/* Prints statement stmt in form of an AST */
void print_stmt(stmt *s, int tabs)
{
    printf("\n");
    for(int i=0;i<tabs;i++) {
        printf("\t");
    }
    printf("| ");
	switch(s->type)
	{
		case ASSIGN:
            printf("ASSIGN var %s to expr: ", s->var->name);
			print_expr(s->expr);
			break;
		case ';':
			print_stmt(s->left, tabs);
			print_stmt(s->right, tabs);
			break;
		case IF:
            printf("IF: ");
			print_expr(s->expr);
            print_stmt(s->left, tabs+1);
			break;
		case ELSE:
            printf("IF-ELSE: ");
			print_expr(s->expr);
            print_stmt(s->left, tabs+1);
            print_stmt(s->right, tabs+1);
			break;
        case RET: 
            printf("RET: ");
            print_expr(s->expr);
            break;
	}
}

/* Prints strats list st in form of an AST */
void print_strats(strats *st) {
    if(st->next) {
        print_strats(st->next);
    }
    printf("\nSTRAT: %s, weight %d", st->name, st->weight);
    print_stmt(st->s,0);
    printf("\n");
}

/* Translate statement to original code */
void translate_stmt(stmt *s)
{
	switch(s->type)
	{
		case ASSIGN:
            printf("%s = ", s->var->name);
			print_expr(s->expr);
			break;
		case ';':
			translate_stmt(s->left);
            printf(";\n\t");
			translate_stmt(s->right);
			break;
		case IF:
            printf("if ");
			print_expr(s->expr);
            printf(" ");
            translate_stmt(s->left);
			break;
		case ELSE:
            printf("if ");
			print_expr(s->expr);
            printf(" ");
            translate_stmt(s->left);
            printf(" else ");
            translate_stmt(s->right);
			break;
        case RET: 
            printf("return ");
            print_expr(s->expr);
            break;
	}
}

/* Translate defs to original code */
void translate_defs(var *v) {
    if(v->next) {
        translate_defs(v->next);
    }

    printf("\tdef %s = %s\n", v->name, v->value);
}

/* Translates a list of strategies to original code */
void translate_strats(strats *st) {
    if(st->next) {
        translate_strats(st->next);
    }
    printf("Strategy %s\n", st->name);
    if(st->defs) translate_defs(st->defs);
    printf("\t");
    translate_stmt(st->s);
    printf("\n\n");
}

/* Translates constant lists to original code */
void print_constants(var *v) {
    if(v->next) {
        print_constants(v->next);
    }

    printf("\t %s = %s\n", v->name, v->value);
}

/* Translates to original code, given a strategies list and a constants list */
void translate_prog(strats *st, var *v) {
    translate_strats(st);
    printf("Constants\n");
    print_constants(v);
}


/****************************************************************************/

int main (int argc, char **argv)
{
    srand(time(NULL));

	if (argc <= 1) { yyerror("no file specified"); exit(1); }
	yyin = fopen(argv[1],"r");
	if (!yyparse()) print_strats(program_strats);

	total_pts=0; //save the total number of points earned by agents
	pop_totale=initial; //save the total number of agents that once existed
	pop=pop_totale; //the actual number of agents
	nb_libres=pop; //actual size of agents
	agt_morts=0; //number of agents that died
	agents=malloc(pop*sizeof(agt)); //array of living agents

    init(); //starts the simulation
	printf("\nFIN DE LA SIMULATION\n\n");
    free(agents);
}

