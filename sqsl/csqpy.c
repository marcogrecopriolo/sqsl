/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         csqpyparse
#define yylex           csqpylex
#define yyerror         csqpyerror
#define yydebug         csqpydebug
#define yynerrs         csqpynerrs

#define yylval          csqpylval
#define yychar          csqpychar

/* Copy the first part of user declarations.  */
#line 1 "csqpy.y" /* yacc.c:339  */

/*
	CSQPY.y  -  SQSL interpreter parser and scanner

	The 4glWorks application framework
	The Structured Query Scripting Language
	Copyright (C) 1992-2017 Marco Greco (marco@4glworks.com)

	Initial release: Mar 00
	Current release: Jun 17

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "coslc.h"
#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "ccmnc.h"
#include "cfioc.h"
#include "cfnmc.h"
#include "chstc.h"
#include "cexpc.h"
#include "cldpc.h"
#include "crqfc.h"
#include "csqpc.h"
#include "csqrc.h"
#include "crqxp.h"
#include "crsxc.h"
#include "crxuc.h"
#include "crxcc.h"
#include "crxxp.h"
#include "crxfc.h"
#include "crxoc.h"
#include "crxvc.h"

#ifndef YYBISON
#define yyparse yyparse_sqp     /* just in case we decide to have */
#define yylex yylex_sqp         /* multiple parsers */
#define yyerror yyerror_sqp
#define yyval yyval_sqp
#define yylval yylval_sqp
#define yyvs yyvs_sqp
#define yycheck yycheck_sqp
#define yychar yychar_sqp
#define yylhs yylhs_sqp
#define yylen yylen_sqp
#define yydefred yydefred_sqp
#define yydgoto yydgoto_sqp
#define yysindex yysindex_sqp
#define yyrindex yyrindex_sqp
#define yygindex yygindex_sqp
#define yytable yytable_sqp
#endif

/* bison tends to complain about missing ';' for macros
   at the end of a rule - don't be surprised if you see
   random ';', they are there for a reason!
*/
#define FAIL(e)		{ \
			    status=e; \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILSYN		{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILCHECK	if (status) \
			{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define FAILTEST(a)	if (status==a) \
			{ \
			    yyerror(""); \
			    YYERROR; \
			}

#define NUMTOKENS (sizeof(tokenlist)/sizeof(tokenlist_t))

/*
** function & reserved words descriptor list
*/
typedef struct tokenlist
{
    char *tl_name;
    int (*tl_func)();
    int tl_type;
    int tl_alttype;
    int tl_parmcount;
    struct tokenlist *tl_next;
} tokenlist_t;

/*
** parser kludges
*/
#define NOOPTION	0x0000
#define STMTHACK	0x0001
#define EXPRHACK	0x0002
#define LASTHACK	0x0004

#define PREPAREHACK	0x0008
#define DMLHACK		0x0010
#define SELECTHACK	0x0020
#define SQLHACK		0x0040
#define FOREACHHACK	0x0080

#define DTHACK		0x0200
#define FUNCHACK	0x0400

#define RESVARHACK	0x0800
#define SAFECOPYHACK	0x1000

#define AGGREGATEHACK	0x2000
#define VALUESHACK	0x4000

/*
** parser and scanner state
*/
TGLOB parserstate_t *pstate;
static int delimident=-1;

static TGLOB int (*thischar)();

#ifdef PUREPARSER
#define BUFSIZE 32768
#define NEXT_STMT	{ \
			    draintokens_sql(); \
			    YYACCEPT; \
			}
#define MARK_STMT	{ \
			    draintokens_sql(); \
			    markend_sql(pstate->ibufp-1); \
			}
#define DRAIN_STMT	{ \
			    draintokens_sql(); \
			}
#else
#define	NEXT_STMT	YYACCEPT;
#define MARK_STMT
#define DRAIN_STMT
#endif

#define STMTPTR(s) (TOKPTR((s)->sqlstart))

/*
** forward declarations
**
** others
*/
static void markend_sql(int c);
static void endstmt_sql();

/*
** parser
*/
#ifdef PUREPARSER
static void draintokens_sql();
static int fillbuffer_sql();
static int expthischar_sql();
#endif
static int thischar_sql();
static char *getstring_sql(fgw_tsstype **h);
static char *getvar_sql(fgw_tsstype **hd);
static int getdigits_sql(int base);
static int getfloat_sql(int i, double *f);
static int scanner_sql();
static void yyerror(char *s);
static int yylex();


#line 265 "csqpy.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int csqpydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    END = 258,
    INSERT = 259,
    SELECT = 260,
    EXECUTE = 261,
    CALL = 262,
    INTO = 263,
    USING = 264,
    FORMAT = 265,
    CNST = 266,
    VAR = 267,
    FNCT = 268,
    FNCS = 269,
    FNCL = 270,
    FNCV = 271,
    FNDP = 272,
    FNDN = 273,
    PFIXFNCT = 274,
    TOK_ERROR = 275,
    XPRTOK = 276,
    LSTTOK = 277,
    VARTOK = 278,
    VASTOK = 279,
    LASTOK = 280,
    BEGIN = 281,
    IMMEDIATE = 282,
    COMPOUND = 283,
    S_END = 284,
    QUIT = 285,
    FOR = 286,
    FOREACH = 287,
    WHILE = 288,
    CLONE = 289,
    PARENT = 290,
    WAIT = 291,
    IF = 292,
    ELIF = 293,
    ELSE = 294,
    FI = 295,
    THEN = 296,
    DO = 297,
    DONE = 298,
    C_END = 299,
    CONTINUE = 300,
    BREAK = 301,
    EXIT = 302,
    DISPLAY = 303,
    LET = 304,
    DISPOSE = 305,
    INVOKE = 306,
    STORE = 307,
    LOAD = 308,
    UNLOAD = 309,
    WHENEVER = 310,
    PIPE = 311,
    APPEND = 312,
    OUTPUT = 313,
    WRITE = 314,
    INPUT = 315,
    READ = 316,
    WIDTH = 317,
    REJECT = 318,
    BLOCKSIZE = 319,
    DELETE = 320,
    UPDATE = 321,
    CREATE = 322,
    PREPARE = 323,
    FREE = 324,
    CONNECT = 325,
    DISCONNECT = 326,
    S_SET = 327,
    CONNECTION = 328,
    AS = 329,
    USER = 330,
    SOURCE = 331,
    CURSOR = 332,
    WITH = 333,
    HOLD = 334,
    SQLTOK = 335,
    PROCEDURE = 336,
    FUNCTION = 337,
    ERROR = 338,
    LOG = 339,
    STOP = 340,
    DEFAULT = 341,
    PLAIN = 342,
    HTML = 343,
    RETURNING = 344,
    IN = 345,
    AGGREGATE = 346,
    AUTOCOMMIT = 347,
    PATTERN = 348,
    EXTRACTOR = 349,
    FROM = 350,
    TO = 351,
    BY = 352,
    DELIMITER = 353,
    BRIEF = 354,
    VERTICAL = 355,
    FULL = 356,
    DELIMITED = 357,
    REGEXP = 358,
    HEADERS = 359,
    TEMP = 360,
    SCRATCH = 361,
    EXTERNAL = 362,
    SPLIT = 363,
    COUNT = 364,
    COLUMN = 365,
    PICTURE = 366,
    CASE = 367,
    WHEN = 368,
    IS = 369,
    NULLVALUE = 370,
    BETWEEN = 371,
    MATCHES = 372,
    LIKE = 373,
    ESCAPE = 374,
    KWCAST = 375,
    TYPEINT = 376,
    TYPEFLOAT = 377,
    TYPEDEC = 378,
    TYPEMONEY = 379,
    TYPEDTIME = 380,
    TYPEINTVL = 381,
    TYPEDATE = 382,
    TYPESTRING = 383,
    TYPEBYTE = 384,
    YEAR = 385,
    MONTH = 386,
    DAY = 387,
    HOUR = 388,
    MINUTE = 389,
    SECOND = 390,
    FRACTION = 391,
    ASSIGN = 392,
    OR = 393,
    AND = 394,
    EQ = 395,
    NEQ = 396,
    LEQ = 397,
    GEQ = 398,
    NOT = 399,
    DPIPE = 400,
    OPCAST = 401,
    NEG = 402,
    PLUS = 403,
    UNITS = 404
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 192 "csqpy.y" /* yacc.c:355  */

char		*var;		/* variable names */
tokenlist_t	*fntok;		/* function descriptors */
int 		(*func)();	/* function pointers */
pcode_t		pcode;		/* executable expression code */
instruction_t	instr;		/* intermediate instructions */
instruction_t	*insptr;	/* intermediate instruction pointers */
exprstack_t	stack;		/* expression result */
int 		tstart;		/* token start in source - for sql hacks */
fgw_fdesc	*fd;		/* output statements */

#line 464 "csqpy.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE csqpylval;

int csqpyparse (void);



/* Copy the second part of user declarations.  */

#line 481 "csqpy.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

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

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
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


#if ! defined yyoverflow || YYERROR_VERBOSE

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
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
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
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  49
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1551

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  192
/* YYNRULES -- Number of rules.  */
#define YYNRULES  419
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  800

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   404

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   157,     2,     2,     2,
     159,   160,   151,   150,   158,   149,   161,   152,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   162,     2,
     143,   140,   145,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   163,     2,   164,     2,     2,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   132,   133,   134,
     135,   136,   137,   138,   139,   141,   142,   144,   146,   147,
     148,   153,   154,   155,   156
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   290,   290,   290,   302,   314,   326,   338,   338,   347,
     347,   372,   386,   386,   412,   412,   438,   439,   439,   475,
     481,   481,   499,   523,   523,   536,   537,   542,   542,   579,
     612,   620,   612,   624,   624,   634,   644,   644,   661,   661,
     671,   672,   673,   674,   675,   676,   678,   679,   681,   682,
     687,   695,   687,   700,   700,   716,   716,   720,   729,   740,
     720,   750,   752,   760,   750,   768,   768,   773,   774,   776,
     777,   784,   787,   789,   793,   794,   798,   802,   813,   817,
     819,   822,   824,   825,   831,   835,   839,   848,   848,   854,
     855,   856,   860,   861,   865,   866,   867,   868,   883,   898,
     902,   906,   908,   909,   913,   917,   921,   925,   930,   932,
     939,   946,   954,   959,   961,   962,   964,   965,   967,   968,
     970,   995,  1012,  1030,  1031,  1033,  1034,  1036,  1037,  1039,
    1040,  1042,  1043,  1045,  1049,  1057,  1058,  1066,  1066,  1073,
    1079,  1080,  1081,  1082,  1085,  1087,  1088,  1095,  1096,  1097,
    1099,  1101,  1102,  1102,  1113,  1114,  1116,  1117,  1119,  1119,
    1136,  1138,  1139,  1141,  1142,  1144,  1145,  1147,  1147,  1165,
    1166,  1168,  1169,  1171,  1180,  1190,  1191,  1197,  1198,  1205,
    1205,  1221,  1229,  1229,  1237,  1244,  1244,  1259,  1276,  1276,
    1292,  1304,  1306,  1309,  1312,  1315,  1318,  1322,  1323,  1333,
    1341,  1351,  1351,  1358,  1362,  1367,  1370,  1370,  1381,  1391,
    1391,  1407,  1419,  1429,  1434,  1434,  1452,  1452,  1462,  1474,
    1475,  1476,  1478,  1479,  1487,  1488,  1489,  1491,  1492,  1493,
    1495,  1500,  1501,  1501,  1518,  1525,  1544,  1544,  1546,  1546,
    1549,  1550,  1550,  1562,  1566,  1574,  1578,  1578,  1585,  1585,
    1594,  1595,  1597,  1605,  1607,  1614,  1616,  1638,  1641,  1661,
    1664,  1667,  1671,  1673,  1674,  1676,  1689,  1700,  1702,  1713,
    1715,  1717,  1718,  1726,  1727,  1739,  1740,  1745,  1746,  1758,
    1769,  1776,  1790,  1799,  1799,  1819,  1826,  1833,  1840,  1847,
    1854,  1861,  1861,  1872,  1872,  1883,  1883,  1894,  1900,  1906,
    1919,  1920,  1937,  1948,  1949,  1954,  1954,  1982,  1990,  1998,
    2006,  2018,  2025,  2032,  2039,  2048,  2055,  2062,  2069,  2076,
    2083,  2090,  2092,  2096,  2100,  2105,  2110,  2114,  2120,  2126,
    2130,  2135,  2143,  2184,  2190,  2234,  2271,  2275,  2285,  2298,
    2311,  2320,  2329,  2337,  2350,  2361,  2361,  2386,  2386,  2410,
    2411,  2413,  2421,  2426,  2430,  2433,  2437,  2442,  2451,  2455,
    2463,  2466,  2470,  2473,  2477,  2480,  2505,  2508,  2511,  2514,
    2517,  2520,  2524,  2525,  2527,  2528,  2530,  2531,  2533,  2533,
    2558,  2558,  2566,  2566,  2591,  2592,  2601,  2604,  2608,  2609,
    2611,  2619,  2623,  2628,  2631,  2640,  2651,  2651,  2667,  2700,
    2709,  2716,  2725,  2728,  2732,  2735,  2739,  2749,  2760,  2763,
    2767,  2770,  2774,  2777,  2781,  2784,  2788,  2789,  2806,  2813
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "END", "INSERT", "SELECT", "EXECUTE",
  "CALL", "INTO", "USING", "FORMAT", "CNST", "VAR", "FNCT", "FNCS", "FNCL",
  "FNCV", "FNDP", "FNDN", "PFIXFNCT", "TOK_ERROR", "XPRTOK", "LSTTOK",
  "VARTOK", "VASTOK", "LASTOK", "BEGIN", "IMMEDIATE", "COMPOUND", "S_END",
  "QUIT", "FOR", "FOREACH", "WHILE", "CLONE", "PARENT", "WAIT", "IF",
  "ELIF", "ELSE", "FI", "THEN", "DO", "DONE", "C_END", "CONTINUE", "BREAK",
  "EXIT", "DISPLAY", "LET", "DISPOSE", "INVOKE", "STORE", "LOAD", "UNLOAD",
  "WHENEVER", "PIPE", "APPEND", "OUTPUT", "WRITE", "INPUT", "READ",
  "WIDTH", "REJECT", "BLOCKSIZE", "DELETE", "UPDATE", "CREATE", "PREPARE",
  "FREE", "CONNECT", "DISCONNECT", "S_SET", "CONNECTION", "AS", "USER",
  "SOURCE", "CURSOR", "WITH", "HOLD", "SQLTOK", "PROCEDURE", "FUNCTION",
  "ERROR", "LOG", "STOP", "DEFAULT", "PLAIN", "HTML", "RETURNING", "IN",
  "AGGREGATE", "AUTOCOMMIT", "PATTERN", "EXTRACTOR", "FROM", "TO", "BY",
  "DELIMITER", "BRIEF", "VERTICAL", "FULL", "DELIMITED", "REGEXP",
  "HEADERS", "TEMP", "SCRATCH", "EXTERNAL", "SPLIT", "COUNT", "COLUMN",
  "PICTURE", "CASE", "WHEN", "IS", "NULLVALUE", "BETWEEN", "MATCHES",
  "LIKE", "ESCAPE", "KWCAST", "TYPEINT", "TYPEFLOAT", "TYPEDEC",
  "TYPEMONEY", "TYPEDTIME", "TYPEINTVL", "TYPEDATE", "TYPESTRING",
  "TYPEBYTE", "YEAR", "MONTH", "DAY", "HOUR", "MINUTE", "SECOND",
  "FRACTION", "ASSIGN", "OR", "AND", "'='", "EQ", "NEQ", "'<'", "LEQ",
  "'>'", "GEQ", "NOT", "DPIPE", "'-'", "'+'", "'*'", "'/'", "OPCAST",
  "NEG", "PLUS", "UNITS", "'$'", "','", "'('", "')'", "'.'", "':'", "'['",
  "']'", "$accept", "target", "$@1", "$@2", "stmt", "$@3", "$@4", "$@5",
  "$@6", "stmt1", "$@7", "$@8", "fnproc", "stmt2", "$@9", "$@10", "$@11",
  "$@12", "$@13", "$@14", "done", "elif", "endif", "ctrl", "$@15", "$@16",
  "$@17", "$@18", "$@19", "$@20", "$@21", "$@22", "$@23", "$@24", "$@25",
  "do", "then", "lastok", "lastok1", "fselect", "fsel1", "cursor", "whold",
  "plain", "$@26", "output", "append", "input", "input1", "outfmt",
  "errcl", "del", "exitcl", "connect", "as", "user", "conopt", "con",
  "con1", "src", "sql", "$@27", "psql", "sql1", "select", "destcl",
  "aggcl", "$@28", "agglst", "aglst1", "agasn1", "$@29", "aggas1",
  "aglst2", "agasn2", "aggas2", "$@30", "aggas3", "aggas4", "aggas5",
  "aggas6", "plainsub", "$@31", "assign", "$@32", "assign2", "$@33",
  "$@34", "assign3", "assign4", "funcl", "retcl", "retlist", "$@35",
  "retl1", "retl2", "$@36", "$@37", "alist", "intocl", "dstcl", "$@38",
  "dstcl1", "$@39", "dstcl2", "dstcl3", "sql2", "dml", "newstcl", "into",
  "$@40", "tmpcon", "usingres", "$@41", "usingagg", "$@42", "using",
  "$@43", "format", "ftype", "@44", "@45", "ftype1", "fspec", "headers",
  "pattern", "patcl", "pat1", "pdel", "pdel1", "pdel2", "delesc", "rdel",
  "reject", "reject1", "reject2", "t_aexp", "t_exp", "$@46", "$@47",
  "$@48", "$@49", "t_exp1", "t_exp2", "t_bexp", "$@50", "typecl", "ovarcl",
  "ovarcl1", "ivarcl", "dvarcl", "ivarcl1", "funcl1", "$@51", "$@52",
  "fncl1", "countcl", "flist", "rangecl", "qualcl", "castql", "unitcls",
  "unitcle", "fractcl", "unitcl", "casecl", "olcase", "obcase", "lcase",
  "$@53", "lelse", "$@54", "bcase", "$@55", "elsecl", "nullcl", "eqcl",
  "t_elstc", "t_elist", "t_elst1", "$@56", "t_elst2", "t_elst3", "incl",
  "inlist", "betweencl", "mrexcl", "lrexcl", "rrexcl", "escape", "_empty_",
  "draincl", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,   394,
      61,   395,   396,    60,   397,    62,   398,   399,   400,    45,
      43,    42,    47,   401,   402,   403,   404,    36,    44,    40,
      41,    46,    58,    91,    93
};
# endif

#define YYPACT_NINF -582

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-582)))

#define YYTABLE_NINF -419

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-419)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     149,  -582,   748,    36,    36,    79,    59,   422,  -582,    51,
    -582,  -582,  -119,   -90,   -53,  -582,  -582,  -582,   422,  -582,
     -34,  -582,  -582,   -11,   422,  1186,  1186,    26,   422,   -52,
    1235,  -582,  -582,  -582,   -20,    -1,  -582,  -119,    23,  -582,
      20,  -582,    42,    27,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,    32,    74,  -582,    38,    51,   -90,   138,   748,   185,
      53,  -582,   748,  -582,  -582,  1186,   748,   345,   210,  1186,
     813,  1186,  -582,  1186,    68,    68,    27,  -582,  -582,   138,
    1167,    77,   422,   422,   422,  -582,  -582,   111,  1186,   -65,
    -582,  -582,  -582,  -582,  -582,  1186,  1186,  1186,  1186,  1186,
       5,  1186,  1186,  1186,  1186,  1186,  -582,  -582,  1186,  -582,
    1186,  1186,  1186,  1186,  1186,  -582,   -39,  -582,    45,   748,
      81,  -582,  -582,  -582,   319,   217,   217,   221,   223,    48,
     226,    48,  -582,  1507,  -582,    40,  -119,   100,   510,    60,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,   172,  -582,
    -582,   163,   258,  -582,   798,   422,   908,   271,    15,  -582,
     412,   845,  -582,  -582,   138,   141,  -582,  -582,  1094,  -582,
     175,  -582,  1331,  1331,  1331,  1331,  1331,  -582,   182,  -582,
    -582,  -582,   259,    70,    70,   -43,   -43,  1241,   413,  1331,
     143,   466,   919,   919,  1331,   722,  -582,  -582,    27,   146,
     157,   159,  -582,   748,   422,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  1408,   183,   201,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,   217,  -582,    55,  -582,
    -582,   479,  -582,   662,  1445,   300,  -582,   304,  -582,  -582,
    -582,  -582,  -582,   413,   240,   241,  -582,  1331,   180,  -582,
     138,  1186,   233,  -582,  -582,  1186,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,   345,   345,  -582,  -582,
    -582,  -582,   191,  -582,  -582,   422,  1186,  1186,  -582,  -582,
    -582,  1186,  -582,   748,  -582,  -582,  -582,  -582,   192,    20,
     138,  1241,  -582,  -582,   422,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,    36,
     158,   422,  1186,   323,   422,  -582,  -582,   276,   278,  -582,
     748,   336,   338,   422,  -582,    36,   352,   280,   748,   270,
     273,   287,   193,   277,    41,   279,   299,   301,  -582,  -582,
      36,  1186,   286,   825,   322,  -582,   288,  -582,  -582,   283,
    -582,  -582,  -582,  -582,   283,   375,   330,  -582,  -582,  -582,
     422,  -582,   345,  -582,  1186,  -582,   285,  -582,  -582,   310,
     401,   376,  1331,  -582,  -582,   377,  1331,  1241,  -582,   331,
    -582,  -582,   415,  -582,  -582,   138,    72,  1331,  1331,   768,
     269,  -582,  -582,   290,  1186,   138,  -582,  -582,  -582,   330,
     330,  -582,  -582,  -582,   361,  -582,   283,   189,    14,   228,
    -582,    12,  -582,   423,  -582,  1186,   423,  -582,  -582,  -582,
     417,   138,  -582,  -582,  -582,   418,   282,  -119,   359,  -582,
     441,  1186,  1186,   -15,  1186,  1186,   999,   197,  1186,  1186,
    1186,  1186,  1186,   355,  1085,  1015,   379,   427,  1331,   825,
     423,   357,  -582,  -582,    90,  -582,   215,  -582,  -582,  1015,
     431,  -582,   423,    12,  -582,  1331,  1186,  -582,  -582,  1186,
    -582,  -582,  -582,  1186,   416,  -582,   302,   413,   303,   422,
    -582,  -582,  -582,   422,  1331,  -582,   435,  -582,  -582,   393,
    -582,  1186,   423,   457,   410,   394,   414,  -582,   487,  -582,
     215,  -582,  -582,   217,  -582,  -582,   748,  -582,   217,  -582,
     423,  -582,   330,  -582,   423,  1331,   330,   359,  -582,   748,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  1059,  1059,  -582,
    -582,   470,  1331,  1141,  -582,   471,  1331,  -582,  -582,   473,
    1250,  1261,  1305,  1331,  1331,  -582,  -582,  -582,   379,  1331,
     488,  -582,  -582,  -582,   476,  -582,  1186,    82,  -582,  -582,
     255,   492,  -582,   442,  -582,  -582,  -582,  -582,   217,  1331,
    1331,  1331,  1186,  -582,  -582,  -582,   138,   138,  -582,   748,
     423,    56,   330,   492,   426,  -582,   423,   442,  -582,    36,
    -582,  -582,  -582,   748,  -582,   330,   423,  -582,  -582,   492,
    -582,    37,  -582,  1186,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,   381,   432,  -582,  -582,  1322,   422,
     422,  -582,   362,  -582,    25,  -582,  -582,  -582,  -582,  -582,
    1070,  -582,  -582,  -582,   154,   496,  -582,   421,  -582,   458,
    -582,  -582,  1331,   484,   330,  -582,  -582,   484,   497,   330,
    -582,  -582,   369,   525,  -582,  -582,  -582,   283,   511,    27,
     382,  -582,  -582,   397,   374,  1331,  -582,   475,  -582,  -582,
    -582,  -582,  1186,   478,  -582,  -582,   515,   391,    31,  -582,
    -582,  -582,  1331,   465,   423,   330,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  1186,  1186,  -582,  -582,  -582,   217,  -582,
     217,  -582,  -582,  -582,  -582,    36,  -582,  -582,  -582,    53,
      37,  -582,     4,  -582,   330,   423,  1331,  1186,   541,  -582,
    -582,  -582,  -582,  1186,   542,  -582,  -582,  -582,  1186,  1186,
    1331,  1331,  -582,  -582,  -582,    36,   424,  -582,  1241,  -582,
    -582,   283,  -582,   272,  -582,  -582,  -582,   544,    93,    30,
    1331,  -582,   240,  1331,  -582,   480,  -582,  -582,   217,    36,
    -582,   748,   457,  1186,    93,  -582,  -582,   446,  -582,  -582,
     265,  -582,  -582,  -582,  -582,  -582,  -582,   748,  -582,  -582,
    -582,  -582,  -582,   217,   420,   492,  1331,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,   563,  -582,  -582,  -582,  -582
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       7,     2,     0,     0,     0,     0,    17,     0,   301,   418,
     350,   349,   418,   401,     0,   345,   339,   338,     0,   351,
       0,   283,   278,     0,     0,     0,     0,     0,     0,   393,
     276,   277,   299,   275,   302,     0,   280,   418,     0,     4,
     390,   392,   418,   418,   333,     5,   331,     6,   182,     1,
      16,     0,     0,     8,    23,   418,     0,     3,     0,     0,
     334,   336,     0,   400,   352,     0,     0,   418,     0,     0,
       0,     0,   313,     0,   297,   298,   418,   303,   302,     0,
     276,   275,     0,     0,     0,   282,   404,     0,     0,     0,
     408,   410,   412,   388,   389,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   293,   291,     0,   305,
       0,     0,     0,     0,     0,   279,     0,   344,     0,     0,
       0,   394,   402,   332,     0,   418,   418,     0,     0,     0,
       0,     0,    18,    38,   334,     0,   418,     0,     0,     0,
     366,   367,   368,   369,   370,   371,   361,   346,     0,   360,
     356,     0,     0,   398,     0,     0,     0,     0,   418,   377,
       0,     0,   300,   321,   399,   311,   312,   414,   286,   386,
       0,   314,   316,   318,   317,   320,   319,   405,     0,   409,
     411,   413,   285,   288,   287,   289,   290,     0,     0,   315,
       0,     0,   418,   418,   310,     0,   395,   396,   418,     0,
       0,     0,   391,     0,     0,   196,   193,   192,   194,   195,
     191,   183,     0,     0,     0,    72,    12,    71,    14,    11,
       9,    26,    25,    20,    19,   419,   418,    46,   418,    48,
      40,     0,    24,     0,   418,     0,    30,     0,   337,   343,
     353,   281,   347,     0,   418,     0,   257,   259,     0,   304,
     382,     0,   380,   375,   284,     0,   376,   373,   385,   295,
     387,   415,   322,   323,   324,   325,   418,   418,   326,   329,
     330,   294,   418,   292,   362,     0,     0,     0,   308,   417,
     309,     0,   354,     0,   335,   342,   340,   341,     0,   211,
     184,     0,   185,   188,     0,   418,   418,   230,   230,    22,
      36,    47,    33,    41,    42,    43,    44,    45,    49,     0,
     230,     0,     0,     0,     0,    28,   227,   230,   230,   230,
       0,     0,     0,   418,   230,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   229,   228,
       0,     0,     0,     0,     0,   230,     0,    39,    89,   418,
     230,    94,    95,   135,   418,     0,   418,   230,   101,    29,
       0,    35,   418,   357,     0,   260,   418,   263,   265,   418,
       0,     0,   378,   374,   372,   418,   384,     0,   327,     0,
     358,   328,     0,   363,   364,   407,     0,   307,   416,     0,
       0,   403,   187,     0,     0,   190,    13,    73,    15,   418,
     418,   418,   418,    50,   230,   230,   418,   418,   418,     0,
      61,   418,   230,   418,   230,     0,   418,   179,    85,    84,
       0,   119,   118,    87,   177,     0,     0,   418,   418,   197,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   418,     0,   132,     0,
     418,     0,   152,    92,   214,   151,     0,   145,   100,     0,
       0,   234,   418,   418,   348,   264,     0,   262,   266,     0,
     261,   268,   258,     0,     0,   381,     0,     0,     0,     0,
     306,   355,   397,     0,   189,    10,     0,    37,    34,     0,
     230,     0,   418,   418,     0,     0,     0,    74,     0,    81,
       0,    83,    68,   418,    67,    57,     0,    70,   418,    69,
     418,   241,   418,   240,   418,   137,   418,   418,    99,     0,
      86,   198,   201,   178,   199,   201,   181,   418,   418,   114,
     115,     0,   110,     0,   106,     0,   107,   112,   113,     0,
       0,     0,     0,   109,   111,   230,   139,   129,   418,   130,
       0,   131,   133,   122,     0,   226,     0,     0,   232,   150,
       0,   418,   231,   418,   256,   235,   146,   224,   418,   267,
     269,   383,     0,   296,   359,   365,   406,   186,    21,     0,
     418,   418,   418,   418,   418,    82,   418,   418,    55,     0,
      62,    65,   225,     0,   147,   418,   418,   149,   180,   418,
     200,     0,   212,     0,    97,   117,    98,    96,   104,   102,
      90,    91,   103,   105,     0,   418,   134,   121,     0,     0,
       0,   153,   154,   157,   155,   164,   166,   160,   201,   219,
       0,   221,   238,   216,     0,     0,   243,     0,   108,   418,
     271,    31,   379,   418,   418,    77,    76,   418,     0,   418,
      79,   418,     0,     0,   418,   242,   148,   418,     0,   418,
     202,   204,   205,   331,     0,   116,   140,     0,   142,   144,
     236,   143,     0,   418,   123,    93,   275,   275,     0,   163,
     233,   220,   218,   418,   418,   418,   245,   251,   250,   248,
     244,   246,   213,     0,     0,   270,   273,   418,   418,    75,
     418,    80,    78,    56,    58,     0,    66,   138,    88,   332,
       0,   206,     0,   141,   418,   418,   124,     0,   419,   125,
     167,   158,   156,     0,     0,   222,   239,   217,   418,   418,
     272,   274,    32,    51,    53,     0,     0,   203,     0,   208,
     209,   418,   237,     0,   127,   120,   128,   418,   418,   418,
     223,   215,   418,   253,   252,   418,   418,   418,   418,     0,
     207,     0,   418,     0,   418,   165,   169,     0,   168,   172,
       0,   171,   175,   159,   162,   161,   249,     0,   247,   254,
      52,    54,    59,   418,     0,   418,   126,   170,   176,   174,
     173,   255,   418,    63,   210,     0,    60,   418,   136,    64
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,   460,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,  -582,  -582,  -582,  -582,  -581,   126,  -102,  -283,  -582,
    -582,  -582,     8,  -582,  -582,  -582,  -582,  -582,   186,  -582,
    -582,    66,  -582,  -582,  -582,  -582,  -582,   136,   148,    61,
    -582,  -582,  -582,  -582,  -582,  -333,  -390,  -582,  -582,  -582,
     -79,  -582,  -582,  -582,   -23,  -582,  -582,  -582,  -157,  -139,
    -582,  -223,  -582,     2,  -582,  -582,  -582,  -582,  -582,  -582,
    -582,    94,  -493,  -582,  -582,   -98,  -582,  -582,  -273,  -582,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,     9,  -246,  -466,
    -582,  -380,  -582,  -582,  -582,  -582,  -353,  -582,  -554,  -582,
    -582,  -582,  -582,  -116,  -582,   124,   557,  -582,  -124,  -582,
    -195,  -582,  -582,    44,  -582,  -582,     7,   702,  -582,  -582,
    -582,  -582,   602,    19,   -16,  -582,  -208,    -2,     1,   605,
    -110,    -7,   306,  -582,  -582,  -582,  -582,   -22,   512,   275,
     372,    69,  -225,  -582,  -162,  -582,  -582,  -582,   389,  -582,
    -582,  -582,   485,  -582,   274,  -582,  -582,   -55,   -49,   527,
    -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,  -582,   451,
      -9,   -71
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     7,     6,    53,   297,   295,   296,    54,   132,
     298,   133,   223,   232,   233,   360,   697,   402,   401,   234,
     235,   236,   237,   315,   489,   756,   757,   651,   589,   735,
     792,   506,   653,   797,   654,   503,   508,   216,   396,   406,
     497,   498,   499,   347,   519,   348,   535,   349,   350,   539,
     531,   604,   420,   351,   673,   718,   745,   548,   447,   551,
     352,   596,   670,   353,   354,   453,   454,   557,   621,   622,
     623,   749,   773,   624,   625,   747,   748,   765,   768,   769,
     770,   626,   517,   627,   124,   211,   393,   394,   212,   213,
     428,   523,   600,   601,   660,   661,   738,   761,   288,   526,
     559,   560,   632,   685,   633,   724,   356,   357,   399,   561,
     628,   460,   714,   715,   683,   684,   512,   593,   635,   690,
     729,   728,   691,   752,   778,   563,   564,   246,   365,   366,
     367,   467,   470,   638,   639,   695,    29,    30,    70,   188,
     187,   377,    31,    32,    33,   190,   271,   662,    46,    34,
      35,   134,    36,    67,   362,    37,    38,    63,   115,   147,
     378,   148,   273,   383,   149,   157,   252,   158,   253,   474,
     374,   375,   159,   371,   257,   171,   108,    39,    40,    41,
     283,    42,   121,   109,   386,   110,   111,   112,   113,   278,
     513,   299
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      61,    45,    60,    64,   292,    48,    47,   137,   201,   135,
     390,   355,    81,   398,    57,   117,   493,   139,   363,   485,
     486,   457,    44,    44,   218,    68,   274,   583,    64,   647,
     529,    72,   602,   122,    61,    79,   123,     8,    76,   320,
      62,   129,    43,    43,    82,   658,    61,   417,    43,   659,
     169,   437,   400,   507,   255,   430,   502,   198,   150,   125,
     126,   199,   698,   516,   407,   511,   700,    61,   130,    65,
     530,   413,   415,   416,   325,    85,   327,   328,   423,    49,
     215,   274,   170,   392,    50,    51,    83,    84,    52,   164,
     165,   166,   301,   767,    43,   177,   320,   555,   558,   450,
    -418,   127,   128,   438,   456,   131,    66,   320,   178,   567,
     106,   462,   196,   107,   239,  -418,   217,   217,   487,   488,
     197,   179,   180,   181,   300,    69,   302,    64,   155,   221,
     222,   325,   594,   327,   328,   680,   597,   439,   619,   582,
      20,    20,   325,   114,   327,   328,    20,    20,    71,   258,
      83,    84,    83,    84,   289,   739,   767,   592,   491,   492,
     116,   595,   250,   740,   404,   405,   510,    88,   514,   476,
       1,     2,     3,     4,     8,    55,    10,    11,   119,    56,
      14,    15,   118,   279,   279,    73,    58,    27,    27,    61,
     620,   284,    20,    27,    27,   619,   200,   136,   119,    16,
     238,   494,   646,   120,   101,   102,   103,   104,   105,   106,
      58,   290,   107,    59,  -335,   656,   425,   217,   119,   217,
     242,   104,   105,   106,   107,   358,   107,   644,   645,   167,
     479,   795,   480,   649,   289,   368,   505,   163,    17,    27,
     203,   620,   215,   657,   580,   495,   219,    85,   220,   336,
     337,   224,   574,   686,   687,   688,   689,   380,   380,   629,
     240,   630,   631,   384,   699,   244,   245,   496,   243,   702,
     248,   289,    19,    20,   254,    21,    83,    84,    22,   289,
      84,   763,   385,    23,   537,   538,   397,   397,   434,   435,
     260,    85,   426,    10,    11,   427,   785,    14,    15,   614,
     261,   395,   275,   151,   152,   727,   285,   403,   151,   152,
     789,   790,    25,    26,   422,   274,    16,   286,   408,   287,
      27,   411,    73,   293,   707,   359,    48,   424,    44,   361,
     421,   726,   605,   605,   741,   379,   379,   364,   443,    88,
     455,   294,   370,   369,    44,   455,   251,   461,    83,    84,
     382,   762,   391,   150,   410,    17,   412,   468,   414,    44,
     471,   418,   742,   419,   198,   431,   258,   463,   703,   432,
     433,   706,   451,   436,   452,   440,   101,   102,   103,   104,
     105,   106,   445,    88,   107,   316,   666,   667,   668,    19,
     461,   461,   397,   397,   441,   449,   442,   455,   501,   504,
     458,   588,   509,   459,   466,   521,   591,   469,   102,   103,
     104,   105,   106,   472,   732,   107,   255,   473,    64,   524,
     101,   102,   103,   104,   105,   106,   478,   477,   107,   482,
     483,    85,   511,     8,    55,    10,    11,   552,    56,    14,
      15,   490,   518,   520,    59,   562,   338,   339,   522,   525,
     545,   590,   553,   556,   509,   550,   566,   572,    16,   204,
     578,   669,   573,   575,   599,   558,   641,   205,   206,   207,
     208,   209,   210,   780,   781,   140,   141,   142,   143,   144,
     145,   146,   114,   579,   562,    85,   576,   584,   784,   434,
     577,   664,   586,   585,   217,   607,   609,    17,   610,   217,
     616,   617,   634,   461,   496,   637,   672,   461,   524,   796,
     303,   304,   305,   306,   799,   307,   308,   693,   368,   368,
     678,   692,   694,    88,   643,   771,   502,   704,   701,    85,
     760,    19,    20,   705,    21,   712,   708,    22,   655,   552,
     710,   771,    23,   140,   141,   142,   143,   144,   145,   272,
     711,   721,   636,   717,   640,   713,   720,   723,    48,   217,
     101,   102,   103,   104,   105,   106,   744,   751,   107,    24,
     259,    25,    26,   461,   636,   501,    44,    88,   640,    27,
     794,    28,   759,   764,   777,   788,   461,   652,   798,   568,
     636,   225,   648,   500,   606,   565,   733,   554,   734,   722,
     664,   679,   663,   676,   677,   276,   674,   787,    44,   615,
     774,   598,   737,   755,   101,   102,   103,   104,   105,   106,
      44,    88,   107,   671,   587,   153,    79,    79,   776,    77,
     696,   650,    78,   429,   504,   461,   214,   464,   504,   381,
     461,   373,   397,   256,   280,   397,   202,   746,   455,   475,
      61,     0,   709,     0,     0,     0,   782,     0,   101,   102,
     103,   104,   105,   106,   719,     0,   107,     0,     0,     0,
     241,     0,     0,     0,   725,     0,   461,     0,     0,    48,
       0,   793,     0,     0,     0,     0,     0,     0,   397,   217,
       0,   217,     0,   309,   310,   311,   312,    44,   313,   314,
       0,     0,     0,   736,     0,   461,     0,     0,     0,     0,
       0,   663,   289,     0,     0,     0,     0,     0,     0,   754,
     754,     0,   791,     0,    44,     0,     0,    74,    75,    44,
      80,     0,   455,   758,     0,     0,     0,     0,   766,   772,
     772,    85,     0,   368,     0,     0,   779,   397,   397,   217,
      48,   775,     0,   562,    44,   772,     0,   783,     0,     8,
       9,    10,    11,    12,    13,    14,    15,   138,    44,     0,
       0,   154,   156,   160,   217,   161,   636,     0,    44,     0,
       0,     0,     0,   397,    16,     0,     0,    85,   397,     0,
     168,     0,     0,     0,     0,     0,     0,   172,   173,   174,
     175,   176,     0,   182,   183,   184,   185,   186,     0,     0,
     189,     0,   191,   192,   193,   194,   195,    85,     0,     0,
       0,     0,     0,    17,     8,    55,    10,    11,     0,    56,
      14,    15,     0,    88,     0,     0,     8,    55,    10,    11,
       0,    56,    14,    15,     0,     0,     0,     0,     0,    16,
       0,     0,     0,   247,     0,     0,    18,    19,    20,     0,
      21,    16,     0,    22,    85,     0,     0,     0,    23,     0,
     101,   102,   103,   104,   105,   106,     0,     0,   107,    88,
     281,     0,     0,     0,     0,     0,   282,     0,    17,     0,
       0,     0,     0,     0,     0,    24,     0,    25,    26,     0,
      17,     0,     0,     0,     0,    27,     0,    28,     0,    88,
       0,   446,     0,     0,     0,     0,   101,   102,   103,   104,
     105,   106,    19,    20,   107,    21,   155,    85,    22,     0,
       0,     0,   481,    23,    19,    20,     0,    21,    85,     0,
      22,     0,     0,     0,     0,    23,   101,   102,   103,   104,
     105,   106,     0,   372,   107,     0,    88,   376,   249,     0,
       0,     0,    25,    26,     0,     0,     0,     0,     0,     0,
      27,     0,    73,     0,    25,    26,     0,     0,   387,   388,
       0,     0,    27,   389,    73,     0,     0,     0,     0,     0,
       0,     0,     0,   101,   102,   103,   104,   105,   106,     0,
       0,   107,     0,     0,     0,   162,     0,     0,     0,     0,
       8,    55,    10,    11,   409,    56,    14,    15,     0,    88,
       0,   251,     0,     0,     0,     0,     8,    55,    10,    11,
      88,    56,    14,    15,     0,    16,     0,     0,   277,     0,
       0,     0,     0,   444,     0,   448,     0,     0,     0,     0,
       0,    16,     0,     0,     0,     0,   101,   102,   103,   104,
     105,   106,     0,     0,   107,     0,   465,   101,   102,   103,
     104,   105,   106,     0,    17,   107,     0,     0,    85,     0,
       0,     8,    55,    10,    11,   534,    56,    14,    15,     0,
      17,     0,     0,     0,     0,     0,   484,     0,     0,     0,
       0,   547,     0,     0,    85,     0,    16,     0,    19,    20,
     546,    21,     0,    85,    22,     0,     0,   515,     0,    23,
       0,     0,     0,     0,    19,    20,     0,    21,     0,     0,
      22,     0,     0,   527,   528,    23,   532,   533,   536,     0,
     540,   541,   542,   543,   544,    17,     0,   549,    25,    26,
     681,   448,     0,     0,     0,     0,    27,   603,    73,     0,
      85,   549,     0,     0,    25,    26,   608,     0,   569,     0,
      88,   570,    27,     0,    73,   571,     0,     0,     0,    19,
      20,     0,    21,     0,     0,    22,    85,     0,     0,     0,
      23,     0,     0,   581,     0,     0,    88,     8,    55,    10,
      11,     0,    56,    14,    15,  -419,     0,   101,   102,   103,
     104,   105,   106,     0,     0,   107,     0,     0,     0,    25,
      26,     0,    16,     0,     0,     0,     0,    27,     0,    73,
       0,     0,     0,   101,   102,   103,   104,   105,   106,     0,
       0,   107,   101,   102,   103,   104,   105,   106,     0,     0,
     107,     0,    88,     0,    85,     0,     0,    86,   618,     0,
       0,    17,     0,     0,     0,     0,     0,     0,     0,    85,
      87,     0,     0,     0,   642,   611,     0,     0,    88,     0,
      85,    89,     0,    90,    91,    92,   612,     0,     0,   101,
     102,   103,   104,   105,   106,    19,    20,   107,    21,     0,
       0,    22,     0,     0,     0,   665,    23,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,     0,     0,   107,    85,    86,     0,   162,     0,     0,
     613,     0,   682,     0,     0,    25,    26,     0,    87,     0,
       0,    85,     0,    27,     0,    73,    88,   675,     0,    89,
      85,    90,    91,    92,     0,     0,     0,     0,     0,     0,
       0,    88,   262,   263,   264,   265,   266,   267,   268,   269,
     270,     0,    88,     0,   716,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,     0,
       0,   107,     0,     0,     0,   730,   731,     0,   101,   102,
     103,   104,   105,   106,     0,     0,   107,     0,     0,   101,
     102,   103,   104,   105,   106,     0,    88,   107,     0,   743,
       0,     0,     0,     0,     0,   750,     0,     0,     0,     0,
     753,   753,     0,    88,     0,     0,     0,     0,     0,     0,
       0,     0,    88,     0,     0,     0,     0,     0,     0,   316,
     317,   318,   319,   101,   102,   103,   104,   105,   106,   320,
       0,   107,     0,     0,     0,   786,     0,     0,     0,     0,
     101,   102,   103,   104,   105,   106,     0,     0,   107,   101,
     102,   103,   104,   105,   106,     0,     0,   107,     0,     0,
     321,   322,   323,   324,   325,   326,   327,   328,   329,   330,
     331,   332,   333,   334,   335,   336,   337,     0,     0,     0,
     338,   339,     0,   340,   341,   342,   343,   344,     0,     0,
       0,     0,     0,     0,     0,   345,     0,     0,   346,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   -27,   -27,
     -27,   -27,   226,   -27,   -27,   227,   228,   229,   291,     0,
     230,   231
};

static const yytype_int16 yycheck[] =
{
       9,     3,     9,    12,   212,     4,     4,    62,   118,    58,
     283,   234,    28,   296,     7,    37,   406,    66,   243,   399,
     400,   354,     3,     4,   126,    18,   188,   493,    37,   583,
      45,    24,   525,    42,    43,    28,    43,    11,    12,    14,
     159,     3,    12,    12,    96,   599,    55,   320,    12,    12,
     115,    10,   298,    41,    39,   328,    42,    12,    67,    27,
      28,    16,   643,   416,   310,     9,   647,    76,    30,   159,
      85,   317,   318,   319,    49,    19,    51,    52,   324,     0,
      25,   243,   147,   291,    25,    26,   138,   139,    29,    82,
      83,    84,    37,    63,    12,    90,    14,   450,     8,   345,
      10,    27,    28,    62,   350,    67,   159,    14,   103,   462,
     153,   357,   151,   156,   136,    25,   125,   126,   401,   402,
     159,   116,   117,   118,   226,   159,   228,   136,   113,    81,
      82,    49,   512,    51,    52,   628,   516,    96,   113,   492,
     110,   110,    49,   163,    51,    52,   110,   110,   159,   158,
     138,   139,   138,   139,   203,   151,    63,   510,   404,   405,
     161,   514,   155,   159,     6,     7,   412,   111,   414,   377,
      21,    22,    23,    24,    11,    12,    13,    14,   158,    16,
      17,    18,   159,   192,   193,   159,   159,   157,   157,   198,
     159,   198,   110,   157,   157,   113,   151,    12,   158,    36,
     160,    12,   582,   161,   148,   149,   150,   151,   152,   153,
     159,   204,   156,   162,   161,   595,   326,   226,   158,   228,
     160,   151,   152,   153,   156,   234,   156,   580,   581,   118,
     158,   785,   160,   586,   283,   244,     8,   160,    75,   157,
     159,   159,    25,   596,   490,    56,    25,    19,    25,    60,
      61,    25,   477,    99,   100,   101,   102,   266,   267,     4,
     160,     6,     7,   272,   644,   102,   103,    78,    96,   649,
      12,   320,   109,   110,     3,   112,   138,   139,   115,   328,
     139,     9,   275,   120,    87,    88,   295,   296,    95,    96,
     115,    19,    12,    13,    14,    15,   762,    17,    18,   545,
     118,   294,   159,    93,    94,   685,   160,   309,    93,    94,
      45,    46,   149,   150,   323,   477,    36,   160,   311,   160,
     157,   314,   159,   140,   657,    25,   325,   325,   309,    25,
     323,   684,   527,   528,   714,   266,   267,    97,   340,   111,
     349,   140,   162,   102,   325,   354,   113,   356,   138,   139,
     159,   741,   160,   362,    31,    75,    80,   366,    80,   340,
     369,    25,   715,    25,    12,    95,   375,   360,   651,    96,
      83,   654,    84,    96,    91,    96,   148,   149,   150,   151,
     152,   153,    96,   111,   156,     4,     5,     6,     7,   109,
     399,   400,   401,   402,    95,    73,    95,   406,   407,   408,
      25,   503,   411,    73,   119,   427,   508,    97,   149,   150,
     151,   152,   153,    12,   697,   156,    39,    41,   427,   428,
     148,   149,   150,   151,   152,   153,    11,    96,   156,   160,
     140,    19,     9,    11,    12,    13,    14,   446,    16,    17,
      18,    80,    25,    25,   162,   454,    65,    66,    89,     8,
      95,   506,    25,    96,   463,    76,    25,    41,    36,   140,
      25,    80,   160,   160,   519,     8,   568,   148,   149,   150,
     151,   152,   153,   756,   757,   130,   131,   132,   133,   134,
     135,   136,   163,    90,   493,    19,   479,    77,   761,    95,
     483,   601,     5,    79,   503,    25,    25,    75,    25,   508,
      12,    25,    10,   512,    78,    63,    74,   516,   517,   792,
      31,    32,    33,    34,   797,    36,    37,    96,   527,   528,
     158,    25,    64,   111,   579,   748,    42,   158,    31,    19,
     738,   109,   110,     8,   112,   161,    25,   115,   593,   548,
     158,   764,   120,   130,   131,   132,   133,   134,   135,   136,
     153,   160,   561,    75,   563,    80,    41,    92,   557,   568,
     148,   149,   150,   151,   152,   153,    25,    25,   156,   147,
     158,   149,   150,   582,   583,   584,   557,   111,   587,   157,
     160,   159,   158,    39,   104,   139,   595,   589,    25,   463,
     599,   131,   584,   407,   528,   459,   698,   449,   700,   678,
     710,   624,   601,   619,   620,   139,   615,   764,   589,   548,
     749,   517,   710,   729,   148,   149,   150,   151,   152,   153,
     601,   111,   156,   614,   500,    68,   619,   620,   752,    27,
     639,   587,    27,   327,   643,   644,   124,   362,   647,   267,
     649,   252,   651,   158,   193,   654,   119,   718,   657,   375,
     659,    -1,   659,    -1,    -1,    -1,   758,    -1,   148,   149,
     150,   151,   152,   153,   673,    -1,   156,    -1,    -1,    -1,
     160,    -1,    -1,    -1,   683,    -1,   685,    -1,    -1,   678,
      -1,   783,    -1,    -1,    -1,    -1,    -1,    -1,   697,   698,
      -1,   700,    -1,    31,    32,    33,    34,   678,    36,    37,
      -1,    -1,    -1,   705,    -1,   714,    -1,    -1,    -1,    -1,
      -1,   710,   761,    -1,    -1,    -1,    -1,    -1,    -1,   728,
     729,    -1,   777,    -1,   705,    -1,    -1,    25,    26,   710,
      28,    -1,   741,   735,    -1,    -1,    -1,    -1,   747,   748,
     749,    19,    -1,   752,    -1,    -1,   755,   756,   757,   758,
     749,   749,    -1,   762,   735,   764,    -1,   759,    -1,    11,
      12,    13,    14,    15,    16,    17,    18,    65,   749,    -1,
      -1,    69,    70,    71,   783,    73,   785,    -1,   759,    -1,
      -1,    -1,    -1,   792,    36,    -1,    -1,    19,   797,    -1,
      88,    -1,    -1,    -1,    -1,    -1,    -1,    95,    96,    97,
      98,    99,    -1,   101,   102,   103,   104,   105,    -1,    -1,
     108,    -1,   110,   111,   112,   113,   114,    19,    -1,    -1,
      -1,    -1,    -1,    75,    11,    12,    13,    14,    -1,    16,
      17,    18,    -1,   111,    -1,    -1,    11,    12,    13,    14,
      -1,    16,    17,    18,    -1,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    -1,   151,    -1,    -1,   108,   109,   110,    -1,
     112,    36,    -1,   115,    19,    -1,    -1,    -1,   120,    -1,
     148,   149,   150,   151,   152,   153,    -1,    -1,   156,   111,
     158,    -1,    -1,    -1,    -1,    -1,   164,    -1,    75,    -1,
      -1,    -1,    -1,    -1,    -1,   147,    -1,   149,   150,    -1,
      75,    -1,    -1,    -1,    -1,   157,    -1,   159,    -1,   111,
      -1,    86,    -1,    -1,    -1,    -1,   148,   149,   150,   151,
     152,   153,   109,   110,   156,   112,   113,    19,   115,    -1,
      -1,    -1,   164,   120,   109,   110,    -1,   112,    19,    -1,
     115,    -1,    -1,    -1,    -1,   120,   148,   149,   150,   151,
     152,   153,    -1,   251,   156,    -1,   111,   255,   160,    -1,
      -1,    -1,   149,   150,    -1,    -1,    -1,    -1,    -1,    -1,
     157,    -1,   159,    -1,   149,   150,    -1,    -1,   276,   277,
      -1,    -1,   157,   281,   159,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   148,   149,   150,   151,   152,   153,    -1,
      -1,   156,    -1,    -1,    -1,   160,    -1,    -1,    -1,    -1,
      11,    12,    13,    14,   312,    16,    17,    18,    -1,   111,
      -1,   113,    -1,    -1,    -1,    -1,    11,    12,    13,    14,
     111,    16,    17,    18,    -1,    36,    -1,    -1,   119,    -1,
      -1,    -1,    -1,   341,    -1,   343,    -1,    -1,    -1,    -1,
      -1,    36,    -1,    -1,    -1,    -1,   148,   149,   150,   151,
     152,   153,    -1,    -1,   156,    -1,   364,   148,   149,   150,
     151,   152,   153,    -1,    75,   156,    -1,    -1,    19,    -1,
      -1,    11,    12,    13,    14,    86,    16,    17,    18,    -1,
      75,    -1,    -1,    -1,    -1,    -1,   394,    -1,    -1,    -1,
      -1,    86,    -1,    -1,    19,    -1,    36,    -1,   109,   110,
      25,   112,    -1,    19,   115,    -1,    -1,   415,    -1,   120,
      -1,    -1,    -1,    -1,   109,   110,    -1,   112,    -1,    -1,
     115,    -1,    -1,   431,   432,   120,   434,   435,   436,    -1,
     438,   439,   440,   441,   442,    75,    -1,   445,   149,   150,
      80,   449,    -1,    -1,    -1,    -1,   157,    98,   159,    -1,
      19,   459,    -1,    -1,   149,   150,    25,    -1,   466,    -1,
     111,   469,   157,    -1,   159,   473,    -1,    -1,    -1,   109,
     110,    -1,   112,    -1,    -1,   115,    19,    -1,    -1,    -1,
     120,    -1,    -1,   491,    -1,    -1,   111,    11,    12,    13,
      14,    -1,    16,    17,    18,   111,    -1,   148,   149,   150,
     151,   152,   153,    -1,    -1,   156,    -1,    -1,    -1,   149,
     150,    -1,    36,    -1,    -1,    -1,    -1,   157,    -1,   159,
      -1,    -1,    -1,   148,   149,   150,   151,   152,   153,    -1,
      -1,   156,   148,   149,   150,   151,   152,   153,    -1,    -1,
     156,    -1,   111,    -1,    19,    -1,    -1,    90,   556,    -1,
      -1,    75,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
     103,    -1,    -1,    -1,   572,    25,    -1,    -1,   111,    -1,
      19,   114,    -1,   116,   117,   118,    25,    -1,    -1,   148,
     149,   150,   151,   152,   153,   109,   110,   156,   112,    -1,
      -1,   115,    -1,    -1,    -1,   603,   120,   140,   141,   142,
     143,   144,   145,   146,   147,   148,   149,   150,   151,   152,
     153,    -1,    -1,   156,    19,    90,    -1,   160,    -1,    -1,
      25,    -1,   630,    -1,    -1,   149,   150,    -1,   103,    -1,
      -1,    19,    -1,   157,    -1,   159,   111,    25,    -1,   114,
      19,   116,   117,   118,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,   121,   122,   123,   124,   125,   126,   127,   128,
     129,    -1,   111,    -1,   672,   140,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,   152,   153,    -1,
      -1,   156,    -1,    -1,    -1,   693,   694,    -1,   148,   149,
     150,   151,   152,   153,    -1,    -1,   156,    -1,    -1,   148,
     149,   150,   151,   152,   153,    -1,   111,   156,    -1,   717,
      -1,    -1,    -1,    -1,    -1,   723,    -1,    -1,    -1,    -1,
     728,   729,    -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,     4,
       5,     6,     7,   148,   149,   150,   151,   152,   153,    14,
      -1,   156,    -1,    -1,    -1,   763,    -1,    -1,    -1,    -1,
     148,   149,   150,   151,   152,   153,    -1,    -1,   156,   148,
     149,   150,   151,   152,   153,    -1,    -1,   156,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    -1,    -1,    -1,
      65,    66,    -1,    68,    69,    70,    71,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    80,    -1,    -1,    83,   121,
     122,   123,   124,   125,   126,   127,   128,   129,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,   140,    -1,
      43,    44
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    21,    22,    23,    24,   166,   168,   167,    11,    12,
      13,    14,    15,    16,    17,    18,    36,    75,   108,   109,
     110,   112,   115,   120,   147,   149,   150,   157,   159,   301,
     302,   307,   308,   309,   314,   315,   317,   320,   321,   342,
     343,   344,   346,    12,   308,   312,   313,   248,   313,     0,
      25,    26,    29,   169,   173,    12,    16,   301,   159,   162,
     316,   355,   159,   322,   355,   159,   159,   318,   301,   159,
     303,   159,   301,   159,   302,   302,    12,   307,   314,   301,
     302,   309,    96,   138,   139,    19,    90,   103,   111,   114,
     116,   117,   118,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,   156,   341,   348,
     350,   351,   352,   353,   163,   323,   161,   322,   159,   158,
     161,   347,   355,   316,   249,    27,    28,    27,    28,     3,
      30,    67,   174,   176,   316,   343,    12,   342,   302,   343,
     130,   131,   132,   133,   134,   135,   136,   324,   326,   329,
     355,    93,    94,   291,   302,   113,   302,   330,   332,   337,
     302,   302,   160,   160,   301,   301,   301,   118,   302,   115,
     147,   340,   302,   302,   302,   302,   302,    90,   103,   116,
     117,   118,   302,   302,   302,   302,   302,   305,   304,   302,
     310,   302,   302,   302,   302,   302,   151,   159,    12,    16,
     151,   315,   344,   159,   140,   148,   149,   150,   151,   152,
     153,   250,   253,   254,   323,    25,   202,   355,   202,    25,
      25,    81,    82,   177,    25,   177,    35,    38,    39,    40,
      43,    44,   178,   179,   184,   185,   186,   187,   160,   322,
     160,   160,   160,    96,   102,   103,   292,   302,    12,   160,
     301,   113,   331,   333,     3,    39,   337,   339,   355,   158,
     115,   118,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   311,   136,   327,   329,   159,   139,   119,   354,   355,
     354,   158,   164,   345,   316,   160,   160,   160,   263,   343,
     301,   140,   311,   140,   140,   171,   172,   170,   175,   356,
     202,    37,   202,    31,    32,    33,    34,    36,    37,    31,
      32,    33,    34,    36,    37,   188,     4,     5,     6,     7,
      14,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    65,    66,
      68,    69,    70,    71,    72,    80,    83,   208,   210,   212,
     213,   218,   225,   228,   229,   246,   271,   272,   355,    25,
     180,    25,   319,   327,    97,   293,   294,   295,   355,   102,
     162,   338,   302,   333,   335,   336,   302,   306,   325,   326,
     355,   325,   159,   328,   355,   301,   349,   302,   302,   302,
     263,   160,   311,   251,   252,   301,   203,   355,   203,   273,
     273,   183,   182,   312,     6,     7,   204,   273,   301,   302,
      31,   301,    80,   273,    80,   273,   273,   263,    25,    25,
     217,   301,   355,   273,   248,   315,    12,    15,   255,   317,
     263,    95,    96,    83,    95,    96,    96,    10,    62,    96,
      96,    95,    95,   312,   302,    96,    86,   223,   302,    73,
     273,    84,    91,   230,   231,   355,   273,   230,    25,    73,
     276,   355,   273,   301,   324,   302,   119,   296,   355,    97,
     297,   355,    12,    41,   334,   339,   311,    96,    11,   158,
     160,   164,   160,   140,   302,   276,   276,   203,   203,   189,
      80,   273,   273,   231,    12,    56,    78,   205,   206,   207,
     213,   355,    42,   200,   355,     8,   196,    41,   201,   355,
     273,     9,   281,   355,   273,   302,   281,   247,    25,   209,
      25,   322,    89,   256,   355,     8,   264,   302,   302,    45,
      85,   215,   302,   302,    86,   211,   302,    87,    88,   214,
     302,   302,   302,   302,   302,    95,    25,    86,   222,   302,
      76,   224,   355,    25,   223,   281,    96,   232,     8,   265,
     266,   274,   355,   290,   291,   222,    25,   281,   201,   302,
     302,   302,    41,   160,   327,   160,   301,   301,    25,    90,
     273,   302,   281,   274,    77,    79,     5,   290,   202,   193,
     342,   202,   281,   282,   276,   281,   226,   276,   256,   342,
     257,   258,   257,    98,   216,   295,   216,    25,    25,    25,
      25,    25,    25,    25,   273,   224,    12,    25,   302,   113,
     159,   233,   234,   235,   238,   239,   246,   248,   275,     4,
       6,     7,   267,   269,    10,   283,   355,    63,   298,   299,
     355,   202,   302,   342,   281,   281,   276,   283,   207,   281,
     298,   192,   312,   197,   199,   342,   276,   281,   283,    12,
     259,   260,   312,   313,   315,   302,     5,     6,     7,    80,
     227,   272,    74,   219,   355,    25,   309,   309,   158,   239,
     257,    80,   302,   279,   280,   268,    99,   100,   101,   102,
     284,   287,    25,    96,    64,   300,   355,   181,   200,   276,
     200,    31,   276,   203,   158,     8,   203,   230,    25,   316,
     158,   153,   161,    80,   277,   278,   302,    75,   220,   355,
      41,   160,   235,    92,   270,   355,   281,   276,   286,   285,
     302,   302,   203,   202,   202,   194,   312,   260,   261,   151,
     159,   276,   281,   302,    25,   221,   356,   240,   241,   236,
     302,    25,   288,   302,   355,   288,   190,   191,   312,   158,
     311,   262,   231,     9,    39,   242,   355,    63,   243,   244,
     245,   246,   355,   237,   244,   248,   293,   104,   289,   355,
     203,   203,   202,   312,   263,   274,   302,   243,   139,    45,
      46,   342,   195,   202,   160,   283,   203,   198,    25,   203
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint16 yyr1[] =
{
       0,   165,   167,   166,   166,   166,   166,   168,   166,   170,
     169,   169,   171,   169,   172,   169,   169,   173,   169,   174,
     175,   174,   174,   176,   174,   177,   177,   179,   178,   178,
     180,   181,   178,   182,   178,   178,   183,   178,   184,   178,
     185,   185,   185,   185,   185,   185,   186,   186,   187,   187,
     189,   190,   188,   191,   188,   192,   188,   193,   194,   195,
     188,   196,   197,   198,   188,   199,   188,   200,   200,   201,
     201,   202,   202,   203,   204,   204,   204,   204,   205,   205,
     206,   206,   207,   207,   208,   208,   208,   209,   208,   208,
     208,   208,   208,   208,   208,   208,   208,   208,   208,   208,
     208,   208,   210,   210,   210,   210,   211,   211,   212,   213,
     213,   213,   214,   214,   215,   215,   216,   216,   217,   217,
     218,   218,   218,   219,   219,   220,   220,   221,   221,   222,
     222,   223,   223,   224,   224,   225,   225,   226,   225,   225,
     227,   227,   227,   227,   227,   228,   228,   229,   229,   229,
     230,   231,   232,   231,   233,   233,   234,   234,   236,   235,
     235,   237,   237,   238,   238,   239,   239,   241,   240,   242,
     242,   243,   243,   244,   244,   245,   245,   246,   246,   247,
     246,   246,   249,   248,   250,   251,   250,   250,   252,   250,
     250,   253,   254,   254,   254,   254,   254,   255,   255,   256,
     256,   258,   257,   259,   259,   260,   261,   260,   260,   262,
     260,   263,   264,   265,   266,   265,   268,   267,   267,   269,
     269,   269,   270,   270,   271,   271,   271,   272,   272,   272,
     273,   274,   275,   274,   276,   276,   278,   277,   280,   279,
     281,   282,   281,   283,   283,   284,   285,   284,   286,   284,
     287,   287,   288,   288,   289,   289,   290,   291,   291,   292,
     292,   292,   293,   294,   294,   295,   296,   296,   297,   297,
     298,   299,   299,   300,   300,   301,   301,   302,   302,   302,
     302,   302,   302,   303,   302,   302,   302,   302,   302,   302,
     302,   304,   302,   305,   302,   306,   302,   302,   302,   302,
     307,   307,   307,   308,   308,   310,   309,   309,   309,   309,
     309,   309,   309,   309,   309,   309,   309,   309,   309,   309,
     309,   309,   311,   311,   311,   311,   311,   311,   311,   311,
     311,   312,   313,   313,   314,   315,   316,   316,   317,   317,
     317,   317,   317,   317,   317,   318,   317,   319,   317,   320,
     320,   321,   322,   322,   323,   323,   324,   324,   325,   325,
     326,   326,   327,   327,   328,   328,   329,   329,   329,   329,
     329,   329,   330,   330,   331,   331,   332,   332,   334,   333,
     336,   335,   338,   337,   339,   339,   340,   340,   341,   341,
     342,   343,   343,   344,   344,   344,   345,   344,   344,   344,
     346,   346,   347,   347,   348,   348,   349,   349,   350,   350,
     351,   351,   352,   352,   353,   353,   354,   354,   355,   356
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     3,     2,     2,     2,     0,     2,     0,
       6,     3,     0,     5,     0,     5,     1,     0,     2,     2,
       0,     6,     3,     0,     2,     1,     1,     0,     2,     2,
       0,     0,     7,     0,     4,     2,     0,     4,     0,     2,
       1,     2,     2,     2,     2,     2,     1,     2,     1,     2,
       0,     0,     9,     0,     9,     0,     6,     0,     0,     0,
      11,     0,     0,     0,    12,     0,     6,     1,     1,     1,
       1,     1,     1,     1,     2,     5,     4,     4,     4,     3,
       4,     1,     2,     1,     2,     2,     3,     0,     6,     1,
       4,     4,     2,     5,     1,     1,     4,     4,     4,     3,
       2,     1,     4,     4,     4,     4,     1,     1,     4,     3,
       3,     3,     1,     1,     1,     1,     2,     1,     1,     1,
       7,     4,     3,     1,     2,     1,     4,     1,     1,     1,
       1,     2,     1,     1,     2,     1,    11,     0,     6,     3,
       1,     2,     1,     1,     1,     2,     3,     4,     5,     4,
       2,     1,     0,     3,     1,     1,     3,     1,     0,     5,
       1,     1,     1,     2,     1,     5,     1,     0,     2,     1,
       2,     1,     1,     2,     2,     1,     2,     2,     3,     0,
       4,     3,     0,     3,     2,     0,     5,     3,     0,     4,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     0,     2,     3,     1,     1,     0,     4,     3,     0,
       6,     1,     2,     3,     0,     5,     0,     3,     2,     1,
       2,     1,     1,     2,     3,     4,     3,     1,     1,     1,
       0,     1,     0,     3,     1,     2,     0,     2,     0,     2,
       1,     0,     3,     1,     2,     1,     0,     4,     0,     4,
       1,     1,     1,     1,     1,     2,     1,     2,     4,     1,
       2,     3,     2,     1,     2,     1,     1,     2,     1,     2,
       2,     1,     3,     1,     2,     1,     1,     1,     1,     2,
       1,     4,     2,     0,     4,     3,     3,     3,     3,     3,
       3,     0,     4,     0,     4,     0,     7,     2,     2,     1,
       3,     1,     1,     2,     4,     0,     6,     5,     4,     4,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     1,     1,     1,     1,     1,     2,     2,     1,
       1,     1,     2,     1,     2,     2,     1,     3,     1,     1,
       4,     4,     4,     4,     2,     0,     3,     0,     6,     1,
       1,     1,     1,     3,     3,     5,     1,     3,     1,     3,
       1,     1,     1,     2,     1,     3,     1,     1,     1,     1,
       1,     1,     3,     2,     2,     1,     2,     1,     0,     5,
       0,     2,     0,     5,     2,     1,     1,     2,     1,     1,
       1,     3,     1,     1,     2,     3,     0,     6,     3,     3,
       2,     1,     1,     4,     1,     2,     3,     1,     1,     2,
       1,     2,     1,     2,     2,     3,     2,     1,     0,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



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

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
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
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
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


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
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
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
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
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
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

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- Do a reduction.  |
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
        case 2:
#line 290 "csqpy.y" /* yacc.c:1646  */
    { pstate->runneroptions|=RT_CANRUNSLEEP; }
#line 2331 "csqpy.c" /* yacc.c:1646  */
    break;

  case 3:
#line 290 "csqpy.y" /* yacc.c:1646  */
    {
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
#line 2348 "csqpy.c" /* yacc.c:1646  */
    break;

  case 4:
#line 302 "csqpy.y" /* yacc.c:1646  */
    {
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
#line 2365 "csqpy.c" /* yacc.c:1646  */
    break;

  case 5:
#line 314 "csqpy.y" /* yacc.c:1646  */
    {
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
#line 2382 "csqpy.c" /* yacc.c:1646  */
    break;

  case 6:
#line 326 "csqpy.y" /* yacc.c:1646  */
    {
/*
** reposition lex pointer to first unparsed char
*/
			    if (yychar==0)
				pstate->ibufp--;
			    else if (yychar>0)
				pstate->ibufp=pstate->tokstart;
			    status=rxx_execute(NULL, NULL);
			    FAILCHECK
			    YYACCEPT;
			}
#line 2399 "csqpy.c" /* yacc.c:1646  */
    break;

  case 7:
#line 338 "csqpy.y" /* yacc.c:1646  */
    {
		pstate->runneroptions|=(RT_CANRUNSLEEP|RT_CANSYSTEM);
	  }
#line 2407 "csqpy.c" /* yacc.c:1646  */
    break;

  case 9:
#line 347 "csqpy.y" /* yacc.c:1646  */
    {
			    if (!pstate->control.count)
				FAILSYN;
			    switch (((controlstack_t *)
					fgw_stackpeek(&pstate->control))->state)
			    {
			      case S_COMPOUND:
			      case S_DISCOMP:
				break;
			      default:
				FAILSYN;
			    }
			}
#line 2425 "csqpy.c" /* yacc.c:1646  */
    break;

  case 10:
#line 359 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if ((c=((controlstack_t *)
				fgw_stackpeek(&pstate->control)))->state)
			    {
				pstate->ssp->sqlend=c->text_end;
				endstmt_sql();
				if (rsx_sqlrun(TOKPTR(c->text_start)))
				    FAILCHECK
			    }
			    rsx_endctrl();
			}
#line 2443 "csqpy.c" /* yacc.c:1646  */
    break;

  case 11:
#line 372 "csqpy.y" /* yacc.c:1646  */
    {
			    if (!pstate->control.count)
				FAILSYN;
			    switch (((controlstack_t *)
					fgw_stackpeek(&pstate->control))->state)
			    {
			      case S_DISIMM:
			      case S_IMMEDIATE:
				break;
			      default:
				FAILSYN;
			    }
			    rsx_endctrl();
			}
#line 2462 "csqpy.c" /* yacc.c:1646  */
    break;

  case 12:
#line 386 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_IMMEDIATE;
				    break;
				  case S_IMMEDIATE:
				  case S_DISIMM:
				  case S_COMPOUND:
				  case S_DISCOMP:
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISIMM;
				}
			    else
				c.state=S_IMMEDIATE;
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
#line 2493 "csqpy.c" /* yacc.c:1646  */
    break;

  case 14:
#line 412 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_COMPOUND;
				    break;
				  case S_IMMEDIATE:
				  case S_DISIMM:
				  case S_COMPOUND:
				  case S_DISCOMP:
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISCOMP;
				}
			    else
				c.state=S_COMPOUND;
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
#line 2524 "csqpy.c" /* yacc.c:1646  */
    break;

  case 17:
#line 439 "csqpy.y" /* yacc.c:1646  */
    {
	    int s;
	    instruction_t *i;
	    controlstack_t *c;

	    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
	    s=pstate->control.count? c->state: S_ACTIVE;
	    switch (s)
	    {
	      case S_IMMEDIATE:
		MARK_STMT
		endstmt_sql();
		if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
		    FAIL(RC_NOMEM)
		i=rxx_newinstr(pstate);
		FAILCHECK
		i->func=rsx_curcon;
		if (rsx_sqlrun(STMTPTR(pstate->ssp)))
		    FAILCHECK
		YYACCEPT;
	      case S_COMPOUND:
		MARK_STMT

		/* accrue beginning and end as we go along, and leave the statement unmarked */
		if (!c->text_start)
		    c->text_start=pstate->ssp->sqlstart;
		c->text_end=pstate->ssp->sqlend;
		pstate->ssp->sqlend=0;
		YYACCEPT;
	      case S_DISIMM:
	      case S_DISCOMP:
		NEXT_STMT
		break;
	    }
	  }
#line 2564 "csqpy.c" /* yacc.c:1646  */
    break;

  case 19:
#line 475 "csqpy.y" /* yacc.c:1646  */
    {
			    if (pstate->flags & PF_QUIT)
				rsx_quit();
			    else
				FAILSYN;
			}
#line 2575 "csqpy.c" /* yacc.c:1646  */
    break;

  case 20:
#line 481 "csqpy.y" /* yacc.c:1646  */
    {
			    if (!pstate->control.count)
				FAILSYN;
			    if (((controlstack_t *)
				fgw_stackpeek(&pstate->control))->stmt_type!=
				(yyvsp[0].stack).type)
				FAILSYN;
			}
#line 2588 "csqpy.c" /* yacc.c:1646  */
    break;

  case 21:
#line 488 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if ((c=((controlstack_t *)
				fgw_stackpeek(&pstate->control)))->state)
			    {
				if (rsx_sqlrun(TOKPTR(c->text_start)))
				    FAILCHECK
			    }
			    rsx_endctrl();
			}
#line 2604 "csqpy.c" /* yacc.c:1646  */
    break;

  case 22:
#line 499 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    c.stmt_type=(yyvsp[-1].stack).type;
			    if (pstate->control.count)
				switch (((controlstack_t *)
				    fgw_stackpeek(&pstate->control))->state)
				{
				  case S_ACTIVE:
				    c.state=S_INPROC;
				    break;
				  case S_INPROC:
				  case S_DISPROC:
				    FAILSYN;
				    break;
				  default:
				    c.state=S_DISPROC;
				}
			    else
				c.state=S_INPROC;
			    c.text_start=(yyvsp[-2].tstart);
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
#line 2633 "csqpy.c" /* yacc.c:1646  */
    break;

  case 23:
#line 523 "csqpy.y" /* yacc.c:1646  */
    {
	    int s;

	    s=pstate->control.count? ((controlstack_t *)
		fgw_stackpeek(&pstate->control))->state: S_ACTIVE;
	    switch (s)
	    {
	      case S_INPROC:
	      case S_DISPROC:
		NEXT_STMT
	    }
	  }
#line 2650 "csqpy.c" /* yacc.c:1646  */
    break;

  case 25:
#line 536 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_FUNCTION; }
#line 2656 "csqpy.c" /* yacc.c:1646  */
    break;

  case 26:
#line 537 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_PROCEDURE; }
#line 2662 "csqpy.c" /* yacc.c:1646  */
    break;

  case 27:
#line 542 "csqpy.y" /* yacc.c:1646  */
    {
/*
** if we don't have to do anything, we don't parse, just accept
*/
	    if (pstate->control.count &&
		((controlstack_t *) fgw_stackpeek(&pstate->control))->state<
							S_ACTIVE)
	    {
		controlstack_t c;

		memset(&c, 0, sizeof(c));
		switch (yychar)
		{
		  case IF:
		    c.stmt_type=T_IF;
		    break;
		  case WHILE:
		    c.stmt_type=T_WHILE;
		    break;
		  case FOR:
		    c.stmt_type=T_FOR;
		    break;
		  case FOREACH:
		    c.stmt_type=T_FOREACH;
		    break;
		  case CLONE:
		    c.stmt_type=T_CLONE;
		    break;
		  case WAIT:
		    c.stmt_type=T_WAIT;
		    break;
		}
		c.state=S_DISABLED;
		fgw_stackpush(&pstate->control, (char *) &c);
		NEXT_STMT
	    }
	  }
#line 2704 "csqpy.c" /* yacc.c:1646  */
    break;

  case 29:
#line 579 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if ((yyvsp[-1].stack).type!=DONE && (yyvsp[-1].stack).type!=c->stmt_type)
				FAILSYN
			    switch(c->stmt_type)
			    {
			      case T_FOR:
				rsx_endfor(c);
				FAILCHECK
				break;
			      case T_FOREACH:
				rsx_endforeach(c);
				FAILCHECK
				break;
			      case T_WHILE:
				rsx_endwhile(c);
				break;
			      case T_CLONE:
				if (rsx_endclone(c))
				    FAILCHECK
				break;
			      case T_WAIT:
				rsx_endwait(c);
				FAILCHECK
				break;
			      default:
				FAILSYN;
			    }
			}
#line 2742 "csqpy.c" /* yacc.c:1646  */
    break;

  case 30:
#line 612 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			}
#line 2756 "csqpy.c" /* yacc.c:1646  */
    break;

  case 31:
#line 620 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_elif();
                            FAILCHECK;
			}
#line 2765 "csqpy.c" /* yacc.c:1646  */
    break;

  case 33:
#line 624 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_else(c);
			}
#line 2780 "csqpy.c" /* yacc.c:1646  */
    break;

  case 35:
#line 634 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_endctrl();
			}
#line 2795 "csqpy.c" /* yacc.c:1646  */
    break;

  case 36:
#line 644 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_CLONE)
				FAILSYN
			    else if (c->flags & CS_ISCHILD)
			    {
				if (c->state==S_DISCLD)
				    FAILSYN
			    }
			    else if (c->state==S_ACTIVE)
				FAILSYN;
			    rsx_parent(c);
			}
#line 2817 "csqpy.c" /* yacc.c:1646  */
    break;

  case 38:
#line 661 "csqpy.y" /* yacc.c:1646  */
    { 
	    if (pstate->control.count &&
		((controlstack_t *) fgw_stackpeek(&pstate->control))->state!=
		  S_ACTIVE)
		{
		    NEXT_STMT
		}
	  }
#line 2830 "csqpy.c" /* yacc.c:1646  */
    break;

  case 40:
#line 671 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=DONE; }
#line 2836 "csqpy.c" /* yacc.c:1646  */
    break;

  case 41:
#line 672 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_FOR; }
#line 2842 "csqpy.c" /* yacc.c:1646  */
    break;

  case 42:
#line 673 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_FOREACH; }
#line 2848 "csqpy.c" /* yacc.c:1646  */
    break;

  case 43:
#line 674 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_WHILE; }
#line 2854 "csqpy.c" /* yacc.c:1646  */
    break;

  case 44:
#line 675 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_CLONE; }
#line 2860 "csqpy.c" /* yacc.c:1646  */
    break;

  case 45:
#line 676 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=T_WAIT; }
#line 2866 "csqpy.c" /* yacc.c:1646  */
    break;

  case 50:
#line 687 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_FOR;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions|=SAFECOPYHACK;
			}
#line 2880 "csqpy.c" /* yacc.c:1646  */
    break;

  case 51:
#line 695 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions&=~SAFECOPYHACK;
			    rsx_for();
			    FAILCHECK;
			}
#line 2890 "csqpy.c" /* yacc.c:1646  */
    break;

  case 53:
#line 700 "csqpy.y" /* yacc.c:1646  */
    {
			    endstmt_sql();
			    if ((yyvsp[-5].var))
			    {
				instruction_t *i;

				i=rxx_newinstr(pstate);
				FAILCHECK
				i->func=rsx_setcursor;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string= (yyvsp[-5].var);
			        i->constant.flags|=EF_NOTONTSS;
			    }
			    if (rsx_foreach(&(yyvsp[-2].pcode)))
				FAILCHECK;
			}
#line 2911 "csqpy.c" /* yacc.c:1646  */
    break;

  case 55:
#line 716 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_while();
			    FAILCHECK;
			}
#line 2920 "csqpy.c" /* yacc.c:1646  */
    break;

  case 57:
#line 720 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));

			    /* count is in destvar */
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_CLONE;
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
#line 2935 "csqpy.c" /* yacc.c:1646  */
    break;

  case 58:
#line 729 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* destvar is in destvar, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->destvar);

			    /* count is in clone */
			    rxx_swappcode(&pstate->pcode, &c->clone);

			}
#line 2952 "csqpy.c" /* yacc.c:1646  */
    break;

  case 59:
#line 740 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* pid is in clone, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->clone);
			    rsx_clone(c);
			    FAILCHECK;
			}
#line 2967 "csqpy.c" /* yacc.c:1646  */
    break;

  case 61:
#line 750 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions|=SAFECOPYHACK;
			}
#line 2975 "csqpy.c" /* yacc.c:1646  */
    break;

  case 62:
#line 752 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_WAIT;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions&=~SAFECOPYHACK;
			}
#line 2989 "csqpy.c" /* yacc.c:1646  */
    break;

  case 63:
#line 760 "csqpy.y" /* yacc.c:1646  */
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    rxx_swappcode(&pstate->pcode, &c->destvar);
			    rsx_wait(c);
			    FAILCHECK;
			}
#line 3002 "csqpy.c" /* yacc.c:1646  */
    break;

  case 65:
#line 768 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_if();
                            FAILCHECK;
			}
#line 3011 "csqpy.c" /* yacc.c:1646  */
    break;

  case 71:
#line 784 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->phase1.next_stmt_start=pstate->tokstart;
			}
#line 3019 "csqpy.c" /* yacc.c:1646  */
    break;

  case 73:
#line 789 "csqpy.y" /* yacc.c:1646  */
    {
			    YYACCEPT;
			}
#line 3027 "csqpy.c" /* yacc.c:1646  */
    break;

  case 74:
#line 793 "csqpy.y" /* yacc.c:1646  */
    { (yyval.var)=(yyvsp[0].var); }
#line 3033 "csqpy.c" /* yacc.c:1646  */
    break;

  case 75:
#line 794 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->ssp->sqlstart=(yyvsp[-4].tstart);
			    (yyval.var)=NULL;
			}
#line 3042 "csqpy.c" /* yacc.c:1646  */
    break;

  case 76:
#line 798 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->ssp->sqlstart=(yyvsp[-3].tstart);
			    (yyval.var)=NULL;
			}
#line 3051 "csqpy.c" /* yacc.c:1646  */
    break;

  case 77:
#line 802 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_executeout;
			    markend_sql((yyvsp[-3].tstart));
			    endstmt_sql();
			    (yyval.var)=NULL;
			}
#line 3066 "csqpy.c" /* yacc.c:1646  */
    break;

  case 78:
#line 813 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->ssp->sqlstart=(yyvsp[-2].tstart);
			    (yyval.var)=(yyvsp[-3].var);
			}
#line 3075 "csqpy.c" /* yacc.c:1646  */
    break;

  case 79:
#line 817 "csqpy.y" /* yacc.c:1646  */
    { (yyval.var)=NULL; }
#line 3081 "csqpy.c" /* yacc.c:1646  */
    break;

  case 80:
#line 819 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.var)=(yyvsp[-3].var);
				}
#line 3089 "csqpy.c" /* yacc.c:1646  */
    break;

  case 81:
#line 822 "csqpy.y" /* yacc.c:1646  */
    { (yyval.var)=NULL; }
#line 3095 "csqpy.c" /* yacc.c:1646  */
    break;

  case 82:
#line 824 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->stmt->options|=SO_WITHHOLD; }
#line 3101 "csqpy.c" /* yacc.c:1646  */
    break;

  case 84:
#line 831 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_breaks(S_DISABLED);
			    FAILCHECK;
			}
#line 3110 "csqpy.c" /* yacc.c:1646  */
    break;

  case 85:
#line 835 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_breaks(S_CONTINUE);
			    FAILCHECK;
			}
#line 3119 "csqpy.c" /* yacc.c:1646  */
    break;

  case 86:
#line 839 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[-1].instr), sizeof(instruction_t));
			    rsx_dispose();
			    FAILCHECK;
			}
#line 3133 "csqpy.c" /* yacc.c:1646  */
    break;

  case 87:
#line 848 "csqpy.y" /* yacc.c:1646  */
    {
			    rqf_setwidth(pstate->ssp->stmt, pstate->width);
	  		}
#line 3141 "csqpy.c" /* yacc.c:1646  */
    break;

  case 88:
#line 850 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_display(&(yyvsp[-1].pcode));
			    FAILCHECK;
			}
#line 3150 "csqpy.c" /* yacc.c:1646  */
    break;

  case 91:
#line 856 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_width();
			    FAILCHECK;
			}
#line 3159 "csqpy.c" /* yacc.c:1646  */
    break;

  case 93:
#line 861 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_errorlog();
			    FAILCHECK;
			}
#line 3168 "csqpy.c" /* yacc.c:1646  */
    break;

  case 97:
#line 869 "csqpy.y" /* yacc.c:1646  */
    {
/* FIXME: implemented only if using 4gl - DEPRECATE */
#if defined(I4GLVER)
			    char *s=TOKPTR(pstate->tokstart);

			    MARK_STMT
			    endstmt_sql();
			    if (rsx_load(s))
				FAILCHECK
			    YYACCEPT;
#else
			    FAIL(RC_NLNKD);
#endif
			}
#line 3187 "csqpy.c" /* yacc.c:1646  */
    break;

  case 98:
#line 884 "csqpy.y" /* yacc.c:1646  */
    {
/* FIXME: implemented only if using 4gl - DEPRECATE */
#if defined(I4GLVER)
			    char *s=TOKPTR(pstate->tokstart);

			    MARK_STMT
			    endstmt_sql();
			    if (rsx_unload(s))
				FAILCHECK
			    YYACCEPT;
#else
			    FAIL(RC_NLNKD)
#endif
			}
#line 3206 "csqpy.c" /* yacc.c:1646  */
    break;

  case 99:
#line 898 "csqpy.y" /* yacc.c:1646  */
    {
			    if (rsx_exit())
				FAILCHECK;
			}
#line 3215 "csqpy.c" /* yacc.c:1646  */
    break;

  case 100:
#line 902 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_evalexpr();
			    FAILCHECK;
			}
#line 3224 "csqpy.c" /* yacc.c:1646  */
    break;

  case 103:
#line 909 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_outputto("w");
			    FAILCHECK;
			}
#line 3233 "csqpy.c" /* yacc.c:1646  */
    break;

  case 104:
#line 913 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_pipeto();
			    FAILCHECK;
			}
#line 3242 "csqpy.c" /* yacc.c:1646  */
    break;

  case 105:
#line 917 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_writeto();
			    FAILCHECK;
			}
#line 3251 "csqpy.c" /* yacc.c:1646  */
    break;

  case 106:
#line 921 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_appenddefault();
			    FAILCHECK;
			}
#line 3260 "csqpy.c" /* yacc.c:1646  */
    break;

  case 107:
#line 925 "csqpy.y" /* yacc.c:1646  */
    {
			    rsx_outputto("a");
			    FAILCHECK;
			}
#line 3269 "csqpy.c" /* yacc.c:1646  */
    break;

  case 109:
#line 932 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_inputfrom;
			}
#line 3281 "csqpy.c" /* yacc.c:1646  */
    break;

  case 110:
#line 939 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_pipefrom;
			}
#line 3293 "csqpy.c" /* yacc.c:1646  */
    break;

  case 111:
#line 946 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_readfrom;
			}
#line 3305 "csqpy.c" /* yacc.c:1646  */
    break;

  case 112:
#line 954 "csqpy.y" /* yacc.c:1646  */
    {
			    if (pstate->style!=F_NONE)
				FAIL(RC_NOFMT)
			    rsx_flagreset(PF_ISHTML);
			}
#line 3315 "csqpy.c" /* yacc.c:1646  */
    break;

  case 113:
#line 959 "csqpy.y" /* yacc.c:1646  */
    { rsx_flagset(PF_ISHTML); }
#line 3321 "csqpy.c" /* yacc.c:1646  */
    break;

  case 114:
#line 961 "csqpy.y" /* yacc.c:1646  */
    { rsx_flagset(PF_ERRORCONT); }
#line 3327 "csqpy.c" /* yacc.c:1646  */
    break;

  case 115:
#line 962 "csqpy.y" /* yacc.c:1646  */
    { rsx_flagreset(PF_ERRORCONT); }
#line 3333 "csqpy.c" /* yacc.c:1646  */
    break;

  case 120:
#line 970 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;
			    int r;

                            i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[-3].stack).val.string;
			    i->constant.flags|=EF_NOTONTSS;
			    if ((yyvsp[-2].stack).type)
				i->constant.count++;
			    if ((yyvsp[-1].stack).type)
				i->constant.count+=2;
			    switch ((yyvsp[-4].stack).type)
			    {
			      case DEFAULT:
				r=rsx_defconnect(((yyvsp[0].tstart)<0)? NULL: TOKPTR((yyvsp[0].tstart)));
				break;
			      default:
				r=rsx_connect(((yyvsp[0].tstart)<0)? NULL: TOKPTR((yyvsp[0].tstart)));
			    }
			    if (r)
				FAILCHECK;
			}
#line 3363 "csqpy.c" /* yacc.c:1646  */
    break;

  case 121:
#line 995 "csqpy.y" /* yacc.c:1646  */
    {
			    int r;

			    switch ((yyvsp[-1].stack).type)
			    {
			      case CONNECTION:
				r=rsx_setconnection();
				break;
			      case SOURCE:
				r=rsx_setsource((yyvsp[-1].stack).val.string);
				break;
			      default:
				r=rsx_setdefault();
			    }
			    if (r)
				FAILCHECK;
			}
#line 3385 "csqpy.c" /* yacc.c:1646  */
    break;

  case 122:
#line 1012 "csqpy.y" /* yacc.c:1646  */
    {
			    int r;

			    switch ((yyvsp[-1].stack).type)
			    {
			      case CONNECTION:
				r=rsx_disconnect();
				break;
			      case SOURCE:
				r=rsx_srcdisconnect((yyvsp[-1].stack).val.string);
				break;
			      default:
				r=rsx_defdisconnect();
			    }
			    if (r)
				FAILCHECK;
			}
#line 3407 "csqpy.c" /* yacc.c:1646  */
    break;

  case 123:
#line 1030 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=0; }
#line 3413 "csqpy.c" /* yacc.c:1646  */
    break;

  case 124:
#line 1031 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=AS; }
#line 3419 "csqpy.c" /* yacc.c:1646  */
    break;

  case 125:
#line 1033 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=0; }
#line 3425 "csqpy.c" /* yacc.c:1646  */
    break;

  case 126:
#line 1034 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=USER; }
#line 3431 "csqpy.c" /* yacc.c:1646  */
    break;

  case 127:
#line 1036 "csqpy.y" /* yacc.c:1646  */
    { (yyval.tstart)=-1; }
#line 3437 "csqpy.c" /* yacc.c:1646  */
    break;

  case 128:
#line 1037 "csqpy.y" /* yacc.c:1646  */
    { (yyval.tstart)=(yyvsp[0].tstart); }
#line 3443 "csqpy.c" /* yacc.c:1646  */
    break;

  case 129:
#line 1039 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=DEFAULT; }
#line 3449 "csqpy.c" /* yacc.c:1646  */
    break;

  case 130:
#line 1040 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=CONNECTION; }
#line 3455 "csqpy.c" /* yacc.c:1646  */
    break;

  case 131:
#line 1042 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack)=(yyvsp[0].stack); }
#line 3461 "csqpy.c" /* yacc.c:1646  */
    break;

  case 132:
#line 1043 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=CONNECTION; }
#line 3467 "csqpy.c" /* yacc.c:1646  */
    break;

  case 133:
#line 1045 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).val.string=NULL;
			    (yyval.stack).type=DEFAULT;
			}
#line 3476 "csqpy.c" /* yacc.c:1646  */
    break;

  case 134:
#line 1049 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).val.string=(yyvsp[0].var);
			    (yyval.stack).type=SOURCE;
			}
#line 3485 "csqpy.c" /* yacc.c:1646  */
    break;

  case 136:
#line 1059 "csqpy.y" /* yacc.c:1646  */
    {
			    endstmt_sql();
			    if (rsx_prepare(&(yyvsp[-1].pcode)))
				FAILCHECK;
			}
#line 3495 "csqpy.c" /* yacc.c:1646  */
    break;

  case 137:
#line 1066 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executeout;
			}
#line 3507 "csqpy.c" /* yacc.c:1646  */
    break;

  case 139:
#line 1073 "csqpy.y" /* yacc.c:1646  */
    {
			    if (rsx_free())
				FAILCHECK;
			}
#line 3516 "csqpy.c" /* yacc.c:1646  */
    break;

  case 140:
#line 1079 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[0].tstart); }
#line 3522 "csqpy.c" /* yacc.c:1646  */
    break;

  case 141:
#line 1080 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[-1].tstart); }
#line 3528 "csqpy.c" /* yacc.c:1646  */
    break;

  case 142:
#line 1081 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[0].tstart); }
#line 3534 "csqpy.c" /* yacc.c:1646  */
    break;

  case 144:
#line 1085 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=pstate->tokstart; }
#line 3540 "csqpy.c" /* yacc.c:1646  */
    break;

  case 146:
#line 1088 "csqpy.y" /* yacc.c:1646  */
    {
			    endstmt_sql();
			    if (rsx_sqlrun(STMTPTR(pstate->ssp)))
		    		FAILCHECK;
			}
#line 3550 "csqpy.c" /* yacc.c:1646  */
    break;

  case 152:
#line 1102 "csqpy.y" /* yacc.c:1646  */
    {
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    pstate->parseroptions|=AGGREGATEHACK;
	  }
#line 3560 "csqpy.c" /* yacc.c:1646  */
    break;

  case 153:
#line 1106 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions&=~AGGREGATEHACK;
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    markend_sql((yyvsp[-2].tstart));
			}
#line 3571 "csqpy.c" /* yacc.c:1646  */
    break;

  case 158:
#line 1119 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
		   	    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=AGGREGATE;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
#line 3587 "csqpy.c" /* yacc.c:1646  */
    break;

  case 159:
#line 1129 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
#line 3599 "csqpy.c" /* yacc.c:1646  */
    break;

  case 167:
#line 1147 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
		   	    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=AGGREGATE;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
#line 3615 "csqpy.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1157 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
#line 3627 "csqpy.c" /* yacc.c:1646  */
    break;

  case 173:
#line 1171 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrbreak;
			    if ((yyvsp[-1].stack).type==REJECT)
				i->flags|=RT_HASREJECT;
			}
#line 3641 "csqpy.c" /* yacc.c:1646  */
    break;

  case 174:
#line 1180 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrcont;
			    if ((yyvsp[-1].stack).type==REJECT)
				i->flags|=RT_HASREJECT;
			}
#line 3655 "csqpy.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1190 "csqpy.y" /* yacc.c:1646  */
    { (yyval.stack).type=0; }
#line 3661 "csqpy.c" /* yacc.c:1646  */
    break;

  case 176:
#line 1191 "csqpy.y" /* yacc.c:1646  */
    {
			    if (!pstate->ssp->stmt->reject)
				FAIL(RC_NRJCT)
			    (yyval.stack).type=REJECT;
			}
#line 3671 "csqpy.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1198 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
#line 3683 "csqpy.c" /* yacc.c:1646  */
    break;

  case 179:
#line 1205 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    if ((yyvsp[0].stack).count!=(yyvsp[-1].fntok)->tl_parmcount && (yyvsp[-1].fntok)->tl_parmcount>=0)
				FAIL(RC_WACNT)
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[-1].fntok)->tl_func;
			}
#line 3697 "csqpy.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1213 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			    i->flags|=RT_DITCHRES;
			}
#line 3710 "csqpy.c" /* yacc.c:1646  */
    break;

  case 181:
#line 1221 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
#line 3722 "csqpy.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1229 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[0].instr), sizeof(instruction_t));
			}
#line 3734 "csqpy.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1237 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
#line 3746 "csqpy.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1244 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions&=~FUNCHACK;
			}
#line 3754 "csqpy.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1246 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[-3].instr).func;
			    i->constant.type=(yyvsp[-3].instr).constant.type;
			    i->constant.val.intvalue=
			        (yyvsp[-3].instr).constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
#line 3772 "csqpy.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1259 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[0].instr).func;
			    i->constant.type=(yyvsp[0].instr).constant.type;
			    i->constant.val.intvalue=
			        (yyvsp[0].instr).constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
		   }
#line 3794 "csqpy.c" /* yacc.c:1646  */
    break;

  case 188:
#line 1276 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			}
#line 3806 "csqpy.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1282 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[-3].instr).func;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
#line 3821 "csqpy.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1292 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			    i->flags=(yyvsp[-2].instr).flags;
			}
#line 3834 "csqpy.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1304 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 3840 "csqpy.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1306 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.instr).func=rxo_add;
			}
#line 3848 "csqpy.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1309 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.instr).func=rxo_sub;
			}
#line 3856 "csqpy.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1312 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.instr).func=rxo_mul;
			}
#line 3864 "csqpy.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1315 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.instr).func=rxo_div;
			}
#line 3872 "csqpy.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1318 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.instr).func=rxo_dpipe;
			}
#line 3880 "csqpy.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1323 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    if ((yyvsp[-1].fntok)->tl_parmcount>=0)
				(yyvsp[0].insptr)->constant.val.intvalue=(yyvsp[-1].fntok)->tl_parmcount;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[-1].fntok)->tl_func;
			}
#line 3894 "csqpy.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1333 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			    i->constant.count=0;
			}
#line 3907 "csqpy.c" /* yacc.c:1646  */
    break;

  case 200:
#line 1341 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[0].stack).count;
			    i->constant.flags|=(yyvsp[0].stack).flags;
			}
#line 3921 "csqpy.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1351 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions|=RESVARHACK;
			}
#line 3929 "csqpy.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1353 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->parseroptions&=~RESVARHACK;
			    (yyval.stack)=(yyvsp[0].stack);
			}
#line 3938 "csqpy.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1358 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).count=(yyvsp[-2].stack).count+1;
			    (yyval.stack).flags=(yyvsp[-2].stack).flags|(yyvsp[0].stack).flags;
			}
#line 3947 "csqpy.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1362 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).count=1;
			    (yyval.stack).flags=(yyvsp[0].stack).flags;
			}
#line 3956 "csqpy.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1367 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).flags=0;
			}
#line 3964 "csqpy.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1370 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 3970 "csqpy.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1370 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    (yyval.stack).flags=0;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[-3].instr), sizeof(instruction_t));
			    i->constant.totype=(yyvsp[0].instr).constant.totype;
			    i->constant.qual=(yyvsp[0].instr).constant.val.intvalue;
			}
#line 3986 "csqpy.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1381 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    (yyval.stack).flags=EF_VARIABLEDEST;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[-2].instr), sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			    i->constant.flags|=EF_EXPANDHASH;
			}
#line 4001 "csqpy.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1391 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[-2].instr), sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			}
#line 4014 "csqpy.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1398 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_identlist;
			    (yyval.stack).flags=EF_VARIABLELIST;
			}
#line 4027 "csqpy.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1407 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[0].stack).count;
			    i->constant.flags=(yyvsp[0].stack).flags;
			    (yyval.stack).count=(yyvsp[0].stack).count;
			    (yyval.stack).flags=(yyvsp[0].stack).flags;
			}
#line 4043 "csqpy.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1419 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[0].stack).count;
			    i->constant.flags|=(yyvsp[0].stack).flags;
			}
#line 4057 "csqpy.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1429 "csqpy.y" /* yacc.c:1646  */
    {
			    endstmt_sql();
			    if (rsx_nonredirected(&(yyvsp[-1].pcode)))
				FAILCHECK;
			}
#line 4067 "csqpy.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1434 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    i->func=rsx_redirect;
			}
#line 4079 "csqpy.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1440 "csqpy.y" /* yacc.c:1646  */
    {
			    endstmt_sql();
/*
** only an insert with values clause or execute procedure, both with
** placeholders will do!
*/
			    if ((pstate->parseroptions & VALUESHACK))
				FAIL(RC_NOSTR);
			    if (rsx_redirected())
				FAILCHECK;
			}
#line 4095 "csqpy.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1452 "csqpy.y" /* yacc.c:1646  */
    {
			    markend_sql((yyvsp[0].tstart)-1);
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM)
			    pstate->ssp->sqlstart=(yyvsp[0].tstart);
			    pstate->ssp->stmt->options|=SO_INSCURS;
			    if (pstate->errorinfo)
				pstate->ssp->line_count=pstate->errorinfo->line_count;
			}
#line 4110 "csqpy.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1462 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executein;
			    markend_sql((yyvsp[-1].tstart));
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
#line 4126 "csqpy.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1474 "csqpy.y" /* yacc.c:1646  */
    { (yyval.tstart)=(yyvsp[0].tstart); }
#line 4132 "csqpy.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1475 "csqpy.y" /* yacc.c:1646  */
    { (yyval.tstart)=(yyvsp[-1].tstart); }
#line 4138 "csqpy.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1476 "csqpy.y" /* yacc.c:1646  */
    { (yyval.tstart)=(yyvsp[0].tstart); }
#line 4144 "csqpy.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1479 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_autocommit;
			}
#line 4156 "csqpy.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1487 "csqpy.y" /* yacc.c:1646  */
    {}
#line 4162 "csqpy.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1488 "csqpy.y" /* yacc.c:1646  */
    {}
#line 4168 "csqpy.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1489 "csqpy.y" /* yacc.c:1646  */
    {}
#line 4174 "csqpy.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1491 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[0].tstart); }
#line 4180 "csqpy.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1492 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[0].tstart); }
#line 4186 "csqpy.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1493 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->sqlstart=(yyvsp[0].tstart); }
#line 4192 "csqpy.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1495 "csqpy.y" /* yacc.c:1646  */
    {
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
#line 4201 "csqpy.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1501 "csqpy.y" /* yacc.c:1646  */
    {
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
	  }
#line 4209 "csqpy.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1503 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_listcount;
                            i->constant.count=(yyvsp[0].stack).count;
			    i->constant.flags|=(yyvsp[0].stack).flags;
                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_into;
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
			    markend_sql((yyvsp[-2].tstart));
			}
#line 4228 "csqpy.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1518 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_curcon;
			}
#line 4240 "csqpy.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1525 "csqpy.y" /* yacc.c:1646  */
    {
                            instruction_t *i;

			    markend_sql((yyvsp[-1].tstart));
                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    if ((yyvsp[0].stack).type==SOURCE)
			    {
				i->func=rsx_srcdefault;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string=(yyvsp[0].stack).val.string;
				i->constant.flags|=EF_NOTONTSS;
			    }
			    else if ((yyvsp[0].stack).type==CONNECTION)
				i->func=rsx_tmpcon;
			    else
				i->func=rsx_curdefault;
			}
#line 4263 "csqpy.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1544 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=RESVARHACK; }
#line 4269 "csqpy.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1545 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions&=~RESVARHACK; }
#line 4275 "csqpy.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1546 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=AGGREGATEHACK; }
#line 4281 "csqpy.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1547 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions&=~AGGREGATEHACK; }
#line 4287 "csqpy.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1550 "csqpy.y" /* yacc.c:1646  */
    {
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
	  }
#line 4296 "csqpy.c" /* yacc.c:1646  */
    break;

  case 242:
#line 1553 "csqpy.y" /* yacc.c:1646  */
    {
			    markend_sql((yyvsp[-2].tstart));
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
			}
#line 4306 "csqpy.c" /* yacc.c:1646  */
    break;

  case 243:
#line 1562 "csqpy.y" /* yacc.c:1646  */
    {
			    pstate->ssp->stmt->options|=SO_TABULAR;
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			}
#line 4315 "csqpy.c" /* yacc.c:1646  */
    break;

  case 244:
#line 1566 "csqpy.y" /* yacc.c:1646  */
    {
			    markend_sql((yyvsp[-1].tstart));
			    pstate->ssp->stmt->width=pstate->width;
			    if (!(yyvsp[0].pcode).pcodehead)
				pstate->ssp->stmt->options|=SO_TABULAR;
			    memcpy(&(yyval.pcode), &(yyvsp[0].pcode), sizeof(pcode_t));
			}
#line 4327 "csqpy.c" /* yacc.c:1646  */
    break;

  case 245:
#line 1574 "csqpy.y" /* yacc.c:1646  */
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    pstate->ssp->stmt->fmt_type=FMT_BRIEF;
			}
#line 4336 "csqpy.c" /* yacc.c:1646  */
    break;

  case 246:
#line 1578 "csqpy.y" /* yacc.c:1646  */
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
#line 4345 "csqpy.c" /* yacc.c:1646  */
    break;

  case 247:
#line 1581 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.pcode)=(yyvsp[-2].pcode);
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
#line 4354 "csqpy.c" /* yacc.c:1646  */
    break;

  case 248:
#line 1585 "csqpy.y" /* yacc.c:1646  */
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
#line 4363 "csqpy.c" /* yacc.c:1646  */
    break;

  case 249:
#line 1588 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.pcode)=(yyvsp[-2].pcode);
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			    pstate->ssp->stmt->fmt_type=FMT_DELIMITED;
			}
#line 4373 "csqpy.c" /* yacc.c:1646  */
    break;

  case 250:
#line 1594 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->stmt->fmt_type=FMT_FULL; }
#line 4379 "csqpy.c" /* yacc.c:1646  */
    break;

  case 251:
#line 1595 "csqpy.y" /* yacc.c:1646  */
    { pstate->ssp->stmt->fmt_type=FMT_VERTICAL; }
#line 4385 "csqpy.c" /* yacc.c:1646  */
    break;

  case 252:
#line 1597 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    rxu_setnull(&i->constant, CINTTYPE);
		        }
#line 4398 "csqpy.c" /* yacc.c:1646  */
    break;

  case 254:
#line 1607 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			}
#line 4410 "csqpy.c" /* yacc.c:1646  */
    break;

  case 256:
#line 1616 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    switch ((yyvsp[0].stack).type)
			    {
			      case DELIMITED:
				i->func=rsx_delimited;
				break;
			      case ~DELIMITED:
				i->func=rsx_regex;
				break;
			      case EXTRACTOR:
				i->func=rsx_extractor;
				break;
			      case PATTERN:
				i->func=rsx_pattern;
				break;
			    }
			}
#line 4436 "csqpy.c" /* yacc.c:1646  */
    break;

  case 257:
#line 1638 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).type=(yyvsp[0].stack).type;
			}
#line 4444 "csqpy.c" /* yacc.c:1646  */
    break;

  case 258:
#line 1641 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[-2].var);
			    i->constant.flags|=EF_NOTONTSS;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[0].var);
			    i->constant.flags|=EF_NOTONTSS;
			    (yyval.stack).type=EXTRACTOR;
			}
#line 4468 "csqpy.c" /* yacc.c:1646  */
    break;

  case 259:
#line 1661 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).type=PATTERN;
			}
#line 4476 "csqpy.c" /* yacc.c:1646  */
    break;

  case 260:
#line 1664 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).type=DELIMITED;
			}
#line 4484 "csqpy.c" /* yacc.c:1646  */
    break;

  case 261:
#line 1667 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.stack).type=~DELIMITED;
			}
#line 4492 "csqpy.c" /* yacc.c:1646  */
    break;

  case 265:
#line 1676 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    if (!(i->constant.val.string=getenv("DBDELIMITER")))
				i->constant.val.string="|";
			}
#line 4509 "csqpy.c" /* yacc.c:1646  */
    break;

  case 266:
#line 1689 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    i->constant.val.string="\\";
			}
#line 4525 "csqpy.c" /* yacc.c:1646  */
    break;

  case 268:
#line 1702 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.flags|=EF_NOTONTSS;
			    i->constant.val.string="[ \t]+";
			}
#line 4541 "csqpy.c" /* yacc.c:1646  */
    break;

  case 272:
#line 1718 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectfile;
			}
#line 4553 "csqpy.c" /* yacc.c:1646  */
    break;

  case 274:
#line 1727 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectsize;
			}
#line 4565 "csqpy.c" /* yacc.c:1646  */
    break;

  case 278:
#line 1746 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}
#line 4578 "csqpy.c" /* yacc.c:1646  */
    break;

  case 279:
#line 1758 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyvsp[-1].instr).func=rxo_variable;
				    (yyvsp[-1].instr).flags=(yyvsp[0].instr).flags;
				    if (pstate->parseroptions & SAFECOPYHACK)
					(yyvsp[-1].instr).flags|=RT_SAFECOPY;
				    fgw_move(i, &(yyvsp[-1].instr), sizeof(instruction_t));
				}
#line 4594 "csqpy.c" /* yacc.c:1646  */
    break;

  case 280:
#line 1769 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
#line 4606 "csqpy.c" /* yacc.c:1646  */
    break;

  case 281:
#line 1776 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-3].fntok)->tl_func;
				    i->constant.flags=EF_FUNCHASH;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
#line 4625 "csqpy.c" /* yacc.c:1646  */
    break;

  case 282:
#line 1790 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    i->func=(yyvsp[0].fntok)->tl_func;
				}
#line 4639 "csqpy.c" /* yacc.c:1646  */
    break;

  case 283:
#line 1799 "csqpy.y" /* yacc.c:1646  */
    {
		   parsestack_t p;

		   p.type=CASE;
		   fgw_stackpush(&pstate->parsestack, (char *) &p);
	       }
#line 4650 "csqpy.c" /* yacc.c:1646  */
    break;

  case 284:
#line 1804 "csqpy.y" /* yacc.c:1646  */
    {
				    parsestack_t *p;
				    instruction_t *e;

				    e=rxx_setjump(pstate);
				    for (;;)
				    {
					p=(parsestack_t *) fgw_stackpop(
							&pstate->parsestack);
					if (p->type==CASE)
					    break;
				        else if (p->type==THEN)
					    p->instr->skip_to=e;
				    }
				}
#line 4670 "csqpy.c" /* yacc.c:1646  */
    break;

  case 285:
#line 1819 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_dpipe;
				}
#line 4682 "csqpy.c" /* yacc.c:1646  */
    break;

  case 286:
#line 1826 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_picture;
				}
#line 4694 "csqpy.c" /* yacc.c:1646  */
    break;

  case 287:
#line 1833 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_add;
				}
#line 4706 "csqpy.c" /* yacc.c:1646  */
    break;

  case 288:
#line 1840 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_sub;
				}
#line 4718 "csqpy.c" /* yacc.c:1646  */
    break;

  case 289:
#line 1847 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_mul;
				}
#line 4730 "csqpy.c" /* yacc.c:1646  */
    break;

  case 290:
#line 1854 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_div;
				}
#line 4742 "csqpy.c" /* yacc.c:1646  */
    break;

  case 291:
#line 1861 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 4748 "csqpy.c" /* yacc.c:1646  */
    break;

  case 292:
#line 1861 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_units;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=TU_IENCODE(5,
						(yyvsp[0].stack).type, (yyvsp[0].stack).type);
				}
#line 4764 "csqpy.c" /* yacc.c:1646  */
    break;

  case 293:
#line 1872 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 4770 "csqpy.c" /* yacc.c:1646  */
    break;

  case 294:
#line 1872 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[0].instr).func;
				    i->constant.type=(yyvsp[0].instr).constant.type;
				    i->constant.val.intvalue=
					(yyvsp[0].instr).constant.val.intvalue;
			}
#line 4786 "csqpy.c" /* yacc.c:1646  */
    break;

  case 295:
#line 1883 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 4792 "csqpy.c" /* yacc.c:1646  */
    break;

  case 296:
#line 1883 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-1].instr).func;
				    i->constant.type=(yyvsp[-1].instr).constant.type;
				    i->constant.val.intvalue=
					(yyvsp[-1].instr).constant.val.intvalue;
				}
#line 4808 "csqpy.c" /* yacc.c:1646  */
    break;

  case 297:
#line 1894 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_minus;
				}
#line 4819 "csqpy.c" /* yacc.c:1646  */
    break;

  case 298:
#line 1900 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_plus;
				}
#line 4830 "csqpy.c" /* yacc.c:1646  */
    break;

  case 299:
#line 1906 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    i->func=rxo_colvalue;
				}
#line 4843 "csqpy.c" /* yacc.c:1646  */
    break;

  case 301:
#line 1920 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    fgw_move(&i->constant, &(yyvsp[0].stack),
						sizeof(exprstack_t));
				    i->constant.flags|=EF_CONSTANT;
				    if ((yyvsp[0].stack).type==CSTRINGTYPE)
				    {
					fgw_tssdetach(&pstate->lexhead,
						      (yyvsp[0].stack).val.string);
					fgw_tssattach(&pstate->pcode.parsehead,
						      (yyvsp[0].stack).val.string);
				    }
				}
#line 4865 "csqpy.c" /* yacc.c:1646  */
    break;

  case 302:
#line 1937 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyvsp[0].instr).func=rxo_variable;
				    if (pstate->parseroptions & SAFECOPYHACK)
					(yyvsp[0].instr).flags|=RT_SAFECOPY;
				    fgw_move(i, &(yyvsp[0].instr), sizeof(instruction_t));
				}
#line 4880 "csqpy.c" /* yacc.c:1646  */
    break;

  case 305:
#line 1954 "csqpy.y" /* yacc.c:1646  */
    {
			    parsestack_t p;

			    p.type=IN;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
#line 4891 "csqpy.c" /* yacc.c:1646  */
    break;

  case 306:
#line 1959 "csqpy.y" /* yacc.c:1646  */
    {
				    parsestack_t *p;
				    instruction_t *e;

				    e=rxx_setjump(pstate);
				    p=(parsestack_t *)
					fgw_stackpop(&pstate->parsestack);
				    p->instr->func=((yyvsp[-4].stack).type==IN)?
							rxo_inend: rxo_niend;
				    for (;;)
				    {
					p=(parsestack_t *)
					    fgw_stackpop(&pstate->parsestack);
					if (p->type==IN)
					    break;
				        else if (p->type==~IN)
					{
					    p->instr->func=((yyvsp[-4].stack).type==IN)?
							   rxo_in: rxo_ni;
					    p->instr->skip_to=e;
					}
				    }
				}
#line 4919 "csqpy.c" /* yacc.c:1646  */
    break;

  case 307:
#line 1982 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_between;
				    i->constant.type=((yyvsp[-3].stack).type==BETWEEN);
				}
#line 4932 "csqpy.c" /* yacc.c:1646  */
    break;

  case 308:
#line 1990 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_matches;
				    i->constant.type=((yyvsp[-2].stack).type==MATCHES);
				}
#line 4945 "csqpy.c" /* yacc.c:1646  */
    break;

  case 309:
#line 1998 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_like;
				    i->constant.type=((yyvsp[-2].stack).type==LIKE);
				}
#line 4958 "csqpy.c" /* yacc.c:1646  */
    break;

  case 310:
#line 2006 "csqpy.y" /* yacc.c:1646  */
    {
#ifdef HAVE_REGEX
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_regexp;
				    i->constant.type=((yyvsp[-1].stack).type==REGEXP);
#else
				    FAIL(RC_NLNKD)
#endif
				}
#line 4975 "csqpy.c" /* yacc.c:1646  */
    break;

  case 311:
#line 2018 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_or;
				}
#line 4987 "csqpy.c" /* yacc.c:1646  */
    break;

  case 312:
#line 2025 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_and;
				}
#line 4999 "csqpy.c" /* yacc.c:1646  */
    break;

  case 313:
#line 2032 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_not;
				}
#line 5011 "csqpy.c" /* yacc.c:1646  */
    break;

  case 314:
#line 2039 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_isnull;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=((yyvsp[0].stack).type==NULLVALUE);
				}
#line 5025 "csqpy.c" /* yacc.c:1646  */
    break;

  case 315:
#line 2048 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_eq;
				}
#line 5037 "csqpy.c" /* yacc.c:1646  */
    break;

  case 316:
#line 2055 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ne;
				}
#line 5049 "csqpy.c" /* yacc.c:1646  */
    break;

  case 317:
#line 2062 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_le;
				}
#line 5061 "csqpy.c" /* yacc.c:1646  */
    break;

  case 318:
#line 2069 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_lt;
				}
#line 5073 "csqpy.c" /* yacc.c:1646  */
    break;

  case 319:
#line 2076 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ge;
				}
#line 5085 "csqpy.c" /* yacc.c:1646  */
    break;

  case 320:
#line 2083 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_gt;
				}
#line 5097 "csqpy.c" /* yacc.c:1646  */
    break;

  case 322:
#line 2092 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_tointeger;
				    (yyval.instr).constant.totype=CINTTYPE;
				}
#line 5106 "csqpy.c" /* yacc.c:1646  */
    break;

  case 323:
#line 2096 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_todouble;
				    (yyval.instr).constant.totype=CFLOATTYPE;
				}
#line 5115 "csqpy.c" /* yacc.c:1646  */
    break;

  case 324:
#line 2100 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_todecimal;
				    (yyval.instr).constant.type=CDECIMALTYPE;
				    (yyval.instr).constant.totype=CDECIMALTYPE;
				}
#line 5125 "csqpy.c" /* yacc.c:1646  */
    break;

  case 325:
#line 2105 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_todecimal;
				    (yyval.instr).constant.type=CMONEYTYPE;
				    (yyval.instr).constant.totype=CMONEYTYPE;
				}
#line 5135 "csqpy.c" /* yacc.c:1646  */
    break;

  case 326:
#line 2110 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_todate;
				    (yyval.instr).constant.totype=CDATETYPE;
				}
#line 5144 "csqpy.c" /* yacc.c:1646  */
    break;

  case 327:
#line 2114 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_todatetime;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.totype=CDTIMETYPE;
				    (yyval.instr).constant.val.intvalue=(yyvsp[0].stack).val.intvalue;
				}
#line 5155 "csqpy.c" /* yacc.c:1646  */
    break;

  case 328:
#line 2120 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_tointerval;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.totype=CINVTYPE;
				    (yyval.instr).constant.val.intvalue=(yyvsp[0].stack).val.intvalue;
				}
#line 5166 "csqpy.c" /* yacc.c:1646  */
    break;

  case 329:
#line 2126 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_tostring;
				    (yyval.instr).constant.totype=CSTRINGTYPE;
				}
#line 5175 "csqpy.c" /* yacc.c:1646  */
    break;

  case 330:
#line 2130 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_tobyte;
				    (yyval.instr).constant.totype=FGWBYTETYPE;
				}
#line 5184 "csqpy.c" /* yacc.c:1646  */
    break;

  case 331:
#line 2135 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[0].instr), sizeof(instruction_t));
				}
#line 5196 "csqpy.c" /* yacc.c:1646  */
    break;

  case 332:
#line 2143 "csqpy.y" /* yacc.c:1646  */
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=(yyvsp[0].stack).type;
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[0].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[0].stack).flags;
				    if ((yyvsp[0].stack).count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[-1].var), &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    (yyval.instr).constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    (yyval.instr).constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							       (yyvsp[-1].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
							      (yyvsp[-1].var), 32))
				    {
					if (e->type==T_HASH)
					     FAIL(RC_INHST)
					(yyval.instr).constant.val.identifier=e->magic;
				    }
				    else
					FAILCHECK
				    fgw_tssfree(&pstate->lexhead, (yyvsp[-1].var));
				    (yyval.instr).flags=RT_RESVAR;
				}
#line 5242 "csqpy.c" /* yacc.c:1646  */
    break;

  case 333:
#line 2184 "csqpy.y" /* yacc.c:1646  */
    {
				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    (yyval.instr).func=rxo_column;
				}
#line 5252 "csqpy.c" /* yacc.c:1646  */
    break;

  case 334:
#line 2190 "csqpy.y" /* yacc.c:1646  */
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=(yyvsp[0].stack).type;
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[0].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[0].stack).flags;
				    if ((yyvsp[0].stack).count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[-1].var), &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    (yyval.instr).constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    (yyval.instr).constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							   (yyvsp[-1].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     (yyvsp[-1].var), (pstate->parseroptions &
						  RESVARHACK)?  32: 0))
				    {
					if (e->type==T_HASH)
					     FAIL(RC_INHST)
					(yyval.instr).constant.val.identifier=e->magic;
				    }
				    else  if (pstate->parseroptions & RESVARHACK)
					FAIL(status)
				    else
					(yyval.instr).constant.val.identifier=0;
				    fgw_tssfree(&pstate->lexhead, (yyvsp[-1].var));
				}
#line 5300 "csqpy.c" /* yacc.c:1646  */
    break;

  case 335:
#line 2234 "csqpy.y" /* yacc.c:1646  */
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=((yyvsp[0].stack).type|RT_HASHVAR);
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[0].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[0].stack).flags;
				    if ((yyvsp[0].stack).count || (pstate->parseroptions & RESVARHACK))
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[-1].var), &d))
					{
					    if (e->type!=T_HASH) 
						FAIL(RC_INSCL)
					    (yyval.instr).constant.val.identifier=e->magic;
				    	}
					else if (status)
					    FAIL(status)
					else
					{
					    (yyval.instr).constant.val.identifier=
						fgw_hstaddnode(pstate->vars, 0,
							   (yyvsp[-1].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     (yyvsp[-1].var), 0))
					(yyval.instr).constant.val.identifier=e->magic;
				    else
					(yyval.instr).constant.val.identifier=0;
				    fgw_tssfree(&pstate->lexhead, (yyvsp[-1].var));
				}
#line 5341 "csqpy.c" /* yacc.c:1646  */
    break;

  case 336:
#line 2271 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=0;
				    (yyval.stack).count=0;
				}
#line 5350 "csqpy.c" /* yacc.c:1646  */
    break;

  case 337:
#line 2275 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=RT_HASHASH;
				    (yyval.stack).count=(yyvsp[-1].stack).count;
				    (yyval.stack).flags=(yyvsp[-1].stack).flags;
				}
#line 5360 "csqpy.c" /* yacc.c:1646  */
    break;

  case 338:
#line 2285 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_user;
				}
#line 5375 "csqpy.c" /* yacc.c:1646  */
    break;

  case 339:
#line 2298 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_wait;
				}
#line 5390 "csqpy.c" /* yacc.c:1646  */
    break;

  case 340:
#line 2311 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_countstar;
				}
#line 5404 "csqpy.c" /* yacc.c:1646  */
    break;

  case 341:
#line 2320 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[-1].instr), sizeof(instruction_t));
				    i->func=rxo_counthash;
				    i->flags|=RT_RESVAR;
				}
#line 5418 "csqpy.c" /* yacc.c:1646  */
    break;

  case 342:
#line 2329 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-1].fntok)->tl_func;
				    i->constant.flags=EF_FUNCCOUNT;
				}
#line 5431 "csqpy.c" /* yacc.c:1646  */
    break;

  case 343:
#line 2337 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;
				    fgw_objtype *obj;

				    if (!(obj=rqx_getobject(pstate, (yyvsp[-3].var), (yyvsp[-1].var))))
					 FAIL(pstate->ca.sqlcode)
				    fgw_tssfree(&pstate->lexhead, (yyvsp[-3].var));
				    fgw_tssfree(&pstate->lexhead, (yyvsp[-1].var));
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=obj->obj;
				    i->src_id=obj->src_id;
				}
#line 5449 "csqpy.c" /* yacc.c:1646  */
    break;

  case 344:
#line 2350 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    if ((yyvsp[0].insptr)->constant.count>0 && (yyvsp[-1].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ((yyvsp[-1].fntok)->tl_parmcount>0)
					(yyvsp[0].insptr)->constant.val.intvalue=(yyvsp[-1].fntok)->tl_parmcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-1].fntok)->tl_func;
				}
#line 5465 "csqpy.c" /* yacc.c:1646  */
    break;

  case 345:
#line 2361 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 5471 "csqpy.c" /* yacc.c:1646  */
    break;

  case 346:
#line 2361 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[0].instr).func;
				    i->constant.type=(yyvsp[0].instr).constant.type;
				    i->constant.val=(yyvsp[0].instr).constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-2].fntok)->tl_func;
				}
#line 5493 "csqpy.c" /* yacc.c:1646  */
    break;

  case 347:
#line 2386 "csqpy.y" /* yacc.c:1646  */
    { pstate->parseroptions|=FUNCHACK; }
#line 5499 "csqpy.c" /* yacc.c:1646  */
    break;

  case 348:
#line 2386 "csqpy.y" /* yacc.c:1646  */
    {
				    exprstack_t *s;
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    if ((yyvsp[-3].stack).count>0 && (yyvsp[-5].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[0].instr).func;
				    i->constant.type=(yyvsp[0].instr).constant.type;
				    i->constant.val=(yyvsp[0].instr).constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=(yyvsp[-3].stack).count+1;
				    i->constant.flags|=(yyvsp[-3].stack).flags;
				    if ((yyvsp[-5].fntok)->tl_parmcount>0)
					i->constant.val.intvalue=(yyvsp[-5].fntok)->tl_parmcount+1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-5].fntok)->tl_func;
				}
#line 5527 "csqpy.c" /* yacc.c:1646  */
    break;

  case 349:
#line 2410 "csqpy.y" /* yacc.c:1646  */
    { (yyval.fntok)=(yyvsp[0].fntok); }
#line 5533 "csqpy.c" /* yacc.c:1646  */
    break;

  case 350:
#line 2411 "csqpy.y" /* yacc.c:1646  */
    { (yyval.fntok)=(yyvsp[0].fntok); }
#line 5539 "csqpy.c" /* yacc.c:1646  */
    break;

  case 351:
#line 2413 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				}
#line 5551 "csqpy.c" /* yacc.c:1646  */
    break;

  case 352:
#line 2421 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.insptr)=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyval.insptr)->func=rxo_emptylist;
				}
#line 5561 "csqpy.c" /* yacc.c:1646  */
    break;

  case 353:
#line 2426 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.insptr)=(yyvsp[-1].insptr);
				}
#line 5569 "csqpy.c" /* yacc.c:1646  */
    break;

  case 354:
#line 2430 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).flags=RT_HASSUB1;
				}
#line 5577 "csqpy.c" /* yacc.c:1646  */
    break;

  case 355:
#line 2433 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).flags=RT_HASSUB2;
				}
#line 5585 "csqpy.c" /* yacc.c:1646  */
    break;

  case 356:
#line 2437 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.instr).func=rxo_constant;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.val.intvalue=-1;
				}
#line 5595 "csqpy.c" /* yacc.c:1646  */
    break;

  case 357:
#line 2442 "csqpy.y" /* yacc.c:1646  */
    {
				    if ((yyvsp[-2].stack).type>(yyvsp[0].stack).type)
					FAIL(RC_IQUAL)
				    (yyval.instr).func=rxo_constant;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.val.intvalue=TU_DTENCODE((yyvsp[-2].stack).type,
								     (yyvsp[0].stack).type);
				}
#line 5608 "csqpy.c" /* yacc.c:1646  */
    break;

  case 358:
#line 2451 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=CINTTYPE;
				    (yyval.stack).val.intvalue=-1;
				}
#line 5617 "csqpy.c" /* yacc.c:1646  */
    break;

  case 359:
#line 2455 "csqpy.y" /* yacc.c:1646  */
    {
				    if ((yyvsp[-2].stack).type>(yyvsp[0].stack).type)
					FAIL(RC_IQUAL)
				    (yyval.stack).type=CINTTYPE;
				    (yyval.stack).val.intvalue=TU_DTENCODE((yyvsp[-2].stack).type,
								(yyvsp[0].stack).type);
				}
#line 5629 "csqpy.c" /* yacc.c:1646  */
    break;

  case 360:
#line 2463 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack)=(yyvsp[0].stack);
				}
#line 5637 "csqpy.c" /* yacc.c:1646  */
    break;

  case 361:
#line 2466 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_F1;
				}
#line 5645 "csqpy.c" /* yacc.c:1646  */
    break;

  case 362:
#line 2470 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack)=(yyvsp[0].stack);
				}
#line 5653 "csqpy.c" /* yacc.c:1646  */
    break;

  case 363:
#line 2473 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack)=(yyvsp[0].stack);
				}
#line 5661 "csqpy.c" /* yacc.c:1646  */
    break;

  case 364:
#line 2477 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_F5;
				}
#line 5669 "csqpy.c" /* yacc.c:1646  */
    break;

  case 365:
#line 2480 "csqpy.y" /* yacc.c:1646  */
    {
				    if ((yyvsp[-1].stack).type!=CINTTYPE)
					FAILSYN;
				    switch((yyvsp[-1].stack).val.intvalue)
				    {
				      case 1:
					(yyval.stack).type=TU_F1;
					break;
				      case 2:
					(yyval.stack).type=TU_F2;
					break;
				      case 3:
					(yyval.stack).type=TU_F3;
					break;
				      case 4:
					(yyval.stack).type=TU_F4;
					break;
				      case 5:
					(yyval.stack).type=TU_F5;
					break;
				      default:
					FAILSYN;
				    }
				}
#line 5698 "csqpy.c" /* yacc.c:1646  */
    break;

  case 366:
#line 2505 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_YEAR;
				}
#line 5706 "csqpy.c" /* yacc.c:1646  */
    break;

  case 367:
#line 2508 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_MONTH;
				}
#line 5714 "csqpy.c" /* yacc.c:1646  */
    break;

  case 368:
#line 2511 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_DAY;
				}
#line 5722 "csqpy.c" /* yacc.c:1646  */
    break;

  case 369:
#line 2514 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_HOUR;
				}
#line 5730 "csqpy.c" /* yacc.c:1646  */
    break;

  case 370:
#line 2517 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_MINUTE;
				}
#line 5738 "csqpy.c" /* yacc.c:1646  */
    break;

  case 371:
#line 2520 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=TU_SECOND;
				}
#line 5746 "csqpy.c" /* yacc.c:1646  */
    break;

  case 378:
#line 2533 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
			    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenexpr;
			    p.type=WHEN;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
#line 5762 "csqpy.c" /* yacc.c:1646  */
    break;

  case 379:
#line 2543 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;
				    parsestack_t p, *pp;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_jump;
				    pp=(parsestack_t *)
					   fgw_stackpop(&pstate->parsestack);
				    pp->instr->skip_to=rxx_setjump(pstate);
				    p.type=THEN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
#line 5781 "csqpy.c" /* yacc.c:1646  */
    break;

  case 380:
#line 2558 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_elseexpr;
				}
#line 5793 "csqpy.c" /* yacc.c:1646  */
    break;

  case 382:
#line 2566 "csqpy.y" /* yacc.c:1646  */
    {
			    instruction_t *i;
			    parsestack_t p;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_whenbool;
			    p.type=WHEN;
			    p.instr=i;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
#line 5809 "csqpy.c" /* yacc.c:1646  */
    break;

  case 383:
#line 2576 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;
				    parsestack_t p, *pp;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_jump;
				    pp=(parsestack_t *)
					    fgw_stackpop(&pstate->parsestack);
				    pp->instr->skip_to=rxx_setjump(pstate);
				    p.type=THEN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
#line 5828 "csqpy.c" /* yacc.c:1646  */
    break;

  case 385:
#line 2592 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}
#line 5841 "csqpy.c" /* yacc.c:1646  */
    break;

  case 386:
#line 2601 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=NULLVALUE;
				}
#line 5849 "csqpy.c" /* yacc.c:1646  */
    break;

  case 387:
#line 2604 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~NULLVALUE;
				}
#line 5857 "csqpy.c" /* yacc.c:1646  */
    break;

  case 390:
#line 2611 "csqpy.y" /* yacc.c:1646  */
    {
			    (yyval.insptr)=rxx_newinstr(pstate);
			    FAILCHECK
			    (yyval.insptr)->func=rxo_listcount;
			    (yyval.insptr)->constant.count=(yyvsp[0].stack).count;
			    (yyval.insptr)->constant.flags=(yyvsp[0].stack).flags;
			}
#line 5869 "csqpy.c" /* yacc.c:1646  */
    break;

  case 391:
#line 2619 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).count=(yyvsp[-2].stack).count+1;
				    (yyval.stack).flags=(yyvsp[-2].stack).flags|(yyvsp[0].stack).flags;
				}
#line 5878 "csqpy.c" /* yacc.c:1646  */
    break;

  case 392:
#line 2623 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).count=1;
				    (yyval.stack).flags|=(yyvsp[0].stack).flags;
				}
#line 5887 "csqpy.c" /* yacc.c:1646  */
    break;

  case 393:
#line 2628 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).flags=0;
				}
#line 5895 "csqpy.c" /* yacc.c:1646  */
    break;

  case 394:
#line 2631 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[-1].fntok)->tl_func;
				    i->constant.flags|=(yyvsp[0].stack).flags;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
#line 5909 "csqpy.c" /* yacc.c:1646  */
    break;

  case 395:
#line 2640 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[-2].instr), sizeof(instruction_t));
				    i->func=rxo_dohashstar;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
#line 5925 "csqpy.c" /* yacc.c:1646  */
    break;

  case 396:
#line 2651 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[-2].instr), sizeof(instruction_t));
				}
#line 5937 "csqpy.c" /* yacc.c:1646  */
    break;

  case 397:
#line 2657 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_varlist;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
#line 5952 "csqpy.c" /* yacc.c:1646  */
    break;

  case 398:
#line 2667 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *ic, *i;

				    ic=rxx_newinstr(pstate);
				    FAILCHECK
				    ic->func=rxo_listcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    switch ((yyvsp[0].stack).type)
				    {
				      case DELIMITED:
					i->func=rxo_delsplit;
					ic->constant.count=3;
					break;
				      case PATTERN:
					i->func=rxo_patsplit;
					ic->constant.count=2;
					break;
				      case ~DELIMITED:
#ifdef HAVE_REGEX
					i->func=rxo_regdelsplit;
					ic->constant.count=2;
#else
					FAIL(RC_NLNKD)
#endif
					break;
				      case EXTRACTOR:
					i->func=rxo_extsplit;
					ic->constant.count=3;
					break;
				    }
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
#line 5990 "csqpy.c" /* yacc.c:1646  */
    break;

  case 399:
#line 2700 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_to;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
#line 6003 "csqpy.c" /* yacc.c:1646  */
    break;

  case 400:
#line 2709 "csqpy.y" /* yacc.c:1646  */
    {
				    if ((yyvsp[0].insptr)->constant.count && (yyvsp[-1].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ((yyvsp[-1].fntok)->tl_parmcount>0)
					(yyvsp[0].insptr)->constant.val.intvalue=(yyvsp[-1].fntok)->tl_parmcount;
				    (yyval.fntok)=(yyvsp[-1].fntok);
				}
#line 6015 "csqpy.c" /* yacc.c:1646  */
    break;

  case 401:
#line 2716 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    (yyval.fntok)=(yyvsp[0].fntok);
				}
#line 6028 "csqpy.c" /* yacc.c:1646  */
    break;

  case 402:
#line 2725 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).flags=0;
				}
#line 6036 "csqpy.c" /* yacc.c:1646  */
    break;

  case 403:
#line 2728 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).flags=EF_FUNCHASH;
				}
#line 6044 "csqpy.c" /* yacc.c:1646  */
    break;

  case 404:
#line 2732 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=IN;
				}
#line 6052 "csqpy.c" /* yacc.c:1646  */
    break;

  case 405:
#line 2735 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~IN;
				}
#line 6060 "csqpy.c" /* yacc.c:1646  */
    break;

  case 406:
#line 2739 "csqpy.y" /* yacc.c:1646  */
    {
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
#line 6075 "csqpy.c" /* yacc.c:1646  */
    break;

  case 407:
#line 2749 "csqpy.y" /* yacc.c:1646  */
    {
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
#line 6090 "csqpy.c" /* yacc.c:1646  */
    break;

  case 408:
#line 2760 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=BETWEEN;
				}
#line 6098 "csqpy.c" /* yacc.c:1646  */
    break;

  case 409:
#line 2763 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~BETWEEN;
				}
#line 6106 "csqpy.c" /* yacc.c:1646  */
    break;

  case 410:
#line 2767 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=MATCHES;
				}
#line 6114 "csqpy.c" /* yacc.c:1646  */
    break;

  case 411:
#line 2770 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~MATCHES;
				}
#line 6122 "csqpy.c" /* yacc.c:1646  */
    break;

  case 412:
#line 2774 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=LIKE;
				}
#line 6130 "csqpy.c" /* yacc.c:1646  */
    break;

  case 413:
#line 2777 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~LIKE;
				}
#line 6138 "csqpy.c" /* yacc.c:1646  */
    break;

  case 414:
#line 2781 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=REGEXP;
				}
#line 6146 "csqpy.c" /* yacc.c:1646  */
    break;

  case 415:
#line 2784 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.stack).type=~REGEXP;
				}
#line 6154 "csqpy.c" /* yacc.c:1646  */
    break;

  case 417:
#line 2789 "csqpy.y" /* yacc.c:1646  */
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    i->constant.type=CSTRINGTYPE;
				    i->constant.flags|=EF_NOTONTSS;
				    i->constant.val.string="\\";
				}
#line 6169 "csqpy.c" /* yacc.c:1646  */
    break;

  case 418:
#line 2806 "csqpy.y" /* yacc.c:1646  */
    {
				    if (yychar==TOK_ERROR)
					YYERROR;
				}
#line 6178 "csqpy.c" /* yacc.c:1646  */
    break;

  case 419:
#line 2813 "csqpy.y" /* yacc.c:1646  */
    {
				    (yyval.tstart)=pstate->tokstart;

/* hack alert - we want the parsing to terminate, but we don't want to accept yet
   hence we fool the parser into thinking that the input has ended */
				    yychar=0;
				    DRAIN_STMT;
				}
#line 6191 "csqpy.c" /* yacc.c:1646  */
    break;


#line 6195 "csqpy.c" /* yacc.c:1646  */
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
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

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

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
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
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
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

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
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
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 2821 "csqpy.y" /* yacc.c:1906  */


/*
** csqpc interface
*/
static void init_sqp();

/*
** the parser
*/
int sql_parser(parserstate_t *state)
{
    int r;
    parserstate_t *save_pstate;
    int (*save_thischar)();

    save_pstate=pstate;
    save_thischar=thischar;
    pstate=state;
#ifdef PUREPARSER
    if (!pstate->parseroptions)
	thischar=expthischar_sql;
    else
#endif
	thischar=thischar_sql;
    if (pstate->parseroptions)
	pstate->parseroptions|=EXPRHACK;
    else
  	pstate->parseroptions=STMTHACK|LASTHACK;
    if (pstate->errorinfo)
    {
	pstate->errorinfo->line_count=0;
	pstate->errorinfo->near[0]='\0';
	pstate->errorinfo->extra[0]='\0';
    }
    pstate->prevtokstart=-1;
    pstate->ibufp=pstate->phase1.stmt_start;
    pstate->prevtoken=-1;
    pstate->prevlval=malloc(sizeof(YYSTYPE));
    pstate->svyylval=malloc(sizeof(YYSTYPE));
 
    pstate->stmtcnt=0;
    memset(&pstate->stmtstack, 0, sizeof(pstate->stmtstack));
    pstate->stmtstack[0].sqlstart=pstate->ibufp;
    pstate->ssp=(stmtstack_t *) &pstate->stmtstack;
    pstate->curstmt=NULL;

    pstate->instrp=NULL;
    pstate->runneroptions=0;
    init_sqp();
    status=0;
    rxx_emptypcode(pstate);
    yyparse();

/*
** clear error if WHENEVER ERROR CONTINUE,
** unless it's a syntax error
*/
    if ((pstate->flags & PF_ERRORCONT) && RC_ERRSKIP(status))
	status=0;
/*
** stored procedures and outer layers require buffer untouched
** restore whatever damage we have done
*/
    if (pstate->stmtstack[0].sqlsv)
	*TOKPTR(pstate->stmtstack[0].sqlend)=pstate->stmtstack[0].sqlsv;
    if (pstate->stmtstack[1].sqlsv)
	*TOKPTR(pstate->stmtstack[1].sqlend)=pstate->stmtstack[1].sqlsv;
    free(pstate->prevlval);
    free(pstate->svyylval);

bad:
/*
** free string resources
*/
    fgw_tssdrop(&(pstate->lexhead));
    pstate=save_pstate;
    thischar=save_thischar;
/*
** status cleared defensively not to affect 4gl code
*/
    r=status;
    status=0;
    return r;
}

/*
** instantiates internal parser resources
*/
void sql_parserinit(parserstate_t *state)
{
    fgw_stackinit(&state->parsestack, sizeof(parsestack_t));
}

/*
** frees internal parser resources
*/
void sql_parserfree(parserstate_t *state)
{
    fgw_tssdrop(&state->lexhead);
    fgw_stackfree(&state->parsestack, NULL);
    rxx_droppcode(&state->pcode);
}

/*
** saves parser state for recursive call
*/

void sql_parsersave(parserstate_t *state)
{
    state->svyychar=yychar;
    *((YYSTYPE *) state->svyylval)=yylval;
}

/*
** restores parser state after recursive call
*/
void sql_parserrestore(parserstate_t *state)
{
    yychar=state->svyychar;
    yylval=*((YYSTYPE *) state->svyylval);
}

/*
** returns delimident flag
*/
int fgw_delimident()
{
    return delimident;
}

/*
** statement related ops
**
** mark the end of an sql segment
*/
static void markend_sql(int c)
{
    if (!pstate->ssp->sqlend || c<pstate->ssp->sqlend)
	pstate->ssp->sqlend=c;
}

/*
** temporarily end sql segment for execution
*/
static void endstmt_sql()
{
    if (pstate->ssp->sqlend)
    {
	pstate->ssp->sqlsv=*TOKPTR(pstate->ssp->sqlend);
	*TOKPTR(pstate->ssp->sqlend)='\0';
    }
}

/*
** scanner & parser hacks
**
** reserved words list
*/
static tokenlist_t tokenlist[]=
{
    "quit", NULL, QUIT, 0, 0, NULL,

    "for", NULL, FOR, 0, 0, NULL,
    "foreach", NULL, FOREACH, 0, 0, NULL,
    "while", NULL, WHILE, 0, 0, NULL,
    "clone", NULL, CLONE, 0, 0, NULL,
    "parent", NULL, PARENT, 0, 0, NULL,
    "if", NULL, IF, 0, 0, NULL,
    "elif", NULL, ELIF, 0, 0, NULL,
    "else", NULL, ELSE, 0, 0, NULL,
    "then", NULL, THEN, 0, 0, NULL,
    "fi", NULL, FI, 0, 0, NULL,
    "done", NULL, DONE, 0, 0, NULL,
    "do", NULL, DO, 0, 0, NULL,
    "continue", NULL, CONTINUE, 0, 0, NULL,
    "break", NULL, BREAK, 0, 0, NULL,
    "exit", NULL, EXIT, 0, 0, NULL,

    "display", NULL, DISPLAY, 0, 0, NULL,
    "let", NULL, LET, 0, 0, NULL,
    "dispose", NULL, DISPOSE, 0, 0, NULL,
    "invoke", NULL, INVOKE, 0, 0, NULL,
    "store", NULL, STORE, 0, 0, NULL,

    "pipe", NULL, PIPE, 0, 0, NULL,
    "append", NULL, APPEND, 0, 0, NULL,
    "output", NULL, OUTPUT, 0, 0, NULL,
    "write", NULL, WRITE, 0, 0, NULL,
    "read", NULL, READ, 0, 0, NULL,
    "input", NULL, INPUT, 0, 0, NULL,
    "reject", NULL, REJECT, 0, 0, NULL,
    "blocksize", NULL, BLOCKSIZE, 0, 0, NULL,

    "whenever", NULL, WHENEVER, 0, 0, NULL,
    "load", NULL, LOAD, 0, 0, NULL,
    "unload", NULL, UNLOAD, 0, 0, NULL,
    "select", NULL, SELECT, 0, 0, NULL,

    /* this is a bit naughty - but we can use upserts
       where inserts are used, and the grammar doesn't
       even need to know about it...
     */
    "upsert", NULL, INSERT, 0, 0, NULL,
    "insert", NULL, INSERT, 0, 0, NULL,
    "delete", NULL, DELETE, 0, 0, NULL,
    "update", NULL, UPDATE, 0, 0, NULL,
    "execute", NULL, EXECUTE, 0, 0, NULL,
    "call", NULL, CALL, 0, 0, NULL,
    "create", NULL, CREATE, 0, 0, NULL,
    "end", NULL, END, 0, 0, NULL,
    "prepare", NULL, PREPARE, 0, 0, NULL,
    "free", NULL, FREE, 0, 0, NULL,

    "connect", NULL, CONNECT, 0, 0, NULL,
    "disconnect", NULL, DISCONNECT, 0, 0, NULL,
    "set", NULL, S_SET, 0, 0, NULL,
    "connection", NULL, CONNECTION, 0, 0, NULL,

    "begin", NULL, BEGIN, 0, 0, NULL,
    "immediate", NULL, IMMEDIATE, 0, 0, NULL,
    "compound", NULL, COMPOUND, 0, 0, NULL,

    "error", NULL, ERROR, 0, 0, NULL,
    "log", NULL, LOG, 0, 0, NULL,
    "stop", NULL, STOP, 0, 0, NULL,
    "procedure", NULL, PROCEDURE, 0, 0, NULL,
    "function", NULL, FUNCTION, 0, 0, NULL,
    "cursor", NULL, CURSOR, 0, 0, NULL,
    "with", NULL, WITH, 0, 0, NULL,
    "hold", NULL, HOLD, 0, 0, NULL,
    "as", NULL, AS, 0, 0, NULL,
    "source", NULL, SOURCE, 0, 0, NULL,
    "default", NULL, DEFAULT, 0, 0, NULL,

    "plain", NULL, PLAIN, 0, 0, NULL,
    "html", NULL, HTML, 0, 0, NULL,
    "width", NULL, WIDTH, 0, 0, NULL,

    "returning", NULL, RETURNING, 0, 0, NULL,

    "in", NULL, IN, 0, 0, NULL,
    "aggregate", NULL, AGGREGATE, 0, 0, NULL,
    "autocommit", NULL, AUTOCOMMIT, 0, 0, NULL,
    "into", NULL, INTO, 0, 0, NULL,
    "using", NULL, USING, 0, 0, NULL,
    "format", NULL, FORMAT, 0, 0, NULL,
    "brief", NULL, BRIEF, 0, 0, NULL,
    "vertical", NULL, VERTICAL, 0, 0, NULL,
    "full", NULL, FULL, 0, 0, NULL,
    "delimited", NULL, DELIMITED, 0, 0, NULL,
    "regexp", NULL, REGEXP, 0, 0, NULL,
    "headers", NULL, HEADERS, 0, 0, NULL,
    "pattern", NULL, PATTERN, 0, 0, NULL,
    "extractor", NULL, EXTRACTOR, 0, 0, NULL,
    "from", NULL, FROM, 0, 0, NULL,
    "to", NULL, TO, 0, 0, NULL,
    "by", NULL, BY, 0, 0, NULL,
    "delimiter", NULL, DELIMITER, 0, 0, NULL,

    "temp", NULL, TEMP, 0, 0, NULL,
    "scratch", NULL, SCRATCH, 0, 0, NULL,
    "external", NULL, EXTERNAL, 0, 0, NULL,

    "cast", NULL, KWCAST, 0, 0, NULL,
    "integer", NULL, TYPEINT, 0, 0, NULL,
    "int", NULL, TYPEINT, 0, 0, NULL,
    "float", NULL, TYPEFLOAT, 0, 0, NULL,
    "decimal", NULL, TYPEDEC, 0, 0, NULL,
    "dec", NULL, TYPEDEC, 0, 0, NULL,
    "money", NULL, TYPEMONEY, 0, 0, NULL,
    "string", NULL, TYPESTRING, 0, 0, NULL,
    "byte", NULL, TYPEBYTE, 0, 0, NULL,

    "split", NULL, SPLIT, 0, 0, NULL,

    "and", NULL, AND, 0, 0, NULL,
    "or", NULL, OR, 0, 0, NULL,
    "not", NULL, NOT, 0, 0, NULL,
    "is", NULL, IS, 0, 0, NULL,
    "case", NULL, CASE, 0, 0, NULL,
    "when", NULL, WHEN, 0, 0, NULL,
    "then", NULL, THEN, 0, 0, NULL,
    "null", NULL, NULLVALUE, 0, 0, NULL,
    "between", NULL, BETWEEN, 0, 0, NULL,
    "matches", NULL, MATCHES, 0, 0, NULL,
    "like", NULL, LIKE, 0, 0, NULL,
    "escape", NULL, ESCAPE, 0, 0, NULL,
    "units", NULL, UNITS, 0, 0, NULL,
    "hour", NULL, HOUR, 0, 0, NULL,
    "minute", NULL, MINUTE, 0, 0, NULL,
    "second", NULL, SECOND, 0, 0, NULL,
    "fraction", NULL, FRACTION, 0, 0, NULL,

    "column", NULL, COLUMN, 0, 0, NULL,
    "columns", rxf_columns, FNCV, 0, 0, NULL,
    "count", NULL, COUNT, 0, 0, NULL,

    "displaylabels", rxf_displaylabels, FNCV, 0, 0, NULL,
    "displayformat", rxf_displayformat, FNCT, 0, -1, NULL,

    "filetotext", rxf_filetotext, FNCT, 0, 1, NULL,
    "filetoblob", rxf_filetoblob, FNCT, 0, 1, NULL,

    "ascii", rxf_ascii, FNCT, 0, 1, NULL,
    "clipped", rxf_clipped, PFIXFNCT, 0, 1, NULL,
    "spaces", rxf_spaces, PFIXFNCT, 0, 1, NULL,
    "picture", NULL, PICTURE, 0, 0, NULL,

    "sleep", rxf_sleep, FNCS, 0, 1, NULL,
    "system", rxf_run, FNCS, 0, 1, NULL,
    "run", rxf_run, FNCS, 0, 1, NULL,
    "spawn", rxf_spawn, FNCS, 0, 1, NULL,
    "exec", rxf_exec, FNCS, 0, 1, NULL,
    "waitpid", rxf_waitpid, FNCS, 0, 1, NULL,
/*    "wait", rxf_wait, FNCS, 0, 0, NULL, */
    "wait", NULL, WAIT, 0, 0, NULL,
    "fork", rxf_fork, FNCT, 0, 0, NULL,
    "fopen", rxf_fopen, FNCT, 0, 2, NULL,
    "popen", rxf_popen, FNCT, 0, 2, NULL,
    "sclose", rxf_sclose, FNCT, 0, 1, NULL,
    "getenv", rxf_getenv, FNCT, 0, 1, NULL,
    "eval", rxf_eval, FNCT, 0, 1, NULL,

    "index", rxf_index, FNCT, 0, 2, NULL,
    "match", rxf_match, FNCT, 0, 2, NULL,
    "pad", rxf_pad, FNCT, 0, 2, NULL,
    "random", rxf_random, FNCT, 0, -1, NULL,

    "current", rxf_current, FNDN, 0, 0, NULL,
    "today", rxf_today, FNCT, 0, 0, NULL,
    "user", NULL, USER, 0, 0, NULL,		/* damn connect statement */
/*    "user", rxf_user, FCNT, 0, 0, NULL, */

/* dbservername */
/* sitename */

    "decode", rxf_decode, FNCT, 0, -1, NULL,
    "nvl", rxf_nvl, FNCT, 0, 2, NULL,

    "abs", rxf_abs, FNCT, 0, 1, NULL,
    "mod", rxf_mod, FNCT, 0, 2, NULL,
    "pow", rxf_pow, FNCT, 0, 2, NULL,
    "root", rxf_root, FNCT, 0, 2, NULL,
    "round", rxf_round, FNCT, 0, -1, NULL,
    "sqrt", rxf_sqrt, FNCT, 0, 1, NULL,
    "trunc", rxf_trunc, FNCT, 0, -1, NULL,

    "dbinfo", rxf_dbinfo, FNCT, 0, 1, NULL,
    "sqlcode", rxf_sqlcode, FNCT, 0, 0, NULL,
    "sqlerrd1", rxf_sqlerrd1, FNCT, 0, 0, NULL,
    "sqlerrd2", rxf_sqlerrd2, FNCT, 0, 0, NULL,
    "errno", rxf_errno, FNCT, 0, 0, NULL,

    "exp", rxf_exp, FNCT, 0, 1, NULL,
    "log10", rxf_log10, FNCT, 0, 1, NULL,
    "logn", rxf_logn, FNCT, 0, 1, NULL,
    "hex", rxf_hex, FNCT, 0, 1, NULL,

    "char_length", rxf_charlength, FNCT, 0, 1, NULL,
    "character_length", rxf_charlength, FNCT, 0, 1, NULL,
    "length", rxf_length, FNCT, 0, 1, NULL,
/* octet_length e */

#ifdef HAS_DTFMT
    "timestamp", rxf_datetime, FNDP, TYPEDTIME, -1, NULL,
    "datetime", rxf_datetime, FNDP, TYPEDTIME, -1, NULL,
    "duration", rxf_interval, FNDP, TYPEINTVL, -1, NULL,
    "interval", rxf_interval, FNDP, TYPEINTVL, -1, NULL,
#else
    "timestamp", rxf_datetime, FNDP, TYPEDTIME, 1, NULL,
    "datetime", rxf_datetime, FNDP, TYPEDTIME, 1, NULL,
    "duration", rxf_interval, FNDP, TYPEINTVL, 1, NULL,
    "interval", rxf_interval, FNDP, TYPEINTVL, 1, NULL,
#endif
    "date", rxf_date, FNCT, TYPEDATE, -1, NULL,
    "day", rxf_day, FNCT, DAY, 1, NULL,
    "month", rxf_month, FNCT, MONTH, 1, NULL,
    "year", rxf_year, FNCT, YEAR, 1, NULL,
    "weekday", rxf_weekday, FNCT, 0, 1, NULL,
    "mdy", rxf_mdy, FNCT, 0, 3, NULL,
/* extend e / f to l */
/* to_char e /fmt */
/* to_date e /fmt */

    "cos", rxf_cos, FNCT, 0, 1, NULL,
    "sin", rxf_sin, FNCT, 0, 1, NULL,
    "tan", rxf_tan, FNCT, 0, 1, NULL,
    "acos", rxf_acos, FNCT, 0, 1, NULL,
    "asin", rxf_asin, FNCT, 0, 1, NULL,
    "atan", rxf_atan, FNCT, 0, 1, NULL,
    "atan2", rxf_atan2, FNCT, 0, 2, NULL,

/* trim leading/trailing/both e from e */
/* substring e from start for length */
    "substr", rxf_substr, FNCT, 0, 3, NULL,
    "replace", rxf_replace, FNCT, 0, -1, NULL,
    "lpad", rxf_lpad, FNCT, 0, -1, NULL,
    "rpad", rxf_rpad, FNCT, 0, -1, NULL,

    "lower", rxf_lower, FNCT, 0, 1, NULL,
    "upper", rxf_upper, FNCT, 0, 1, NULL,
    "initcap", rxf_initcap, FNCT, 0, 1, NULL
};

#define ERRCONTEXTLEN	10
/*
** error handler
*/
static void yyerror(char *s)
{
    int i, eoi=(!pstate->tokstart) || *TOKPTR(pstate->tokstart)==';';
    char *n;

/*
** set error if not alread set
*/
    if (!status)
	status=RC_PSNTX;
/*
** copy error context
*/
    if (pstate->errorinfo && SQ_ISSYNTAX(SQ_TOSRC(status)))
    {
/*
** first copy portions of token prior to the error
*/
	if (pstate->prevtokstart>=0)
	{
	    if (pstate->prevtoklen<=ERRCONTEXTLEN)
		strncat((char *) &pstate->errorinfo->near,
			 TOKPTR(pstate->prevtokstart),
			 pstate->prevtoklen);
	    else
	    {
		char *s=TOKPTR(pstate->prevtokstart)+
			pstate->prevtoklen-ERRCONTEXTLEN;

		strcat((char *) &pstate->errorinfo->near, "...");
		strncat((char *) &pstate->errorinfo->near, s, ERRCONTEXTLEN);
	    }
	}
/*
** then portions of token resulting in the error
*/
	strcat((char *) &pstate->errorinfo->near, " >>");
	if (!eoi)
	{
	    if (pstate->toklen<=ERRCONTEXTLEN)
		strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), pstate->toklen);
	    else
	    {
	        strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), ERRCONTEXTLEN);
	         strcat((char *) &pstate->errorinfo->near, "...");
	    }
	}
	strcat((char *) &pstate->errorinfo->near, "<< ");
	if (*TOKPTR(pstate->tokstart) && eoi)
	    strncat((char *) &pstate->errorinfo->near,
			TOKPTR(pstate->tokstart), pstate->toklen);
/*
** clear newlines and tabs from 'near' text
*/
	for (n=(char *) &pstate->errorinfo->near; *n; n++)
	    if (*n=='\n' || *n=='\t')
		*n=' ';
    }
/*
** free resources
*/
    for (i=0; i<=pstate->stmtcnt-1; i++)
	if (pstate->stmtstack[i].stmt)
	{
	    if (pstate->stmtstack[i].stmt->st_entry.signature & SS_INTERNAL)
		rqx_closestatement(pstate->stmtstack[i].stmt);
	    else
		rqx_freestatement(pstate->stmtstack[i].stmt);
	    pstate->stmtstack[i].stmt=NULL;
	}
#ifdef PUREPARSER
    draintokens_sql();
#endif
}

#define PD_ESCAPE	1
#define PD_IDENT	2

typedef struct
{
    char *dirname;
    int dirvalue;
} dirlist_t;

dirlist_t dirlist[] =
{
    "escape", PD_ESCAPE,
    "esc", PD_ESCAPE,
    "ignore", PD_ESCAPE,
    "ign", PD_ESCAPE,
    "ident", PD_IDENT,
    NULL, 0
};

/*
** parser directives
*/
static int getdir_sql(char *s, char *e)
{
    dirlist_t *d;
    char *c;
    int l;

/*
** scan directives
*/
    for (d=dirlist; d->dirname; d++)
    {
	c=s;
/*
** skip initial spaces
*/
	while (*c==' ' || *c=='\n' || *c=='\t')
	  c++;
/*
** compare directive name
*/
	l=strlen(d->dirname);
	if (e-c+1<l)
	   continue;
	if (strncmp(c, d->dirname, l))
	    continue;
	c+=l;
/*
** skip final spaces and make sure there isn't anything else
*/
	for (;;)
	    if (c>e)
		return d-> dirvalue;
	    else if (*c==' ' || *c=='\n' || *c=='\t')
		c++;
	    else
		break;
    }
}

#ifdef PUREPARSER
/*
** early YYACCEPT: purge remaining statement tokens
*/
static void draintokens_sql()
{
    int c;
    int old_status=status;

    status=0;
    if ((c=*TOKPTR(pstate->tokstart))==';' || c=='\0')
	goto end;
    for (;;)
	switch (scanner_sql())
	{
	  case VAR:
	    fgw_tssfree(&pstate->lexhead, yylval.var);
	    break;
	  case CNST:
	    if (yylval.stack.type==CSTRINGTYPE)
		fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
	    break;
	  case TOK_ERROR:
	  case ';':
	  case 0:
	    pstate->phase1.next_stmt_start=pstate->ibufp;
	    goto end;
	}
end:
    status=old_status;
}

/*
** pure parser: fill buffers and fire expansions
*/
static int fillbuffer_sql()
{
    int r, ip, s, e, l, nlc;
    char *txt, *tp;
    controlstack_t *c;

/*
** in case of consecutive comments, no character could make it to
** the buffer, so have to try repeatedly
*/
   for (;;)
   {
/*
** still characters to go in the expansion buffer
*/
	if (pstate->phase1.output_buffer_len>pstate->ibufp)
	    return 0;

/*
** reached the end of the script
*/
	if (pstate->o_size && pstate->phase1.input_char==pstate->i_size)
	    return 0;
	txt=NULL;
/*
** fire expansion
*/
	pstate->svyychar=yychar;
	*((YYSTYPE *) pstate->svyylval)=yylval;
	pstate->phase1.expstart=-1;
	r=fgw_sqlexplode(pstate->i_query,
			 pstate->i_size, pstate->vars,
			 (pstate->flags & PF_VERBOSE),
			 ((!pstate->control.count ||
			   (c=(controlstack_t *)
				fgw_stackpeek(&pstate->control),
				 c->state>S_DISABLED))?
			  (pstate->expstate): ES_DISABLED),
			 pstate->phase1.input_char+1, &ip, &txt,
			 &s, &e, &nlc);
	yychar=pstate->svyychar;
	yylval=*((YYSTYPE *) pstate->svyylval);
/*
** error in expansion? abort
*/
	if (r!=0)
	{
	    if (pstate->errorinfo)
		pstate->errorinfo->line_count=pstate->phase1.line_count+nlc+1;
/* FIXME - handle non expansion facility errors better */
	    if (txt)
		free(txt);
	    return r;
	}
/*
** resize output buffer
*/
	if (pstate->o_size<
	    (s<0? 0: e-s)+
	    (txt? strlen(txt): 0)+2+nlc)
	{
	    char *newbuf;

	    newbuf=realloc(pstate->o_query, pstate->o_size+BUFSIZE);
	    if (!newbuf)
	    {
		if (txt)
		    free(txt);
		return RC_NOMEM;
	    }
	    pstate->o_query=newbuf;
	    pstate->o_size+=BUFSIZE;
	}
/*
** finally, copy strings
*/
	if (s>=0 && (l=e-s+1))
	{
	    fgw_move(pstate->o_query+pstate->phase1.output_buffer_len,
		    pstate->i_query+s-1, l);
	    pstate->phase1.output_buffer_len+=l;
	    *(pstate->o_query+pstate->phase1.output_buffer_len)='\0';
	}
	if (txt)
	{
	    pstate->phase1.expstart=pstate->phase1.output_buffer_len;
	    if (pstate->expcount==pstate->phase1.expcount)
		pstate->phase1.prev_input_char=pstate->phase1.input_char;
	    l=strlen(txt);
	    strcpy(pstate->o_query+pstate->phase1.output_buffer_len, txt);
	    pstate->phase1.output_buffer_len+=l;
	    pstate->phase1.expcount++;
	    free(txt);
	}
/*
** add new lines to the end of the expansion to keep track
** of newlines in expansion text
*/
	if (nlc)
	{
	     tp=pstate->o_query+pstate->phase1.output_buffer_len;
	     pstate->phase1.output_buffer_len+=nlc;
	     while (nlc--)
		*tp++='\n';
	     *tp='\0';
	}
/*
** set next expansion search start
*/
	if (ip<0)
	    pstate->phase1.input_char=pstate->i_size;
	else
	    pstate->phase1.input_char=ip-1;
    }
    return 0;
}

/*
** advance scanner current char - expansion handling version
*/
static int expthischar_sql()
{
    int r;

    if (r=fillbuffer_sql())
    {
	status=r;
	return -1;
    }
    return *TOKPTR(pstate->ibufp);
}
#endif 

/*
** advance scanner current char - non reentrant parser version
*/
static int thischar_sql()
{
    return *TOKPTR(pstate->ibufp);
}

/*
** scanner
*/
static int scanner_sql()
{
    int c, c1;
    int r;
    tokenlist_t *t;
    int newdir=0, dir;

    memset(&yylval.stack, 0, sizeof(yylval.stack));

/* if we ever want to revert to true datetime/interval constant syntax,
   here's the place to

    if (pstate->parseroptions & DTHACK)
    {
	pstate->parseroptions&=~DTHACK;
	while (c=*TOKPTR(pstate->ibufp)!=')')
...etc
    }
    else
*/
    while ((c=(thischar)())>0)
    {
	pstate->ibufp++;
/*
** skip spaces, tabs & newlines
*/
	if (c==' ' || c=='\t')
	    continue;
	if (c=='\n')
	{
	    pstate->phase1.line_count++;
	    continue;
	}
	if (pstate->errorinfo && !pstate->errorinfo->line_count)
	    pstate->errorinfo->line_count=pstate->phase1.line_count+1;
	pstate->prevtokstart=pstate->tokstart;
	pstate->prevtoklen=pstate->toklen;
	pstate->tokstart=pstate->ibufp-1;
	pstate->toklen=0;
	dir=newdir;
	newdir=0;
/*
** looks like an hex or number
*/
	if (c=='0')
	{
/*
** we return TOK_ERROR to trigger a YYERROR if there's an expansion error
*/
	    if ((c1=(thischar)())<0)
		return TOK_ERROR;
	    if (c1=='x' || c1=='X')
	    {
		pstate->ibufp++;
		yylval.stack.type=CINTTYPE;
		if ((yylval.stack.val.intvalue=getdigits_sql(16))<0)
		    return 0;
		return CNST;
	    }
	    else if (c1>'0' && c1<'8')
	    {
		yylval.stack.type=CINTTYPE;
		if ((yylval.stack.val.intvalue=getdigits_sql(8))<0)
		    return 0;
		return CNST;
	    }
	}
/*
** integer or float
*/
	if (c>='0' && c<='9')
	{
	    int tmp;

	    pstate->ibufp--;
	    if ((tmp=getdigits_sql(10))<0)
		return TOK_ERROR;

	    /* FIXME: GLS */
	    if ((c=(thischar)())<0)
		return TOK_ERROR;
	    if (c=='.')
	    {
		pstate->ibufp++;
		yylval.stack.type=CDOUBLETYPE;
		if (getfloat_sql(tmp, &yylval.stack.val.real))
		    return TOK_ERROR;
	    }
	    else
	    {
		yylval.stack.type=CINTTYPE;
		yylval.stack.val.intvalue=tmp;
	    }
	    return CNST;
	}
/*
** variable, function or keyword
*/
	else if ((c>='A' && c<='Z') || (c>='a' &&c<='z'))
	{
	    if (!(yylval.var=getvar_sql(&(pstate->lexhead))))
		return 0;
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    t=&tokenlist[fgw_hash(yylval.var, NUMTOKENS)];
	    while (t)
	    {
		if (strcmp(t->tl_name, yylval.var))
		    t=t->tl_next;
		else
		{
		    /* escaped identifier */
		    if (dir==PD_ESCAPE)
			break;
		    fgw_tssfree(&pstate->lexhead, yylval.var);
		    if (t->tl_alttype && (pstate->parseroptions & FUNCHACK))
		    {
			yylval.tstart=pstate->tokstart;
			return t->tl_alttype;
		    }
		    else if (t->tl_func)
		    {
			yylval.fntok=t;
			return t->tl_type;
		    }
		    else
		    {
			yylval.tstart=pstate->tokstart;
			return t->tl_type;
		    }
		}
	    }
	    return VAR;
	}
/*
** delimited identifiers
*/
	else if (c=='\"' && (delimident || (dir==PD_IDENT)))
	{
	    yylval.var=getstring_sql(&(pstate->lexhead));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (yylval.var)
		return VAR;
	    else
		return TOK_ERROR;
	}
/*
** strings
*/
	else if (c=='\'' || c=='\"')
	{
	    yylval.stack.type=CSTRINGTYPE;
	    yylval.stack.val.string=getstring_sql(&(pstate->lexhead));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (yylval.stack.val.string)
		return CNST;
	    else
		return TOK_ERROR;
	}
/*
** operators
*/
	if ((c1=(thischar)())<0)
	    return TOK_ERROR;
	if (c=='<' && c1=='>')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return NEQ;
	}
	else if ((r=GEQ, c=='>') || (r=LEQ, c=='<') ||
		 (r=EQ, c=='=') || (r=NEQ, c=='!'))
	{
	    if (c1=='=')
	    {
		pstate->ibufp++;
		pstate->toklen=2;
	    	return r;
	    }
	}
	else if (c=='|' && c1=='|')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return DPIPE;
	}
	else if (c==':' && c1==':')
	{
	    pstate->ibufp++;
	    pstate->toklen=2;
	    return OPCAST;
	}
/*
** comments
*/
	else if (c=='{')
	{
	    char *cs=TOKPTR(pstate->ibufp);

	    c=(thischar)();
	    while (c && c!='}')
	    {
		if (c<0)
		    return TOK_ERROR;
		if (c=='\n')
		    pstate->phase1.line_count++;
	 	pstate->ibufp++;
		c=(thischar)();
	    }
	    newdir=getdir_sql(cs, TOKPTR(pstate->ibufp-1));
	    pstate->toklen=pstate->ibufp-pstate->tokstart;
	    if (!c)
	    {
		status=RC_BADCMT;
		return 0;
	    }
	    pstate->ibufp++;
	    continue;
	}
	else if (c=='-')
	{
	    if ((c1=(thischar)())<0)
		return TOK_ERROR;
	    if (c1=='-')
	    {
		char *cs=TOKPTR(++pstate->ibufp); 

		c=(thischar)();
		while (c && c!='\n')
		{
		    if (c<0)
			return TOK_ERROR;
		    pstate->ibufp++;
		    c=(thischar)();
		}

		newdir=getdir_sql(cs, TOKPTR(pstate->ibufp-1));
		pstate->toklen=pstate->ibufp-pstate->tokstart;

		/* last line, was a comment bail out */
		if (!c)
		    break;
		pstate->ibufp++;
		pstate->phase1.line_count++;
		continue;
	    }
	}
/*
** everything else
*/
	pstate->toklen=1;
/*
** in order to avoid sql injection from the expansion facility,
** we can disallow statement terminators coming from the expansion
*/
	if (c==';' && (pstate->flags & PF_INJECTION) &&
	    pstate->phase1.expstart>=0 &&
	    pstate->ibufp>pstate->phase1.expstart)
	{
	    status=RC_EXPINJ;
	    return TOK_ERROR;
	}
	return c;
    }
    pstate->prevtokstart=pstate->tokstart;
    pstate->prevtoklen=pstate->toklen;
    pstate->tokstart=pstate->ibufp;
    pstate->toklen=0;
    pstate->ibufp++;
    return (status? TOK_ERROR: 0);
}

/*
** parser hacks
*/
static int yylex()
{
    YYSTYPE swlval;
    int t;

    if (pstate->prevtoken>=0)
    {
	t=pstate->prevtoken;
	pstate->prevtoken=-1;
	yylval=*((YYSTYPE *) pstate->prevlval);
	return t;
    }
    else if (pstate->parseroptions & EXPRHACK)
    {
	pstate->parseroptions&=~EXPRHACK;
	if (pstate->parseroptions & EXPRONLY)
	    return XPRTOK;
	if (pstate->parseroptions & EXPRLIST)
	    return LSTTOK;
	else if (pstate->parseroptions & VARONLY)
	    return VARTOK;
	else
	    return VASTOK;
    }
    else if (pstate->parseroptions & (DMLHACK | SELECTHACK | SQLHACK))
    {
	for(;;)
	{
	    t=scanner_sql();
	    switch (t)
	    {
/*
** we could actually postpone cleaning up those to when lexhead is dropped
** we do it here to avoid allocating needlessly memory
** FIXME: we could get scanner_sql to avoid allocating memory in the first place
*/
	      case VAR:
/*
** we try to capture the VALUES clause of an INSERT statement for the benefit
** of the redirection clause
*/
		if ((pstate->parseroptions & VALUESHACK) &&
		    !strcmp(yylval.var, "values"))
		    pstate->parseroptions&=~VALUESHACK;
		fgw_tssfree(&pstate->lexhead, yylval.var);
		break;
	      case CNST:
		if (yylval.stack.type==CSTRINGTYPE)
		    fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
		break;
	      case '$':
	      case '%':
	      case '?':
		if (pstate->ssp && pstate->ssp->stmt)
		    pstate->ssp->stmt->phcount++;
		break;
	      case INTO:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;
		swlval=yylval;
		t=scanner_sql();
		if (t==VAR)
		{
		    pstate->parseroptions&=~(DMLHACK | SELECTHACK);
		    pstate->prevtoken=t;
		    *((YYSTYPE *) pstate->prevlval)=yylval;
		    yylval=swlval;
		    return INTO;
		}
		if (t==CNST && yylval.stack.type==CSTRINGTYPE)
		    fgw_tssfree(&pstate->lexhead, yylval.stack.val.string);
		break;
	      case INSERT:
	      case EXECUTE:
	      case CALL:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;
		pstate->parseroptions=(pstate->parseroptions | STMTHACK) &
			~SELECTHACK;
		goto redirhack;
	      case DO:
		if (!(pstate->parseroptions & FOREACHHACK))
		    break;
	      case AGGREGATE:
	      case FORMAT:
		if (!(pstate->parseroptions & SELECTHACK))
		    break;

		/* if not using the using clause in dml, remove SQLHACK for USING */
	      case USING:
	      case CONNECTION:
		pstate->parseroptions&=~(DMLHACK | SELECTHACK | SQLHACK);
		return t;
	      case ';':
		markend_sql(pstate->ibufp-1);
		t=0;
	      case 0:
		if (pstate->parseroptions & LASTHACK)
		{
		    pstate->prevtoken=t;
		    t=LASTOK;
		}
#ifdef PUREPARSER
		pstate->phase1.next_stmt_start=pstate->ibufp;
#endif
	      case TOK_ERROR:
/*
** syntax error
*/
		return t;
	    }
	}
    }
    t=scanner_sql();
redirhack:
    if (pstate->parseroptions & (STMTHACK | FOREACHHACK))
    {
	swlval=yylval;
	switch (t)
	{
/*
** differentiate between EXECUTE FUNCTION/PROCEDURE & EXECUTE
*/
	  case EXECUTE:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken==FUNCTION || pstate->prevtoken==PROCEDURE)
	    {
		pstate->prevtoken=SQLTOK;
		pstate->parseroptions|=SELECTHACK;
	    }
	    else
		*((YYSTYPE *) pstate->prevlval)=yylval;
	    pstate->parseroptions&=~(STMTHACK|FOREACHHACK);
	    break;
	}
	yylval=swlval;
    }
    if (pstate->parseroptions & STMTHACK)
    {
	swlval=yylval;
	switch (t)
	{
	  case VAR:
	    t=SQLTOK;
	    break;
/*
** avoid one R/R conflict between create proc and create something else
** VAR or CNST removed from tss at stmt end
*/
	  case CREATE:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=FUNCTION && pstate->prevtoken!=PROCEDURE)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** same for set connection & set something else
*/
	  case S_SET:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=CONNECTION)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** same for begin immediate/compound & begin work
*/
	  case BEGIN:
	    pstate->prevtoken=scanner_sql();
	    if (pstate->prevtoken!=IMMEDIATE && pstate->prevtoken!=COMPOUND)
	    {
		t=SQLTOK;
		pstate->prevtoken=-1;
	    }
	    break;
/*
** avoid shift reduce conflicts for END IMMEDIATE & COMPOUND vs
** END PROCEDURE & FUNCTION vs END IF & FOR & FOREACH & WHILE (keeps grammar
** simpler)
*/
	  case END:
	    switch (pstate->prevtoken=scanner_sql())
	    {
	      case IMMEDIATE:
	      case COMPOUND:
		t=S_END;
		break;
	      case IF:
	      case FOR:
	      case FOREACH:
	      case WHILE:
	      case CLONE:
	      case WAIT:
		t=C_END;
		break;
	    }
	    break;
	}
	yylval=swlval;
	pstate->parseroptions&=~STMTHACK;
    }
    switch (t)
    {
      case INSERT:
	pstate->parseroptions|=VALUESHACK;
      case DELETE:
      case UPDATE:
	pstate->parseroptions|=DMLHACK;
	break;
      case SELECT:
      case CALL:
	pstate->parseroptions|=SELECTHACK;
	break;
      case SQLTOK:
	pstate->parseroptions|=SQLHACK;
	break;
      case FOREACH:
	pstate->parseroptions|=FOREACHHACK;
	break;
      case PREPARE:
	pstate->parseroptions|=PREPAREHACK;
	break;
      case FROM:
	if (pstate->parseroptions & PREPAREHACK)
	    pstate->parseroptions|=STMTHACK;
	pstate->parseroptions&=~PREPAREHACK;
	break;
      case ';':
	if (pstate->parseroptions & (EXPRONLY|EXPRLIST|VARONLY))
	    break;
      case 0:
	if (pstate->parseroptions & LASTHACK)
	{
	    pstate->prevtoken=0;
	    t=LASTOK;
	}
#ifdef PUREPARSER
	pstate->phase1.next_stmt_start=pstate->ibufp;
#endif
    }
    return t;
}

/*
** initiliazes module
*/
static void init_sqp()
{
    tokenlist_t src[NUMTOKENS], temp[NUMTOKENS], *t;
    int i, b, f;
    char *c;
    long h;

    if (delimident>=0)
	return;
    delimident=(getenv("DELIMIDENT")!=NULL);
    fgw_srand(time(NULL));
/*
** initialize token lists
*/
    for (i=0; i<NUMTOKENS; i++)
	src[i]=tokenlist[i];
    memset(&tokenlist, 0, sizeof(tokenlist));
    memset(&temp, 0, sizeof(temp));
    b=0;
    for (i=0; i<NUMTOKENS; i++)
    {
/*
** hash name, determine bucket
*/
	h=fgw_hash(src[i].tl_name, NUMTOKENS);
/*
** oops, bucket already in use
*/
	if (tokenlist[h].tl_name)
	{
	    temp[b]=src[i];
	    temp[b++].tl_next=&tokenlist[h];
	}
	else
	    tokenlist[h]=src[i];
    }
/*
** insert remaining tokens
*/
   f=0;
   for (i=0; i<b; i++)
   {
/*
** scan bucket chain
*/
	t=temp[i].tl_next;
	while (t->tl_next)
	    t=t->tl_next;
/*
** find next free entry
*/
	while (tokenlist[f].tl_name)
	    f++;
	tokenlist[f]=temp[i];
	tokenlist[f].tl_next=NULL;
	t->tl_next=&tokenlist[f];
    }
}

/*
** scans input buffer for variable name
*/
static char *getvar_sql(fgw_tsstype **hd)
{
    char *v, *s, *d;
/*
** this takes into account the fact that we have already scanned
** the first character of the name in scanner_sql()
*/
    int n, l=1;

    if ((n=(thischar)())<0)
	return NULL;
    while ((n>='a' && n<='z') || (n>='A' && n<='Z') ||
	   (n>='0' && n<='9') || (n=='_'))
    {
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return NULL;
	l++;
    }
    if (v=(char *) fgw_tssmalloc(hd, l))
    {
	d=v;
	s=TOKPTR(pstate->ibufp-l);
	while (l--)
	{
	   *d=fgw_tolower(*s);
	   fgw_chrnext(s);
	   fgw_chrnext(d);
	}
	*d='\0';
    }
    else
	status=RC_NOMEM;
    return v;
}

/*
** scans input buffer for integer
*/
static int getdigits_sql(int base)
{
    int n, max='0'+base, hexmax=0;
    int r=0;

    if (max>'9')
    {
	max='9';
	hexmax='a'+base-11;
    }
    if ((n=(thischar)())<0)
	return -1;
    while (n)
    {
	if (n>='0' && n<=max)
	    r=r*base+n-'0';
	else if (hexmax && (n=tolower(n))>='a' && n<=hexmax)
	    r=r*base+n-'a'+10;
	else
	    break;
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
    }
    return r;
}

/*
** scans input buffer for double decimal places and exponent
*/
static int getfloat_sql(int i, double *f)
{
    int n;
    double d=10.0;

    *f=i;
    if ((n=(thischar)())<0)
	return -1;
    while (n>='0' && n<='9')
    {
	*f+=((n-'0')/d);
	d*=10.0;
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
    }
    if (n=='e' || n=='E')
    {
	int mult=1, exp=0;

	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    return -1;
	if (n=='-')
	{
	    mult=-1;
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		return -1;
	}
	else if ((n=='+'))
	{
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		return -1;
	}
	if ((exp=getdigits_sql(10))<0)
	    return -1;
	else
	    *f*=pow(10, mult*exp);
    }
    return 0;
}

/*
** scans input buffer for string
*/
static char *getstring_sql(fgw_tsstype **h)
{
    char *sbuf=NULL;
    int n, d;
    int s, sz=0, newsz=0;

    /* we should normally use (thischar)() but here it's fine */
    d=*TOKPTR(pstate->ibufp-1);
    s=pstate->ibufp;
    for(;;)
    {
	if ((n=(thischar)())<0)
	    goto bad_exit;
/*
** scan till end of quote
*/
	while ((n) && (n!=d))
	{
	    if (n=='\n')
		pstate->phase1.line_count++;
	    pstate->ibufp++;
	    if ((n=(thischar)())<0)
		goto bad_exit;
	    newsz++;
	}
/*
** none found, bail out
*/
	if (!n)
	{
	    status=RC_BADQT;
	    goto bad_exit;
	}
/*
** allocate new memory
*/
	if (!(sbuf=(char *) fgw_tssrealloc(h, sbuf, sz+newsz)))
	{
	    status=RC_NOMEM;
	    goto bad_exit;
	}
/*
** copy string & adjust sizes
*/
	if (newsz)
	    fgw_move(sbuf+sz, TOKPTR(s), newsz);
	sz+=newsz;
	*(sbuf+sz)='\0';
/*
** single quote: we're thru
*/
	pstate->ibufp++;
	if ((n=(thischar)())<0)
	    goto bad_exit;
	if (n!=d)
	    return sbuf;
/*
** if it was two quotes instead, will have to start again
*/
	else
	{
	    newsz=1;
	    s=pstate->ibufp++;
	}
    }

bad_exit:
    if (sbuf)
	fgw_tssfree(h, sbuf);
    return NULL;
}
