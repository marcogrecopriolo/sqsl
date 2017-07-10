/* A Bison parser, made by GNU Bison 2.7.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2012 Free Software Foundation, Inc.
   
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
#define YYBISON_VERSION "2.7"

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
#define yylval          csqpylval
#define yychar          csqpychar
#define yydebug         csqpydebug
#define yynerrs         csqpynerrs

/* Copy the first part of user declarations.  */
/* Line 371 of yacc.c  */
#line 1 "csqpy.y"

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


/* Line 371 of yacc.c  */
#line 266 "csqpy.c"

# ifndef YY_NULL
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULL nullptr
#  else
#   define YY_NULL 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int csqpydebug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     LASTOK = 276,
     VASTOK = 277,
     VARTOK = 278,
     LSTTOK = 279,
     XPRTOK = 280,
     S_END = 281,
     COMPOUND = 282,
     IMMEDIATE = 283,
     BEGIN = 284,
     QUIT = 285,
     C_END = 286,
     DONE = 287,
     DO = 288,
     THEN = 289,
     FI = 290,
     ELSE = 291,
     ELIF = 292,
     IF = 293,
     WAIT = 294,
     PARENT = 295,
     CLONE = 296,
     WHILE = 297,
     FOREACH = 298,
     FOR = 299,
     EXIT = 300,
     BREAK = 301,
     CONTINUE = 302,
     WHENEVER = 303,
     UNLOAD = 304,
     LOAD = 305,
     STORE = 306,
     INVOKE = 307,
     DISPOSE = 308,
     LET = 309,
     DISPLAY = 310,
     BLOCKSIZE = 311,
     REJECT = 312,
     WIDTH = 313,
     READ = 314,
     INPUT = 315,
     WRITE = 316,
     OUTPUT = 317,
     APPEND = 318,
     PIPE = 319,
     FREE = 320,
     PREPARE = 321,
     CREATE = 322,
     UPDATE = 323,
     DELETE = 324,
     SOURCE = 325,
     USER = 326,
     AS = 327,
     CONNECTION = 328,
     S_SET = 329,
     DISCONNECT = 330,
     CONNECT = 331,
     HOLD = 332,
     WITH = 333,
     CURSOR = 334,
     FUNCTION = 335,
     PROCEDURE = 336,
     SQLTOK = 337,
     RETURNING = 338,
     HTML = 339,
     PLAIN = 340,
     DEFAULT = 341,
     STOP = 342,
     LOG = 343,
     ERROR = 344,
     PATTERN = 345,
     AUTOCOMMIT = 346,
     AGGREGATE = 347,
     IN = 348,
     DELIMITER = 349,
     BY = 350,
     TO = 351,
     FROM = 352,
     EXTRACTOR = 353,
     HEADERS = 354,
     REGEXP = 355,
     DELIMITED = 356,
     FULL = 357,
     VERTICAL = 358,
     BRIEF = 359,
     EXTERNAL = 360,
     SCRATCH = 361,
     TEMP = 362,
     NULLVALUE = 363,
     IS = 364,
     WHEN = 365,
     CASE = 366,
     PICTURE = 367,
     COLUMN = 368,
     COUNT = 369,
     SPLIT = 370,
     ESCAPE = 371,
     LIKE = 372,
     MATCHES = 373,
     BETWEEN = 374,
     TYPEDATE = 375,
     TYPEINTVL = 376,
     TYPEDTIME = 377,
     TYPEMONEY = 378,
     TYPEDEC = 379,
     TYPEFLOAT = 380,
     TYPEINT = 381,
     KWCAST = 382,
     TYPEBYTE = 383,
     TYPESTRING = 384,
     FRACTION = 385,
     SECOND = 386,
     MINUTE = 387,
     HOUR = 388,
     DAY = 389,
     MONTH = 390,
     YEAR = 391,
     ASSIGN = 392,
     OR = 393,
     AND = 394,
     GEQ = 395,
     LEQ = 396,
     NEQ = 397,
     EQ = 398,
     NOT = 399,
     DPIPE = 400,
     OPCAST = 401,
     PLUS = 402,
     NEG = 403,
     UNITS = 404
   };
#endif


#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 387 of yacc.c  */
#line 192 "csqpy.y"

char		*var;		/* variable names */
tokenlist_t	*fntok;		/* function descriptors */
int 		(*func)();	/* function pointers */
pcode_t		pcode;		/* executable expression code */
instruction_t	instr;		/* intermediate instructions */
instruction_t	*insptr;	/* intermediate instruction pointers */
exprstack_t	stack;		/* expression result */
int 		tstart;		/* token start in source - for sql hacks */
fgw_fdesc	*fd;		/* output statements */


/* Line 387 of yacc.c  */
#line 468 "csqpy.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE csqpylval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int csqpyparse (void *YYPARSE_PARAM);
#else
int csqpyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int csqpyparse (void);
#else
int csqpyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* Copy the second part of user declarations.  */

/* Line 390 of yacc.c  */
#line 496 "csqpy.c"

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
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
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
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(N) (N)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
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
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

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
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  49
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1578

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  165
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  192
/* YYNRULES -- Number of rules.  */
#define YYNRULES  419
/* YYNRULES -- Number of states.  */
#define YYNSTATES  800

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   404

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,   157,     2,     2,     2,
     159,   160,   151,   150,   158,   149,   161,   152,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   162,     2,
     141,   140,   142,     2,     2,     2,     2,     2,     2,     2,
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
     135,   136,   137,   138,   139,   143,   144,   145,   146,   147,
     148,   153,   154,   155,   156
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     8,    11,    14,    17,    18,    21,
      22,    29,    33,    34,    40,    41,    47,    49,    50,    53,
      56,    57,    64,    68,    69,    72,    74,    76,    77,    80,
      83,    84,    85,    93,    94,    99,   102,   103,   108,   109,
     112,   114,   117,   120,   123,   126,   129,   131,   134,   136,
     139,   140,   141,   151,   152,   162,   163,   170,   171,   172,
     173,   185,   186,   187,   188,   201,   202,   209,   211,   213,
     215,   217,   219,   221,   223,   226,   232,   237,   242,   247,
     251,   256,   258,   261,   263,   266,   269,   273,   274,   281,
     283,   288,   293,   296,   302,   304,   306,   311,   316,   321,
     325,   328,   330,   335,   340,   345,   350,   352,   354,   359,
     363,   367,   371,   373,   375,   377,   379,   382,   384,   386,
     388,   396,   401,   405,   407,   410,   412,   417,   419,   421,
     423,   425,   428,   430,   432,   435,   437,   449,   450,   457,
     461,   463,   466,   468,   470,   472,   475,   479,   484,   490,
     495,   498,   500,   501,   505,   507,   509,   513,   515,   516,
     522,   524,   526,   528,   531,   533,   539,   541,   542,   545,
     547,   550,   552,   554,   557,   560,   562,   565,   568,   572,
     573,   578,   582,   583,   587,   590,   591,   597,   601,   602,
     607,   611,   613,   615,   617,   619,   621,   623,   625,   628,
     630,   633,   634,   637,   641,   643,   645,   646,   651,   655,
     656,   663,   665,   668,   672,   673,   679,   680,   684,   687,
     689,   692,   694,   696,   699,   703,   708,   712,   714,   716,
     718,   719,   721,   722,   726,   728,   731,   732,   735,   736,
     739,   741,   742,   746,   748,   751,   753,   754,   759,   760,
     765,   767,   769,   771,   773,   775,   778,   780,   783,   788,
     790,   793,   797,   800,   802,   805,   807,   809,   812,   814,
     817,   820,   822,   826,   828,   831,   833,   835,   837,   839,
     842,   844,   849,   852,   853,   858,   862,   866,   870,   874,
     878,   882,   883,   888,   889,   894,   895,   903,   906,   909,
     911,   915,   917,   919,   922,   927,   928,   935,   941,   946,
     951,   955,   959,   963,   966,   970,   974,   978,   982,   986,
     990,   994,   998,  1000,  1002,  1004,  1006,  1008,  1011,  1014,
    1016,  1018,  1020,  1023,  1025,  1028,  1031,  1033,  1037,  1039,
    1041,  1046,  1051,  1056,  1061,  1064,  1065,  1069,  1070,  1077,
    1079,  1081,  1083,  1085,  1089,  1093,  1099,  1101,  1105,  1107,
    1111,  1113,  1115,  1117,  1120,  1122,  1126,  1128,  1130,  1132,
    1134,  1136,  1138,  1142,  1145,  1148,  1150,  1153,  1155,  1156,
    1162,  1163,  1166,  1167,  1173,  1176,  1178,  1180,  1183,  1185,
    1187,  1189,  1193,  1195,  1197,  1200,  1204,  1205,  1212,  1216,
    1220,  1223,  1225,  1227,  1232,  1234,  1237,  1241,  1243,  1245,
    1248,  1250,  1253,  1255,  1258,  1261,  1265,  1268,  1270,  1271
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     166,     0,    -1,    -1,    25,   167,   301,    -1,    24,   342,
      -1,    23,   312,    -1,    22,   248,    -1,    -1,   168,   169,
      -1,    -1,    26,    27,    21,   170,   273,   276,    -1,    26,
      28,    21,    -1,    -1,    29,    28,   202,   171,   203,    -1,
      -1,    29,    27,   202,   172,   203,    -1,    21,    -1,    -1,
     173,   174,    -1,    30,    21,    -1,    -1,     3,   177,   175,
     273,   276,    21,    -1,    67,   177,   356,    -1,    -1,   176,
     178,    -1,    80,    -1,    81,    -1,    -1,   179,   188,    -1,
     185,    21,    -1,    -1,    -1,   186,   180,   301,   201,   202,
     181,   203,    -1,    -1,    36,   202,   182,   203,    -1,   187,
      21,    -1,    -1,    40,   202,   183,   203,    -1,    -1,   184,
     208,    -1,    32,    -1,    31,    44,    -1,    31,    43,    -1,
      31,    42,    -1,    31,    41,    -1,    31,    39,    -1,    37,
      -1,    36,    38,    -1,    35,    -1,    31,    38,    -1,    -1,
      -1,    44,   312,   189,    93,   342,   200,   202,   190,   203,
      -1,    -1,    43,   204,   231,   274,   283,   200,   202,   191,
     203,    -1,    -1,    42,   301,   200,   202,   192,   203,    -1,
      -1,    -1,    -1,    41,   302,     8,   193,   312,   158,   194,
     312,   202,   195,   203,    -1,    -1,    -1,    -1,    39,    44,
     196,   342,   197,     8,   312,   158,   312,   202,   198,   203,
      -1,    -1,    38,   301,   201,   202,   199,   203,    -1,   355,
      -1,    33,    -1,   355,    -1,    34,    -1,   355,    -1,    21,
      -1,   355,    -1,   273,   205,    -1,     6,    82,   273,   281,
     276,    -1,     7,   273,   281,   276,    -1,     6,   273,   302,
     281,    -1,   206,     5,   281,   276,    -1,   213,   290,   298,
      -1,    12,    79,   207,    44,    -1,   207,    -1,    78,    77,
      -1,   355,    -1,    46,    21,    -1,    47,    21,    -1,    53,
     315,    21,    -1,    -1,    55,   273,   209,   342,   283,    21,
      -1,   210,    -1,    62,    10,   214,    21,    -1,    62,    58,
     302,    21,    -1,   212,   230,    -1,    89,    88,    96,   302,
      21,    -1,   218,    -1,   225,    -1,    48,    89,   215,    21,
      -1,    50,    97,   302,   216,    -1,    49,    96,   302,   216,
      -1,    45,   217,    21,    -1,   246,    21,    -1,   355,    -1,
      63,    96,   211,    21,    -1,    62,    96,   302,    21,    -1,
      64,    96,   302,    21,    -1,    61,    96,   302,    21,    -1,
      86,    -1,   302,    -1,   213,   273,   290,   298,    -1,    60,
      97,   302,    -1,    64,    97,   302,    -1,    59,    97,   302,
      -1,    85,    -1,    84,    -1,    47,    -1,    87,    -1,    94,
     302,    -1,   295,    -1,   355,    -1,   301,    -1,    76,    96,
     222,   224,   219,   220,   221,    -1,    74,    73,   223,    21,
      -1,    75,   223,    21,    -1,   355,    -1,    72,   302,    -1,
     355,    -1,    71,   302,     9,   302,    -1,    21,    -1,   356,
      -1,    86,    -1,   302,    -1,    86,   224,    -1,   302,    -1,
     355,    -1,    70,    12,    -1,   228,    -1,    66,   312,    97,
     273,   227,   277,   276,   231,   274,   283,    21,    -1,    -1,
       6,   273,   302,   226,   281,   230,    -1,    65,   302,    21,
      -1,     5,    -1,     6,    82,    -1,     7,    -1,   272,    -1,
      82,    -1,   229,   230,    -1,   271,   276,    21,    -1,     5,
     273,   281,   276,    -1,     6,    82,   273,   281,   276,    -1,
       7,   273,   281,   276,    -1,   231,   265,    -1,   355,    -1,
      -1,    92,   232,   233,    -1,   234,    -1,   238,    -1,   234,
     158,   235,    -1,   235,    -1,    -1,   159,   309,   160,   236,
     237,    -1,   248,    -1,   248,    -1,   244,    -1,   238,   239,
      -1,   239,    -1,   110,   309,    34,   240,   242,    -1,   246,
      -1,    -1,   241,   243,    -1,   355,    -1,    36,   243,    -1,
     246,    -1,   244,    -1,   245,    46,    -1,   245,    47,    -1,
     355,    -1,    57,   139,    -1,    54,   248,    -1,    52,   255,
     256,    -1,    -1,    14,   263,   247,   256,    -1,    51,   263,
     264,    -1,    -1,   313,   249,   250,    -1,   140,   301,    -1,
      -1,   253,   311,   251,   140,   301,    -1,   253,   140,   311,
      -1,    -1,   254,   140,   252,   302,    -1,   323,   140,   301,
      -1,   153,    -1,   150,    -1,   149,    -1,   151,    -1,   152,
      -1,   148,    -1,   317,    -1,    15,   322,    -1,   355,    -1,
      83,   257,    -1,    -1,   258,   259,    -1,   259,   158,   260,
      -1,   260,    -1,   312,    -1,    -1,   313,   153,   261,   311,
      -1,   315,   161,   151,    -1,    -1,   315,   161,   159,   262,
     263,   160,    -1,   343,    -1,     8,   257,    -1,   274,   283,
      21,    -1,    -1,   266,   267,   279,   270,    21,    -1,    -1,
     269,   268,   276,    -1,     6,   302,    -1,     4,    -1,     6,
      82,    -1,     7,    -1,   355,    -1,    91,   302,    -1,   272,
     273,   281,    -1,     5,    82,   273,   281,    -1,    82,   273,
     281,    -1,     4,    -1,    68,    -1,    69,    -1,    -1,   355,
      -1,    -1,     8,   275,   257,    -1,   355,    -1,    73,   222,
      -1,    -1,   278,   281,    -1,    -1,   280,   281,    -1,   355,
      -1,    -1,     9,   282,   342,    -1,   355,    -1,    10,   284,
      -1,   104,    -1,    -1,   287,   285,   288,   289,    -1,    -1,
     101,   286,   288,   293,    -1,   102,    -1,   103,    -1,   355,
      -1,   302,    -1,   355,    -1,    99,   342,    -1,   291,    -1,
      90,   292,    -1,    98,    12,   162,    12,    -1,   302,    -1,
     101,   293,    -1,   100,   101,   297,    -1,   294,   296,    -1,
     295,    -1,    95,   302,    -1,   355,    -1,   355,    -1,   116,
     302,    -1,   355,    -1,    95,   302,    -1,   299,   300,    -1,
     355,    -1,    57,    96,   302,    -1,   355,    -1,    56,   302,
      -1,   309,    -1,   302,    -1,   307,    -1,   108,    -1,   314,
     323,    -1,   317,    -1,    16,   159,   302,   160,    -1,   302,
      19,    -1,    -1,   111,   303,   330,     3,    -1,   302,   148,
     302,    -1,   302,   112,   302,    -1,   302,   150,   302,    -1,
     302,   149,   302,    -1,   302,   151,   302,    -1,   302,   152,
     302,    -1,    -1,   302,   156,   304,   327,    -1,    -1,   302,
     153,   305,   311,    -1,    -1,   127,   159,   302,   158,   306,
     311,   160,    -1,   149,   302,    -1,   150,   302,    -1,   308,
      -1,   159,   302,   160,    -1,    11,    -1,   314,    -1,   157,
     307,    -1,   113,   159,   302,   160,    -1,    -1,   302,   348,
     310,   159,   349,   160,    -1,   302,   350,   302,   139,   302,
      -1,   302,   351,   302,   354,    -1,   302,   352,   302,   354,
      -1,   302,   353,   302,    -1,   301,   138,   301,    -1,   301,
     139,   301,    -1,   147,   301,    -1,   302,   109,   340,    -1,
     302,   341,   302,    -1,   302,   145,   302,    -1,   302,   144,
     302,    -1,   302,   141,   302,    -1,   302,   143,   302,    -1,
     302,   142,   302,    -1,   159,   309,   160,    -1,   126,    -1,
     125,    -1,   124,    -1,   123,    -1,   120,    -1,   122,   325,
      -1,   121,   325,    -1,   129,    -1,   128,    -1,   313,    -1,
      12,   316,    -1,   308,    -1,    12,   316,    -1,    12,   316,
      -1,   355,    -1,   159,   343,   160,    -1,    71,    -1,    39,
      -1,   321,   159,   151,   160,    -1,   321,   159,   315,   160,
      -1,   321,   159,    16,   160,    -1,    12,   162,    12,   322,
      -1,   320,   322,    -1,    -1,    18,   318,   324,    -1,    -1,
      17,   159,   343,   160,   319,   324,    -1,    14,    -1,    13,
      -1,   114,    -1,   355,    -1,   159,   342,   160,    -1,   163,
     302,   164,    -1,   163,   302,   158,   302,   164,    -1,   355,
      -1,   326,    96,   327,    -1,   355,    -1,   326,    96,   327,
      -1,   329,    -1,   130,    -1,   329,    -1,   130,   328,    -1,
     355,    -1,   159,    11,   160,    -1,   136,    -1,   135,    -1,
     134,    -1,   133,    -1,   132,    -1,   131,    -1,   302,   331,
     335,    -1,   332,   339,    -1,   331,   333,    -1,   333,    -1,
     332,   337,    -1,   337,    -1,    -1,   110,   302,   334,    34,
     302,    -1,    -1,   336,   339,    -1,    -1,   110,   301,   338,
      34,   302,    -1,    36,   302,    -1,   355,    -1,   108,    -1,
     147,   108,    -1,   140,    -1,   146,    -1,   343,    -1,   343,
     158,   344,    -1,   344,    -1,   301,    -1,   346,   347,    -1,
     315,   161,   151,    -1,    -1,   315,   161,   159,   345,   263,
     160,    -1,   115,   301,   291,    -1,   301,    96,   301,    -1,
      15,   322,    -1,    16,    -1,   355,    -1,   161,   159,   263,
     160,    -1,    93,    -1,   147,    93,    -1,   349,   158,   301,
      -1,   301,    -1,   119,    -1,   147,   119,    -1,   118,    -1,
     147,   118,    -1,   117,    -1,   147,   117,    -1,   100,   117,
      -1,   147,   100,   117,    -1,   116,   302,    -1,   355,    -1,
      -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
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
  "FNCV", "FNDP", "FNDN", "PFIXFNCT", "TOK_ERROR", "LASTOK", "VASTOK",
  "VARTOK", "LSTTOK", "XPRTOK", "S_END", "COMPOUND", "IMMEDIATE", "BEGIN",
  "QUIT", "C_END", "DONE", "DO", "THEN", "FI", "ELSE", "ELIF", "IF",
  "WAIT", "PARENT", "CLONE", "WHILE", "FOREACH", "FOR", "EXIT", "BREAK",
  "CONTINUE", "WHENEVER", "UNLOAD", "LOAD", "STORE", "INVOKE", "DISPOSE",
  "LET", "DISPLAY", "BLOCKSIZE", "REJECT", "WIDTH", "READ", "INPUT",
  "WRITE", "OUTPUT", "APPEND", "PIPE", "FREE", "PREPARE", "CREATE",
  "UPDATE", "DELETE", "SOURCE", "USER", "AS", "CONNECTION", "S_SET",
  "DISCONNECT", "CONNECT", "HOLD", "WITH", "CURSOR", "FUNCTION",
  "PROCEDURE", "SQLTOK", "RETURNING", "HTML", "PLAIN", "DEFAULT", "STOP",
  "LOG", "ERROR", "PATTERN", "AUTOCOMMIT", "AGGREGATE", "IN", "DELIMITER",
  "BY", "TO", "FROM", "EXTRACTOR", "HEADERS", "REGEXP", "DELIMITED",
  "FULL", "VERTICAL", "BRIEF", "EXTERNAL", "SCRATCH", "TEMP", "NULLVALUE",
  "IS", "WHEN", "CASE", "PICTURE", "COLUMN", "COUNT", "SPLIT", "ESCAPE",
  "LIKE", "MATCHES", "BETWEEN", "TYPEDATE", "TYPEINTVL", "TYPEDTIME",
  "TYPEMONEY", "TYPEDEC", "TYPEFLOAT", "TYPEINT", "KWCAST", "TYPEBYTE",
  "TYPESTRING", "FRACTION", "SECOND", "MINUTE", "HOUR", "DAY", "MONTH",
  "YEAR", "ASSIGN", "OR", "AND", "'='", "'<'", "'>'", "GEQ", "LEQ", "NEQ",
  "EQ", "NOT", "DPIPE", "'-'", "'+'", "'*'", "'/'", "OPCAST", "PLUS",
  "NEG", "UNITS", "'$'", "','", "'('", "')'", "'.'", "':'", "'['", "']'",
  "$accept", "target", "$@1", "$@2", "stmt", "$@3", "$@4", "$@5", "$@6",
  "stmt1", "$@7", "$@8", "fnproc", "stmt2", "$@9", "$@10", "$@11", "$@12",
  "$@13", "$@14", "done", "elif", "endif", "ctrl", "$@15", "$@16", "$@17",
  "$@18", "$@19", "$@20", "$@21", "$@22", "$@23", "$@24", "$@25", "do",
  "then", "lastok", "lastok1", "fselect", "fsel1", "cursor", "whold",
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
  "draincl", YY_NULL
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
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
      61,    60,    62,   395,   396,   397,   398,   399,   400,    45,
      43,    42,    47,   401,   402,   403,   404,    36,    44,    40,
      41,    46,    58,    91,    93
};
# endif

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

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
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

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       7,     0,     0,     0,     2,     0,    17,   418,     0,     0,
       6,   333,   182,     5,   331,   301,   418,   350,   349,   418,
     401,     0,   345,   339,   338,   278,   283,   351,     0,     0,
       0,     0,     0,     0,   393,   276,   277,   299,   275,   302,
       0,   280,   418,     0,     4,   390,   392,   418,     0,     1,
      16,     0,     0,     8,    23,     0,   332,   336,     0,   418,
       0,   303,   302,     0,     0,   334,     0,   400,   352,     0,
       0,   418,     0,   418,     0,     0,     0,   313,   297,   298,
       0,   276,   275,     0,     0,     0,   282,   404,     0,     0,
       0,   412,   410,   408,   388,     0,     0,     0,     0,     0,
     389,     0,     0,     0,     0,     0,     0,   293,   291,     0,
     305,     0,     0,     0,     0,     0,   279,     0,   344,     0,
       0,     0,   394,   402,     3,     0,     0,   418,   418,     0,
       0,     0,    18,    38,     0,     0,   334,     0,     0,   196,
     193,   192,   194,   195,   191,   183,     0,     0,     0,   418,
       0,     0,     0,   361,   371,   370,   369,   368,   367,   366,
     346,     0,   360,   356,     0,     0,     0,   418,   377,     0,
       0,   398,     0,   300,   321,   399,   311,   312,   414,   386,
       0,   314,   286,   318,   320,   319,   317,   316,   405,     0,
     413,   411,   409,   285,   288,   287,   289,   290,     0,     0,
     315,     0,     0,   418,   418,   310,     0,   395,   396,   418,
       0,     0,     0,   391,     0,     9,    11,    72,    14,    71,
      12,    25,    26,    20,    19,   419,     0,    40,    48,   418,
      46,   418,    24,     0,   418,     0,    30,     0,   337,   304,
     184,   326,   418,   418,   325,   324,   323,   322,   330,   329,
       0,   185,   188,     0,   343,   353,   281,   347,     0,   382,
       0,   380,   375,   284,     0,   376,   373,   385,     0,   418,
     257,   259,     0,   295,   387,   415,   294,   418,   292,   362,
       0,     0,     0,   308,   417,   309,     0,   354,     0,   335,
     342,   340,   341,     0,   211,   230,   418,   418,   230,    22,
      49,    45,    44,    43,    42,    41,    47,    33,    36,     0,
       0,     0,     0,   230,     0,    28,   227,   230,   230,   230,
       0,   418,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   230,     0,     0,     0,     0,     0,     0,     0,     0,
     228,   229,     0,     0,     0,   230,     0,    39,    89,   418,
     230,    94,    95,   135,   418,     0,   418,   230,   101,    29,
       0,    35,   328,     0,   358,   327,   187,     0,     0,   190,
     418,   357,     0,   378,   374,   372,   418,   384,   418,     0,
     260,   418,   263,   265,     0,     0,     0,   363,   364,   407,
       0,   307,   416,     0,     0,   403,   418,    15,    73,    13,
     418,   418,   418,   418,    61,     0,   418,   230,   230,   418,
     418,    50,   230,   418,   230,     0,   418,   179,     0,   119,
     118,    84,    85,     0,     0,     0,     0,     0,   418,   418,
     197,     0,   177,    87,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   418,     0,   132,     0,
     418,     0,   152,    92,   214,   151,     0,   145,   100,     0,
       0,   234,   418,   418,     0,     0,   189,   348,     0,     0,
     381,     0,   261,   268,   264,     0,   262,   266,   258,     0,
       0,     0,   306,   355,   397,    10,     0,    34,    37,    70,
     418,    69,     0,    57,    68,   418,    67,   230,     0,   418,
     418,     0,     0,     0,    74,     0,    81,     0,    83,     0,
     418,   241,   418,   240,   418,   137,   418,   418,    99,   114,
     115,     0,   418,   418,   201,   181,   198,   201,   178,   199,
      86,     0,   111,   109,     0,   113,   112,     0,     0,     0,
     106,     0,   107,     0,   110,   139,   230,     0,     0,   131,
     133,   122,   129,   418,   130,   226,     0,     0,   232,   150,
       0,   418,   231,   418,   256,   235,   146,   224,   418,   359,
     186,   383,     0,   269,   267,   296,   365,   406,    21,    65,
      62,     0,    55,   418,   418,   418,   418,   418,    82,   418,
     418,     0,   225,     0,   147,   418,   418,   149,   180,    96,
       0,    98,   117,    97,   212,     0,   200,   418,   105,    90,
      91,   103,   102,   104,     0,   121,   134,   418,     0,     0,
       0,   153,   154,   157,   155,   164,   166,   160,   201,   219,
       0,   221,   238,   216,     0,     0,   243,     0,   108,   418,
     271,    31,   379,   418,     0,     0,   418,   418,    77,    76,
     418,     0,   418,    79,   418,   242,   148,   418,   116,   418,
     202,   204,   205,   331,     0,     0,   140,     0,   142,   144,
     236,   143,     0,   418,   123,    93,   275,   275,     0,   163,
     233,   220,   218,   418,   418,   418,   248,   250,   251,   245,
     244,   246,   213,     0,     0,   270,   273,   418,    66,     0,
      58,    56,    75,   418,    80,    78,   418,   138,   332,     0,
     206,     0,    88,   141,   418,   418,   124,     0,   419,   125,
     167,   158,   156,     0,     0,   222,   239,   217,   418,   418,
     272,   274,    32,     0,     0,    53,    51,   203,     0,   208,
     209,   418,   237,     0,   127,   120,   128,   418,   418,   418,
     223,   215,   418,   253,   252,   418,     0,   418,   418,   418,
     207,     0,   418,     0,   418,   165,   169,     0,   168,   172,
       0,   171,   175,   159,   162,   161,   249,     0,   247,   254,
     418,    59,    54,    52,     0,   418,   126,   170,   176,   173,
     174,   255,    63,   418,   210,     0,   418,    60,   136,    64
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,    48,     6,    53,   295,   297,   296,    54,   132,
     298,   133,   223,   232,   233,   360,   697,   401,   402,   234,
     235,   236,   237,   315,   509,   759,   758,   646,   581,   734,
     793,   492,   644,   796,   643,   495,   490,   218,   397,   409,
     504,   505,   506,   347,   531,   348,   541,   349,   350,   537,
     521,   601,   418,   351,   673,   718,   745,   553,   447,   549,
     352,   596,   670,   353,   354,   453,   454,   557,   621,   622,
     623,   749,   773,   624,   625,   747,   748,   765,   768,   769,
     770,   626,   517,   627,    63,   145,   367,   368,   146,   147,
     429,   528,   604,   605,   660,   661,   738,   761,   293,   525,
     559,   560,   632,   685,   633,   724,   356,   357,   396,   561,
     628,   460,   714,   715,   683,   684,   512,   593,   635,   690,
     729,   728,   691,   752,   778,   563,   564,   270,   380,   381,
     382,   476,   472,   638,   639,   695,    34,    35,    72,   199,
     198,   385,    36,    37,    38,   201,   251,   662,    14,    39,
      40,   136,    41,    71,   370,    42,    43,    67,   116,   160,
     362,   161,   278,   387,   162,   166,   261,   167,   262,   469,
     375,   376,   168,   372,   266,   181,   109,    44,    45,    46,
     288,    47,   122,   110,   390,   111,   112,   113,   114,   283,
     513,   299
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -562
static const yytype_int16 yypact[] =
{
     255,    21,    21,   780,  -562,   104,    80,  -134,   -27,    26,
    -562,  -562,  -562,  -562,  -562,  -562,     2,  -562,  -562,   -19,
      -5,     3,  -562,  -562,  -562,  -562,  -562,  -562,   617,    18,
     617,  1122,  1122,   617,   -42,  1223,  -562,  -562,  -562,   -69,
     -53,  -562,   -19,    29,  -562,    46,  -562,    69,   617,  -562,
    -562,    41,    98,  -562,    25,   780,  -562,  -562,  1122,  -134,
    1122,  -562,  -562,   419,   181,    72,   780,  -562,  -562,  1122,
     780,   327,   798,     2,    -5,   110,  1122,  -562,    62,    62,
      76,  1148,   107,   617,   617,   617,  -562,  -562,   103,   -65,
    1122,  -562,  -562,  -562,  -562,  1122,  1122,  1122,  1122,  1122,
    -562,   175,  1122,  1122,  1122,  1122,  1122,  -562,  -562,  1122,
    -562,  1122,  1122,  1122,  1122,  1122,  -562,   -94,  -562,    35,
     780,    99,  -562,  -562,    76,   219,   264,   270,   270,   118,
     280,   118,  -562,  1369,   -11,   385,  -562,   512,   617,  -562,
    -562,  -562,  -562,  -562,  -562,  -562,  1438,   171,   177,   -19,
     160,   877,    11,  -562,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,   232,  -562,  -562,   617,   325,   333,     6,  -562,   341,
     328,  -562,   909,  -562,  -562,    76,   202,  -562,  -562,  -562,
     243,  -562,  1328,  1339,  1339,  1339,  1339,  1339,  -562,   239,
    -562,  -562,  -562,   182,   156,   156,    39,    39,   843,   649,
    1339,   203,  1065,  1000,  1000,  1339,   267,  -562,  -562,  -134,
     204,   206,   207,  -562,   780,  -562,  -562,  -562,  -562,  -562,
    -562,  -562,  -562,  -562,  -562,  -562,   222,  -562,  -562,    42,
    -562,   270,  -562,   553,  1468,   344,  -562,   347,  -562,  -562,
      76,  -562,   327,   327,  -562,  -562,  -562,  -562,  -562,  -562,
     843,  -562,  -562,   617,  -562,  -562,  -562,  -562,   649,    76,
    1122,   262,  -562,  -562,  1122,  -562,  -562,  -562,   274,   281,
    -562,  1339,   215,  -562,  -562,  -562,  -562,   229,  -562,  -562,
     617,  1122,  1122,  -562,  -562,  -562,  1122,  -562,   780,  -562,
    -562,  -562,  -562,   218,    46,  -562,  -562,  -562,  -562,  -562,
    -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,   617,
     337,  1122,   617,    81,    21,  -562,  -562,   309,   318,  -562,
     780,   617,   380,   384,   319,   311,   312,   780,   489,   398,
      21,  -562,   314,   316,   330,    57,   331,   142,  1122,    21,
    -562,  -562,   356,   848,   334,  -562,   326,  -562,  -562,   340,
    -562,  -562,  -562,  -562,   340,   412,   361,  -562,  -562,  -562,
     617,  -562,  -562,   342,  -562,  -562,  -562,   296,  1122,    76,
     327,  -562,   406,  1339,  -562,  -562,   407,  1339,   349,  1122,
    -562,   329,  -562,  -562,   434,   843,   437,  -562,  -562,    76,
      65,  1339,  1339,   234,   290,  -562,   361,  -562,  -562,  -562,
     361,  -562,  -562,    12,  -562,    31,    37,   369,  -562,   340,
      59,  -562,  -562,   444,  -562,  1122,   444,  -562,   443,    76,
    -562,  -562,  -562,    -6,  1122,  1122,   457,   307,   -19,   388,
    -562,   451,  -562,  -562,  1122,  1122,  1122,   167,  1122,  1122,
     865,  1122,  1122,  1201,   382,   848,   410,   463,  1339,   969,
     444,   391,  -562,  -562,   195,  -562,   -15,  -562,  -562,   969,
     471,  -562,   444,    12,   649,   617,  1339,  -562,  1122,   461,
    -562,  1122,  -562,  -562,  1339,  1122,  -562,  -562,  -562,   336,
     339,   617,  -562,  -562,  -562,  -562,   487,  -562,  -562,  -562,
     270,  -562,   780,  -562,  -562,   270,  -562,  -562,  1122,   444,
     503,   433,   416,   440,  -562,   509,  -562,   -15,  -562,   425,
     444,  -562,   361,  -562,   444,  1339,   361,   388,  -562,  -562,
    -562,   498,   941,   941,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,   780,  1339,  1339,  1232,  -562,  -562,   500,  1243,  1266,
    -562,   501,  1339,  1286,  1339,  -562,  -562,   504,   514,  -562,
    -562,  -562,  -562,   410,  1339,  -562,  1122,    60,  -562,  -562,
     298,   520,  -562,   467,  -562,  -562,  -562,  -562,   270,  -562,
      76,  1339,  1122,  1339,  1339,  -562,  -562,    76,  -562,  -562,
    -562,    21,  -562,   444,    94,   361,   520,   462,  -562,   444,
     467,   780,  -562,   780,  -562,   361,   444,  -562,  -562,  -562,
    1122,  -562,  -562,  -562,  -562,    44,  -562,   520,  -562,  -562,
    -562,  -562,  -562,  -562,   205,  -562,  -562,   470,  1315,   617,
     617,  -562,   381,  -562,    48,  -562,  -562,  -562,  -562,  -562,
    1033,  -562,  -562,  -562,   223,   522,  -562,   448,  -562,   493,
    -562,  -562,  1339,  -562,   542,   393,  -562,   361,  -562,  -562,
     519,   511,   361,  -562,   519,  -562,  -562,   340,  1339,  -134,
     395,  -562,  -562,   404,   397,   541,  -562,   481,  -562,  -562,
    -562,  -562,  1122,   494,  -562,  -562,   539,   414,    32,  -562,
    -562,  -562,  1339,   484,   444,   361,  -562,  -562,  -562,  -562,
    -562,  -562,  -562,  1122,  1122,  -562,  -562,  -562,  -562,    21,
    -562,  -562,  -562,   270,  -562,  -562,   270,  -562,    72,    44,
    -562,   -29,  -562,  -562,   361,   444,  1339,  1122,   555,  -562,
    -562,  -562,  -562,  1122,   556,  -562,  -562,  -562,  1122,  1122,
    1339,  1339,  -562,   423,    21,  -562,  -562,  -562,   843,  -562,
    -562,   340,  -562,   147,  -562,  -562,  -562,   548,    84,    33,
    1339,  -562,   281,  1339,  -562,   488,    21,   270,  -562,  -562,
    -562,   780,   503,  1122,    84,  -562,  -562,   447,  -562,  -562,
     209,  -562,  -562,  -562,  -562,  -562,  -562,   780,  -562,  -562,
     270,  -562,  -562,  -562,   429,   520,  1339,  -562,  -562,  -562,
    -562,  -562,  -562,  -562,  -562,   577,  -562,  -562,  -562,  -562
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,  -562,   468,  -562,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,  -562,  -562,  -562,  -562,  -561,   141,   -98,  -273,  -562,
    -562,  -562,    19,  -562,  -562,  -562,  -562,  -562,   197,  -562,
    -562,    86,  -562,  -562,  -562,  -562,  -562,   152,   168,    63,
    -562,  -562,  -562,  -562,  -562,  -338,  -396,  -562,  -562,  -562,
     -63,  -562,  -562,  -562,    -1,  -562,  -562,  -562,  -147,  -131,
    -562,  -216,  -562,     0,  -562,  -562,  -562,  -562,  -562,  -562,
    -562,   102,  -504,  -562,  -562,   -87,  -562,  -562,  -267,  -562,
    -562,  -562,  -562,  -562,  -562,  -562,  -562,    13,  -240,  -478,
    -562,  -364,  -562,  -562,  -562,  -562,  -355,  -562,  -559,  -562,
    -562,  -562,  -562,  -104,  -562,   119,   562,  -562,  -114,  -562,
    -251,  -562,  -562,    51,  -562,  -562,     1,   730,  -562,  -562,
    -562,  -562,   633,     9,   -18,  -562,  -191,     4,     7,   635,
    -116,    -2,   320,  -562,  -562,  -562,  -562,   -22,   583,   279,
     408,    73,  -232,  -562,  -182,  -562,  -562,  -562,   392,  -562,
    -562,  -562,   491,  -562,   278,  -562,  -562,   -64,   -51,   535,
    -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,  -562,   465,
      -7,   -48
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -419
static const yytype_int16 yytable[] =
{
      57,    10,   150,   212,   134,    56,    13,   276,    12,    57,
      11,    11,    68,   500,    65,    82,   457,   279,   355,   152,
     118,   394,   586,   606,   399,    55,   371,   650,   129,    75,
     220,    77,   485,     7,    80,    68,   486,    15,    59,   493,
     123,   519,   264,   179,     7,     7,   489,   209,   665,   124,
      86,   210,    57,   417,    83,   130,   659,   207,   400,   366,
     426,   516,   320,   217,   163,   208,    57,   437,   125,   126,
     494,   501,     7,   410,   320,   169,   279,   413,   415,   416,
     306,   520,   180,   170,   175,   176,   177,   407,   408,   703,
     767,   433,   131,   706,   115,   555,    84,    85,   320,   327,
     328,    50,   330,   511,    49,   450,    51,   567,   117,    52,
     456,   327,   328,    86,   330,   438,   164,   462,   332,   333,
     219,   219,   739,   502,   680,   127,   128,   254,   487,   488,
     740,   307,    58,   308,     8,   327,   328,   503,   330,   240,
      66,   767,    68,    90,   585,     8,     8,   120,   594,   238,
      84,    85,   597,   439,    69,   592,   763,     8,   619,   595,
     267,    55,    70,   294,    64,   259,    86,   498,   499,   120,
     619,   257,   510,     8,   514,    84,    85,    76,     9,   102,
     103,   104,   105,   106,   107,    60,   211,   108,   119,     9,
       9,   620,   107,   149,   479,   108,   284,   284,   221,   222,
     169,     9,    57,   558,   120,  -418,    90,   289,   170,   316,
     666,   667,   668,   431,    84,    85,  -418,     9,   108,   620,
     178,   649,   219,   481,   219,   482,   795,   358,   647,   648,
     121,   656,   569,  -335,   652,   364,   364,   294,   441,   442,
     215,   657,   102,   103,   104,   105,   106,   107,    84,    85,
     108,   535,   536,    86,   369,   789,   790,   583,   214,    90,
     300,   301,   383,   302,   303,   304,   305,   174,   188,   294,
     388,   602,   602,   340,   341,   189,   294,     1,     2,     3,
       4,   389,   279,   702,   785,   216,    86,   669,   705,   398,
     398,   217,   190,   191,   192,   102,   103,   104,   105,   106,
     107,   224,   629,   108,   630,   631,   614,   105,   106,   107,
     403,   252,   108,   406,   420,   363,   363,   253,   411,   707,
     255,   727,   419,    11,   686,   687,   688,   689,   258,   726,
     432,   103,   104,   105,   106,   107,   263,    12,   108,    11,
     272,    85,   455,   444,    86,   762,    90,   455,    11,   461,
     741,   274,    15,    73,    17,    18,   275,    74,    21,    22,
     742,   463,   280,   163,   290,   359,   291,   292,   361,   267,
     698,   473,   260,   701,   477,   378,   379,   384,   395,    90,
      23,   404,   102,   103,   104,   105,   106,   107,   386,   461,
     108,   412,   579,   461,   398,   398,   491,   582,   483,   496,
     414,   421,   455,   508,    86,   422,   526,   424,   423,   425,
     209,   434,    24,   435,   451,   102,   103,   104,   105,   106,
     107,    68,   529,   108,   732,   286,   436,   440,   580,   445,
     449,   287,   452,   458,   459,   260,   465,    90,   464,   550,
     468,   268,   269,   264,   471,   475,   478,   562,   480,    25,
     484,   497,    26,   511,     8,    27,   491,   153,   154,   155,
     156,   157,   158,   159,   518,   524,   570,   607,    29,    64,
     641,   527,   530,   102,   103,   104,   105,   106,   107,   546,
     548,   108,   577,   219,   551,   782,   783,   556,   219,   664,
      31,    32,   566,   562,   784,   572,   575,    90,     9,   576,
      60,   427,    17,    18,   428,   461,    21,    22,   578,   461,
     529,   558,   587,   442,   589,   383,   383,   588,   591,   599,
     797,   609,   612,   799,   637,   615,   616,   654,    23,   655,
     634,    86,   771,   102,   103,   104,   105,   106,   107,   678,
     503,   108,   672,   692,   693,   239,   550,   760,   771,   694,
     699,   700,   494,   709,   636,   704,   640,   710,   711,   138,
      24,   219,   712,   713,    12,   717,    11,   139,   140,   141,
     142,   143,   144,   720,   721,   723,   744,   751,   461,   636,
     508,   756,   115,   640,   764,   645,   788,   777,   461,   794,
      11,   309,   310,   664,   311,   312,   313,   314,   798,   225,
     636,   676,   677,    27,   568,   735,   651,   507,   736,   603,
     674,   565,   663,   547,    11,   722,   617,   787,   774,   598,
      80,    80,   737,   679,    90,   755,   590,   671,    15,    73,
      17,    18,   696,    74,    21,    22,   398,   171,   776,   398,
     461,   653,    61,   496,    62,   461,   148,   496,   430,   467,
     455,   365,    57,   374,   470,   213,    23,   708,   265,   781,
     102,   103,   104,   105,   106,   107,   719,     0,   108,   285,
     746,     0,   173,     0,     0,     0,   725,     0,   461,     0,
       0,     0,   792,     0,     0,    12,     0,    11,    24,     0,
     398,     0,     0,     0,     0,     0,   219,     0,     0,   219,
       0,     0,     0,   733,     0,     0,     0,   461,    11,     0,
     294,     0,     0,   791,     0,     0,   663,     0,    11,     0,
       0,   754,   754,     0,     0,    25,     0,     0,    26,     0,
       8,    27,     0,     0,   455,     0,     0,     0,   757,     0,
     766,   772,   772,    11,    29,   383,     0,     0,   779,   775,
     219,   398,   398,     0,     0,   562,    12,   772,    11,     0,
     780,    78,    79,    81,    30,    11,    31,    32,     0,     0,
       0,     0,     0,   219,     9,     0,    33,     0,   636,   277,
     154,   155,   156,   157,   158,   159,   398,     0,   135,   398,
     137,    15,    16,    17,    18,    19,    20,    21,    22,   151,
       0,     0,   165,     0,     0,     0,   172,     0,     0,    15,
      73,    17,    18,     0,    74,    21,    22,     0,     0,    23,
     182,     0,     0,     0,     0,   183,   184,   185,   186,   187,
       0,     0,   193,   194,   195,   196,   197,    23,     0,   200,
       0,   202,   203,   204,   205,   206,     0,     0,     0,     0,
       0,    24,     0,     0,     0,     0,     0,     0,     0,    15,
      73,    17,    18,     0,    74,    21,    22,     0,     0,    24,
       0,     0,     0,     0,     0,     0,    15,    73,    17,    18,
       0,    74,    21,    22,     0,     0,     0,    23,    25,     0,
       0,    26,     0,     8,    27,    28,    86,     0,     0,   271,
       0,     0,     0,     0,    23,     0,    25,    29,   164,    26,
       0,     8,    27,     0,     0,     0,     0,     0,     0,    24,
       0,     0,     0,     0,     0,    29,     0,    30,    86,    31,
      32,     0,     0,     0,   446,     0,    24,     9,     0,    33,
       0,     0,     0,     0,     0,     0,     0,    31,    32,     0,
       0,   540,     0,     0,     0,     9,    25,    60,     0,    26,
      86,     8,    27,   241,   242,   243,   244,   245,   246,   247,
       0,   248,   249,    25,     0,    29,    26,     0,     8,    27,
      15,    73,    17,    18,     0,    74,    21,    22,     0,    90,
     373,     0,    29,     0,   377,     0,     0,    31,    32,     0,
       0,     0,     0,     0,     0,     9,     0,    60,    23,     0,
       0,   391,   392,     0,    31,    32,   393,     0,     0,    86,
       0,    90,     9,     0,    60,   102,   103,   104,   105,   106,
     107,     0,     0,   108,     0,   600,     0,   256,     0,     0,
      24,   405,     0,     0,    15,    73,    17,    18,     0,    74,
      21,    22,     0,    90,     0,   552,     0,   102,   103,   104,
     105,   106,   107,     0,     0,   108,     0,   273,   443,     0,
       0,     0,    23,   448,     0,     0,     0,    25,     0,     0,
      26,     0,     8,    27,    86,     0,     0,     0,     0,   102,
     103,   104,   105,   106,   107,     0,    29,   108,   466,     0,
       0,     0,     0,     0,    24,     0,     0,     0,     0,   474,
       0,     0,    90,     0,     0,   681,   282,     0,    31,    32,
       0,     0,     0,     0,     0,     0,     9,     0,    60,     0,
       0,     0,     0,    15,    73,    17,    18,     0,    74,    21,
      22,    25,     0,     0,    26,   515,     8,    27,   102,   103,
     104,   105,   106,   107,   522,   523,   108,     0,     0,     0,
      29,    23,     0,     0,   532,   533,   534,    86,   538,   539,
     542,   543,   544,     0,     0,   448,     0,    90,     0,   554,
       0,     0,    31,    32,     0,     0,     0,     0,     0,   554,
       9,     0,    60,    24,     0,     0,     0,     0,   571,     0,
       0,   573,     0,     0,   281,   574,     0,     0,     0,     0,
       0,     0,     0,   102,   103,   104,   105,   106,   107,     0,
      86,   108,   545,     0,     0,     0,     0,     0,   584,     0,
      25,     0,     0,    26,     0,     8,    27,     0,     0,     0,
       0,    87,    86,     0,     0,     0,     0,     0,    88,    29,
       0,    86,     0,   608,     0,     0,     0,    89,     0,     0,
      90,     0,    86,     0,   610,    91,    92,    93,     0,     0,
       0,    31,    32,     0,     0,     0,     0,     0,     0,     9,
       0,    60,     0,     0,     0,    86,   618,   611,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   642,     0,   108,    86,     0,   613,   173,     0,
       0,     0,     0,    90,     0,     0,    87,     0,     0,     0,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
     658,     0,    89,     0,    86,    90,   675,     0,     0,     0,
      91,    92,    93,     0,    90,     0,     0,    86,     0,   102,
     103,   104,   105,   106,   107,    90,     0,   108,    86,     0,
     682,     0,     0,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,     0,    90,   108,
     102,   103,   104,   105,   106,   107,     0,     0,   108,     0,
       0,   102,   103,   104,   105,   106,   107,     0,    90,   108,
     226,   227,   716,     0,   228,   229,   230,   -27,   -27,   231,
     -27,   -27,   -27,   -27,   102,   103,   104,   105,   106,   107,
       0,     0,   108,   730,   731,     0,     0,    90,     0,     0,
       0,     0,     0,     0,   102,   103,   104,   105,   106,   107,
    -419,     0,   108,     0,     0,     0,     0,   743,     0,     0,
       0,    90,     0,   750,     0,     0,     0,     0,   753,   753,
       0,     0,     0,   102,   103,   104,   105,   106,   107,     0,
       0,   108,   316,   317,   318,   319,   102,   103,   104,   105,
     106,   107,   320,     0,   108,     0,     0,   102,   103,   104,
     105,   106,   107,   786,     0,   108,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   321,   322,   323,   324,   325,   326,   327,
     328,   329,   330,   331,     0,     0,     0,   332,   333,   334,
     335,   336,   337,   338,   339,     0,   340,   341,     0,     0,
       0,     0,   342,   343,   344,     0,     0,     0,     0,     0,
     345,     0,     0,     0,     0,     0,     0,   346,   241,   242,
     243,   244,   245,   246,   247,     0,   248,   249,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   250
};

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-562)))

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-419)))

static const yytype_int16 yycheck[] =
{
       7,     1,    66,   119,    55,     7,     2,   198,     1,    16,
       1,     2,    19,   409,    16,    33,   354,   199,   234,    70,
      42,   288,   500,   527,   297,   159,   258,   586,     3,    28,
     128,    30,   396,    12,    33,    42,   400,    11,    12,     8,
      47,    47,    36,   108,    12,    12,    34,    12,   607,    48,
      19,    16,    59,   320,    96,    30,    12,   151,   298,   250,
     327,   416,    14,    21,    71,   159,    73,    10,    27,    28,
      33,    12,    12,   313,    14,    90,   258,   317,   318,   319,
      38,    87,   147,    98,    83,    84,    85,     6,     7,   650,
      57,   331,    67,   654,   163,   450,   138,   139,    14,    51,
      52,    21,    54,     9,     0,   345,    26,   462,   161,    29,
     350,    51,    52,    19,    54,    58,   110,   357,    59,    60,
     127,   128,   151,    64,   628,    27,    28,   149,   401,   402,
     159,   229,   159,   231,   113,    51,    52,    78,    54,   138,
     159,    57,   149,   112,   499,   113,   113,   158,   512,   160,
     138,   139,   516,    96,   159,   510,     9,   113,   110,   514,
     167,   159,   159,   214,   162,   164,    19,   407,   408,   158,
     110,   160,   412,   113,   414,   138,   139,   159,   157,   148,
     149,   150,   151,   152,   153,   159,   151,   156,   159,   157,
     157,   159,   153,    12,   385,   156,   203,   204,    80,    81,
      90,   157,   209,     8,   158,    10,   112,   209,    98,     4,
       5,     6,     7,   329,   138,   139,    21,   157,   156,   159,
     117,   585,   229,   158,   231,   160,   785,   234,   583,   584,
     161,   595,   464,   161,   589,   242,   243,   288,    96,    97,
      21,   596,   148,   149,   150,   151,   152,   153,   138,   139,
     156,    84,    85,    19,   253,    46,    47,   497,   159,   112,
      38,    39,   269,    41,    42,    43,    44,   160,    93,   320,
     277,   522,   523,    68,    69,   100,   327,    22,    23,    24,
      25,   280,   464,   647,   762,    21,    19,    82,   652,   296,
     297,    21,   117,   118,   119,   148,   149,   150,   151,   152,
     153,    21,     4,   156,     6,     7,   546,   151,   152,   153,
     309,   140,   156,   312,   321,   242,   243,   140,   314,   657,
     160,   685,   321,   314,   101,   102,   103,   104,    96,   684,
     330,   149,   150,   151,   152,   153,     3,   330,   156,   330,
      12,   139,   349,   339,    19,   741,   112,   354,   339,   356,
     714,   108,    11,    12,    13,    14,   117,    16,    17,    18,
     715,   360,   159,   370,   160,    21,   160,   160,    21,   376,
     643,   378,   110,   646,   381,   101,    95,   162,   160,   112,
      39,    44,   148,   149,   150,   151,   152,   153,   159,   396,
     156,    82,   490,   400,   401,   402,   403,   495,   164,   406,
      82,    21,   409,   410,    19,    21,   428,    96,    89,    97,
      12,    97,    71,    97,    88,   148,   149,   150,   151,   152,
     153,   428,   429,   156,   697,   158,    96,    96,   492,    73,
      96,   164,    92,    21,    73,   110,   140,   112,    96,   446,
      34,   100,   101,    36,    95,   116,    12,   454,    11,   108,
     160,    82,   111,     9,   113,   114,   463,   130,   131,   132,
     133,   134,   135,   136,    21,     8,   465,   531,   127,   162,
     568,    83,    21,   148,   149,   150,   151,   152,   153,    97,
      70,   156,   481,   490,    21,   758,   759,    96,   495,   605,
     149,   150,    21,   500,   761,    34,   160,   112,   157,   160,
     159,    12,    13,    14,    15,   512,    17,    18,    21,   516,
     517,     8,    79,    97,     5,   522,   523,    77,    93,    21,
     793,    21,    21,   796,    57,    21,    12,   591,    39,   593,
      10,    19,   748,   148,   149,   150,   151,   152,   153,   158,
      78,   156,    72,    21,    96,   160,   553,   738,   764,    56,
       8,   158,    33,   158,   561,    44,   563,   153,   161,   140,
      71,   568,    21,    82,   557,    71,   557,   148,   149,   150,
     151,   152,   153,    34,   160,    91,    21,    21,   585,   586,
     587,   158,   163,   590,    36,   581,   139,    99,   595,   160,
     581,    38,    39,   709,    41,    42,    43,    44,    21,   131,
     607,   619,   620,   114,   463,   703,   587,   410,   706,   523,
     617,   459,   605,   445,   605,   678,   553,   764,   749,   517,
     619,   620,   709,   624,   112,   729,   507,   614,    11,    12,
      13,    14,   639,    16,    17,    18,   643,    75,   752,   646,
     647,   590,     9,   650,     9,   652,    63,   654,   328,   370,
     657,   243,   659,   261,   376,   120,    39,   659,   167,   757,
     148,   149,   150,   151,   152,   153,   673,    -1,   156,   204,
     718,    -1,   160,    -1,    -1,    -1,   683,    -1,   685,    -1,
      -1,    -1,   780,    -1,    -1,   678,    -1,   678,    71,    -1,
     697,    -1,    -1,    -1,    -1,    -1,   703,    -1,    -1,   706,
      -1,    -1,    -1,   699,    -1,    -1,    -1,   714,   699,    -1,
     761,    -1,    -1,   777,    -1,    -1,   709,    -1,   709,    -1,
      -1,   728,   729,    -1,    -1,   108,    -1,    -1,   111,    -1,
     113,   114,    -1,    -1,   741,    -1,    -1,    -1,   734,    -1,
     747,   748,   749,   734,   127,   752,    -1,    -1,   755,   749,
     757,   758,   759,    -1,    -1,   762,   749,   764,   749,    -1,
     756,    31,    32,    33,   147,   756,   149,   150,    -1,    -1,
      -1,    -1,    -1,   780,   157,    -1,   159,    -1,   785,   130,
     131,   132,   133,   134,   135,   136,   793,    -1,    58,   796,
      60,    11,    12,    13,    14,    15,    16,    17,    18,    69,
      -1,    -1,    72,    -1,    -1,    -1,    76,    -1,    -1,    11,
      12,    13,    14,    -1,    16,    17,    18,    -1,    -1,    39,
      90,    -1,    -1,    -1,    -1,    95,    96,    97,    98,    99,
      -1,    -1,   102,   103,   104,   105,   106,    39,    -1,   109,
      -1,   111,   112,   113,   114,   115,    -1,    -1,    -1,    -1,
      -1,    71,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    11,
      12,    13,    14,    -1,    16,    17,    18,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,    -1,    11,    12,    13,    14,
      -1,    16,    17,    18,    -1,    -1,    -1,    39,   108,    -1,
      -1,   111,    -1,   113,   114,   115,    19,    -1,    -1,   169,
      -1,    -1,    -1,    -1,    39,    -1,   108,   127,   110,   111,
      -1,   113,   114,    -1,    -1,    -1,    -1,    -1,    -1,    71,
      -1,    -1,    -1,    -1,    -1,   127,    -1,   147,    19,   149,
     150,    -1,    -1,    -1,    86,    -1,    71,   157,    -1,   159,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   149,   150,    -1,
      -1,    86,    -1,    -1,    -1,   157,   108,   159,    -1,   111,
      19,   113,   114,   120,   121,   122,   123,   124,   125,   126,
      -1,   128,   129,   108,    -1,   127,   111,    -1,   113,   114,
      11,    12,    13,    14,    -1,    16,    17,    18,    -1,   112,
     260,    -1,   127,    -1,   264,    -1,    -1,   149,   150,    -1,
      -1,    -1,    -1,    -1,    -1,   157,    -1,   159,    39,    -1,
      -1,   281,   282,    -1,   149,   150,   286,    -1,    -1,    19,
      -1,   112,   157,    -1,   159,   148,   149,   150,   151,   152,
     153,    -1,    -1,   156,    -1,    94,    -1,   160,    -1,    -1,
      71,   311,    -1,    -1,    11,    12,    13,    14,    -1,    16,
      17,    18,    -1,   112,    -1,    86,    -1,   148,   149,   150,
     151,   152,   153,    -1,    -1,   156,    -1,   158,   338,    -1,
      -1,    -1,    39,   343,    -1,    -1,    -1,   108,    -1,    -1,
     111,    -1,   113,   114,    19,    -1,    -1,    -1,    -1,   148,
     149,   150,   151,   152,   153,    -1,   127,   156,   368,    -1,
      -1,    -1,    -1,    -1,    71,    -1,    -1,    -1,    -1,   379,
      -1,    -1,   112,    -1,    -1,    82,   116,    -1,   149,   150,
      -1,    -1,    -1,    -1,    -1,    -1,   157,    -1,   159,    -1,
      -1,    -1,    -1,    11,    12,    13,    14,    -1,    16,    17,
      18,   108,    -1,    -1,   111,   415,   113,   114,   148,   149,
     150,   151,   152,   153,   424,   425,   156,    -1,    -1,    -1,
     127,    39,    -1,    -1,   434,   435,   436,    19,   438,   439,
     440,   441,   442,    -1,    -1,   445,    -1,   112,    -1,   449,
      -1,    -1,   149,   150,    -1,    -1,    -1,    -1,    -1,   459,
     157,    -1,   159,    71,    -1,    -1,    -1,    -1,   468,    -1,
      -1,   471,    -1,    -1,   139,   475,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   148,   149,   150,   151,   152,   153,    -1,
      19,   156,    21,    -1,    -1,    -1,    -1,    -1,   498,    -1,
     108,    -1,    -1,   111,    -1,   113,   114,    -1,    -1,    -1,
      -1,    93,    19,    -1,    -1,    -1,    -1,    -1,   100,   127,
      -1,    19,    -1,    21,    -1,    -1,    -1,   109,    -1,    -1,
     112,    -1,    19,    -1,    21,   117,   118,   119,    -1,    -1,
      -1,   149,   150,    -1,    -1,    -1,    -1,    -1,    -1,   157,
      -1,   159,    -1,    -1,    -1,    19,   556,    21,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
     152,   153,   572,    -1,   156,    19,    -1,    21,   160,    -1,
      -1,    -1,    -1,   112,    -1,    -1,    93,    -1,    -1,    -1,
      -1,    -1,    -1,   100,    -1,    -1,    -1,    -1,    -1,    -1,
     600,    -1,   109,    -1,    19,   112,    21,    -1,    -1,    -1,
     117,   118,   119,    -1,   112,    -1,    -1,    19,    -1,   148,
     149,   150,   151,   152,   153,   112,    -1,   156,    19,    -1,
     630,    -1,    -1,   140,   141,   142,   143,   144,   145,   146,
     147,   148,   149,   150,   151,   152,   153,    -1,   112,   156,
     148,   149,   150,   151,   152,   153,    -1,    -1,   156,    -1,
      -1,   148,   149,   150,   151,   152,   153,    -1,   112,   156,
      31,    32,   672,    -1,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,   148,   149,   150,   151,   152,   153,
      -1,    -1,   156,   693,   694,    -1,    -1,   112,    -1,    -1,
      -1,    -1,    -1,    -1,   148,   149,   150,   151,   152,   153,
     112,    -1,   156,    -1,    -1,    -1,    -1,   717,    -1,    -1,
      -1,   112,    -1,   723,    -1,    -1,    -1,    -1,   728,   729,
      -1,    -1,    -1,   148,   149,   150,   151,   152,   153,    -1,
      -1,   156,     4,     5,     6,     7,   148,   149,   150,   151,
     152,   153,    14,    -1,   156,    -1,    -1,   148,   149,   150,
     151,   152,   153,   763,    -1,   156,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    -1,    -1,    -1,    59,    60,    61,
      62,    63,    64,    65,    66,    -1,    68,    69,    -1,    -1,
      -1,    -1,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    -1,    -1,    -1,    -1,    89,   120,   121,
     122,   123,   124,   125,   126,    -1,   128,   129,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   140
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint16 yystos[] =
{
       0,    22,    23,    24,    25,   166,   168,    12,   113,   157,
     248,   308,   313,   312,   313,    11,    12,    13,    14,    15,
      16,    17,    18,    39,    71,   108,   111,   114,   115,   127,
     147,   149,   150,   159,   301,   302,   307,   308,   309,   314,
     315,   317,   320,   321,   342,   343,   344,   346,   167,     0,
      21,    26,    29,   169,   173,   159,   316,   355,   159,    12,
     159,   307,   314,   249,   162,   316,   159,   322,   355,   159,
     159,   318,   303,    12,    16,   301,   159,   301,   302,   302,
     301,   302,   309,    96,   138,   139,    19,    93,   100,   109,
     112,   117,   118,   119,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,   152,   153,   156,   341,
     348,   350,   351,   352,   353,   163,   323,   161,   322,   159,
     158,   161,   347,   355,   301,    27,    28,    27,    28,     3,
      30,    67,   174,   176,   343,   302,   316,   302,   140,   148,
     149,   150,   151,   152,   153,   250,   253,   254,   323,    12,
     342,   302,   343,   130,   131,   132,   133,   134,   135,   136,
     324,   326,   329,   355,   110,   302,   330,   332,   337,    90,
      98,   291,   302,   160,   160,   301,   301,   301,   117,   108,
     147,   340,   302,   302,   302,   302,   302,   302,    93,   100,
     117,   118,   119,   302,   302,   302,   302,   302,   305,   304,
     302,   310,   302,   302,   302,   302,   302,   151,   159,    12,
      16,   151,   315,   344,   159,    21,    21,    21,   202,   355,
     202,    80,    81,   177,    21,   177,    31,    32,    35,    36,
      37,    40,   178,   179,   184,   185,   186,   187,   160,   160,
     301,   120,   121,   122,   123,   124,   125,   126,   128,   129,
     140,   311,   140,   140,   322,   160,   160,   160,    96,   301,
     110,   331,   333,     3,    36,   337,   339,   355,   100,   101,
     292,   302,    12,   158,   108,   117,   311,   130,   327,   329,
     159,   139,   116,   354,   355,   354,   158,   164,   345,   316,
     160,   160,   160,   263,   343,   170,   172,   171,   175,   356,
      38,    39,    41,    42,    43,    44,    38,   202,   202,    38,
      39,    41,    42,    43,    44,   188,     4,     5,     6,     7,
      14,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    59,    60,    61,    62,    63,    64,    65,    66,
      68,    69,    74,    75,    76,    82,    89,   208,   210,   212,
     213,   218,   225,   228,   229,   246,   271,   272,   355,    21,
     180,    21,   325,   326,   355,   325,   311,   251,   252,   301,
     319,   327,   338,   302,   333,   335,   336,   302,   101,    95,
     293,   294,   295,   355,   162,   306,   159,   328,   355,   301,
     349,   302,   302,   302,   263,   160,   273,   203,   355,   203,
     273,   182,   183,   301,    44,   302,   301,     6,     7,   204,
     273,   312,    82,   273,    82,   273,   273,   263,   217,   301,
     355,    21,    21,    89,    96,    97,   263,    12,    15,   255,
     317,   315,   248,   273,    97,    97,    96,    10,    58,    96,
      96,    96,    97,   302,   312,    73,    86,   223,   302,    96,
     273,    88,    92,   230,   231,   355,   273,   230,    21,    73,
     276,   355,   273,   301,    96,   140,   302,   324,    34,   334,
     339,    95,   297,   355,   302,   116,   296,   355,    12,   311,
      11,   158,   160,   164,   160,   276,   276,   203,   203,    34,
     201,   355,   196,     8,    33,   200,   355,    82,   273,   273,
     231,    12,    64,    78,   205,   206,   207,   213,   355,   189,
     273,     9,   281,   355,   273,   302,   281,   247,    21,    47,
      87,   215,   302,   302,     8,   264,   322,    83,   256,   355,
      21,   209,   302,   302,   302,    84,    85,   214,   302,   302,
      86,   211,   302,   302,   302,    21,    97,   223,    70,   224,
     355,    21,    86,   222,   302,   281,    96,   232,     8,   265,
     266,   274,   355,   290,   291,   222,    21,   281,   201,   327,
     301,   302,    34,   302,   302,   160,   160,   301,    21,   202,
     342,   193,   202,   273,   302,   281,   274,    79,    77,     5,
     290,    93,   281,   282,   276,   281,   226,   276,   256,    21,
      94,   216,   295,   216,   257,   258,   257,   342,    21,    21,
      21,    21,    21,    21,   273,    21,    12,   224,   302,   110,
     159,   233,   234,   235,   238,   239,   246,   248,   275,     4,
       6,     7,   267,   269,    10,   283,   355,    57,   298,   299,
     355,   202,   302,   199,   197,   312,   192,   281,   281,   276,
     283,   207,   281,   298,   342,   342,   276,   281,   302,    12,
     259,   260,   312,   313,   315,   283,     5,     6,     7,    82,
     227,   272,    72,   219,   355,    21,   309,   309,   158,   239,
     257,    82,   302,   279,   280,   268,   101,   102,   103,   104,
     284,   287,    21,    96,    56,   300,   355,   181,   203,     8,
     158,   203,   276,   200,    44,   276,   200,   230,   316,   158,
     153,   161,    21,    82,   277,   278,   302,    71,   220,   355,
      34,   160,   235,    91,   270,   355,   281,   276,   286,   285,
     302,   302,   203,   312,   194,   202,   202,   260,   261,   151,
     159,   276,   281,   302,    21,   221,   356,   240,   241,   236,
     302,    21,   288,   302,   355,   288,   158,   312,   191,   190,
     311,   262,   231,     9,    36,   242,   355,    57,   243,   244,
     245,   246,   355,   237,   244,   248,   293,    99,   289,   355,
     312,   202,   203,   203,   263,   274,   302,   243,   139,    46,
      47,   342,   202,   195,   160,   283,   198,   203,    21,   203
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
      YYERROR;							\
    }								\
while (YYID (0))

/* Error token number */
#define YYTERROR	1
#define YYERRCODE	256


/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */
#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
        break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

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
#ifndef	YYINITDEPTH
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
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
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULL, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULL;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
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
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULL, yytname[yyx]);
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

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}




/* The lookahead symbol.  */
int yychar;


#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval YY_INITIAL_VALUE(yyval_default);

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

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
      yychar = YYLEX;
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
     `$$ = $1'.

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
/* Line 1792 of yacc.c  */
#line 290 "csqpy.y"
    { pstate->runneroptions|=RT_CANRUNSLEEP; }
    break;

  case 3:
/* Line 1792 of yacc.c  */
#line 290 "csqpy.y"
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
    break;

  case 4:
/* Line 1792 of yacc.c  */
#line 302 "csqpy.y"
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
    break;

  case 5:
/* Line 1792 of yacc.c  */
#line 314 "csqpy.y"
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
    break;

  case 6:
/* Line 1792 of yacc.c  */
#line 326 "csqpy.y"
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
    break;

  case 7:
/* Line 1792 of yacc.c  */
#line 338 "csqpy.y"
    {
		pstate->runneroptions|=(RT_CANRUNSLEEP|RT_CANSYSTEM);
	  }
    break;

  case 9:
/* Line 1792 of yacc.c  */
#line 347 "csqpy.y"
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
    break;

  case 10:
/* Line 1792 of yacc.c  */
#line 359 "csqpy.y"
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
    break;

  case 11:
/* Line 1792 of yacc.c  */
#line 372 "csqpy.y"
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
    break;

  case 12:
/* Line 1792 of yacc.c  */
#line 386 "csqpy.y"
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
    break;

  case 14:
/* Line 1792 of yacc.c  */
#line 412 "csqpy.y"
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
    break;

  case 17:
/* Line 1792 of yacc.c  */
#line 439 "csqpy.y"
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
    break;

  case 19:
/* Line 1792 of yacc.c  */
#line 475 "csqpy.y"
    {
			    if (pstate->flags & PF_QUIT)
				rsx_quit();
			    else
				FAILSYN;
			}
    break;

  case 20:
/* Line 1792 of yacc.c  */
#line 481 "csqpy.y"
    {
			    if (!pstate->control.count)
				FAILSYN;
			    if (((controlstack_t *)
				fgw_stackpeek(&pstate->control))->stmt_type!=
				(yyvsp[(2) - (2)].stack).type)
				FAILSYN;
			}
    break;

  case 21:
/* Line 1792 of yacc.c  */
#line 488 "csqpy.y"
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
    break;

  case 22:
/* Line 1792 of yacc.c  */
#line 499 "csqpy.y"
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    c.stmt_type=(yyvsp[(2) - (3)].stack).type;
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
			    c.text_start=(yyvsp[(1) - (3)].tstart);
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
    break;

  case 23:
/* Line 1792 of yacc.c  */
#line 523 "csqpy.y"
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
    break;

  case 25:
/* Line 1792 of yacc.c  */
#line 536 "csqpy.y"
    { (yyval.stack).type=T_FUNCTION; }
    break;

  case 26:
/* Line 1792 of yacc.c  */
#line 537 "csqpy.y"
    { (yyval.stack).type=T_PROCEDURE; }
    break;

  case 27:
/* Line 1792 of yacc.c  */
#line 542 "csqpy.y"
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
    break;

  case 29:
/* Line 1792 of yacc.c  */
#line 579 "csqpy.y"
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if ((yyvsp[(1) - (2)].stack).type!=DONE && (yyvsp[(1) - (2)].stack).type!=c->stmt_type)
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
    break;

  case 30:
/* Line 1792 of yacc.c  */
#line 612 "csqpy.y"
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			}
    break;

  case 31:
/* Line 1792 of yacc.c  */
#line 620 "csqpy.y"
    {
			    rsx_elif();
                            FAILCHECK;
			}
    break;

  case 33:
/* Line 1792 of yacc.c  */
#line 624 "csqpy.y"
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_else(c);
			}
    break;

  case 35:
/* Line 1792 of yacc.c  */
#line 634 "csqpy.y"
    {
			    controlstack_t *c;

			    if (!pstate->control.count)
				FAILSYN;
			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    if (c->stmt_type!=T_IF)
				FAILSYN;
			    rsx_endctrl();
			}
    break;

  case 36:
/* Line 1792 of yacc.c  */
#line 644 "csqpy.y"
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
    break;

  case 38:
/* Line 1792 of yacc.c  */
#line 661 "csqpy.y"
    { 
	    if (pstate->control.count &&
		((controlstack_t *) fgw_stackpeek(&pstate->control))->state!=
		  S_ACTIVE)
		{
		    NEXT_STMT
		}
	  }
    break;

  case 40:
/* Line 1792 of yacc.c  */
#line 671 "csqpy.y"
    { (yyval.stack).type=DONE; }
    break;

  case 41:
/* Line 1792 of yacc.c  */
#line 672 "csqpy.y"
    { (yyval.stack).type=T_FOR; }
    break;

  case 42:
/* Line 1792 of yacc.c  */
#line 673 "csqpy.y"
    { (yyval.stack).type=T_FOREACH; }
    break;

  case 43:
/* Line 1792 of yacc.c  */
#line 674 "csqpy.y"
    { (yyval.stack).type=T_WHILE; }
    break;

  case 44:
/* Line 1792 of yacc.c  */
#line 675 "csqpy.y"
    { (yyval.stack).type=T_CLONE; }
    break;

  case 45:
/* Line 1792 of yacc.c  */
#line 676 "csqpy.y"
    { (yyval.stack).type=T_WAIT; }
    break;

  case 50:
/* Line 1792 of yacc.c  */
#line 687 "csqpy.y"
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_FOR;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions|=SAFECOPYHACK;
			}
    break;

  case 51:
/* Line 1792 of yacc.c  */
#line 695 "csqpy.y"
    {
			    pstate->parseroptions&=~SAFECOPYHACK;
			    rsx_for();
			    FAILCHECK;
			}
    break;

  case 53:
/* Line 1792 of yacc.c  */
#line 700 "csqpy.y"
    {
			    endstmt_sql();
			    if ((yyvsp[(2) - (7)].var))
			    {
				instruction_t *i;

				i=rxx_newinstr(pstate);
				FAILCHECK
				i->func=rsx_setcursor;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string= (yyvsp[(2) - (7)].var);
			        i->constant.flags|=EF_NOTONTSS;
			    }
			    if (rsx_foreach(&(yyvsp[(5) - (7)].pcode)))
				FAILCHECK;
			}
    break;

  case 55:
/* Line 1792 of yacc.c  */
#line 716 "csqpy.y"
    {
			    rsx_while();
			    FAILCHECK;
			}
    break;

  case 57:
/* Line 1792 of yacc.c  */
#line 720 "csqpy.y"
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));

			    /* count is in destvar */
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_CLONE;
			    fgw_stackpush(&pstate->control, (char *) &c);
			}
    break;

  case 58:
/* Line 1792 of yacc.c  */
#line 729 "csqpy.y"
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* destvar is in destvar, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->destvar);

			    /* count is in clone */
			    rxx_swappcode(&pstate->pcode, &c->clone);

			}
    break;

  case 59:
/* Line 1792 of yacc.c  */
#line 740 "csqpy.y"
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);

			    /* pid is in clone, count in pcode */
			    rxx_swappcode(&pstate->pcode, &c->clone);
			    rsx_clone(c);
			    FAILCHECK;
			}
    break;

  case 61:
/* Line 1792 of yacc.c  */
#line 750 "csqpy.y"
    {
			    pstate->parseroptions|=SAFECOPYHACK;
			}
    break;

  case 62:
/* Line 1792 of yacc.c  */
#line 752 "csqpy.y"
    {
			    controlstack_t c;

			    memset(&c, 0, sizeof(c));
			    rxx_swappcode(&pstate->pcode, &c.destvar);
			    c.stmt_type=T_WAIT;
			    fgw_stackpush(&pstate->control, (char *) &c);
			    pstate->parseroptions&=~SAFECOPYHACK;
			}
    break;

  case 63:
/* Line 1792 of yacc.c  */
#line 760 "csqpy.y"
    {
			    controlstack_t *c;

			    c=(controlstack_t *) fgw_stackpeek(&pstate->control);
			    rxx_swappcode(&pstate->pcode, &c->destvar);
			    rsx_wait(c);
			    FAILCHECK;
			}
    break;

  case 65:
/* Line 1792 of yacc.c  */
#line 768 "csqpy.y"
    {
			    rsx_if();
                            FAILCHECK;
			}
    break;

  case 71:
/* Line 1792 of yacc.c  */
#line 784 "csqpy.y"
    {
			    pstate->phase1.next_stmt_start=pstate->tokstart;
			}
    break;

  case 73:
/* Line 1792 of yacc.c  */
#line 789 "csqpy.y"
    {
			    YYACCEPT;
			}
    break;

  case 74:
/* Line 1792 of yacc.c  */
#line 793 "csqpy.y"
    { (yyval.var)=(yyvsp[(2) - (2)].var); }
    break;

  case 75:
/* Line 1792 of yacc.c  */
#line 794 "csqpy.y"
    {
			    pstate->ssp->sqlstart=(yyvsp[(1) - (5)].tstart);
			    (yyval.var)=NULL;
			}
    break;

  case 76:
/* Line 1792 of yacc.c  */
#line 798 "csqpy.y"
    {
			    pstate->ssp->sqlstart=(yyvsp[(1) - (4)].tstart);
			    (yyval.var)=NULL;
			}
    break;

  case 77:
/* Line 1792 of yacc.c  */
#line 802 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_executeout;
			    markend_sql((yyvsp[(1) - (4)].tstart));
			    endstmt_sql();
			    (yyval.var)=NULL;
			}
    break;

  case 78:
/* Line 1792 of yacc.c  */
#line 813 "csqpy.y"
    {
			    pstate->ssp->sqlstart=(yyvsp[(2) - (4)].tstart);
			    (yyval.var)=(yyvsp[(1) - (4)].var);
			}
    break;

  case 79:
/* Line 1792 of yacc.c  */
#line 817 "csqpy.y"
    { (yyval.var)=NULL; }
    break;

  case 80:
/* Line 1792 of yacc.c  */
#line 819 "csqpy.y"
    {
				    (yyval.var)=(yyvsp[(1) - (4)].var);
				}
    break;

  case 81:
/* Line 1792 of yacc.c  */
#line 822 "csqpy.y"
    { (yyval.var)=NULL; }
    break;

  case 82:
/* Line 1792 of yacc.c  */
#line 824 "csqpy.y"
    { pstate->ssp->stmt->options|=SO_WITHHOLD; }
    break;

  case 84:
/* Line 1792 of yacc.c  */
#line 831 "csqpy.y"
    {
			    rsx_breaks(S_DISABLED);
			    FAILCHECK;
			}
    break;

  case 85:
/* Line 1792 of yacc.c  */
#line 835 "csqpy.y"
    {
			    rsx_breaks(S_CONTINUE);
			    FAILCHECK;
			}
    break;

  case 86:
/* Line 1792 of yacc.c  */
#line 839 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[(2) - (3)].instr), sizeof(instruction_t));
			    rsx_dispose();
			    FAILCHECK;
			}
    break;

  case 87:
/* Line 1792 of yacc.c  */
#line 848 "csqpy.y"
    {
			    rqf_setwidth(pstate->ssp->stmt, pstate->width);
	  		}
    break;

  case 88:
/* Line 1792 of yacc.c  */
#line 850 "csqpy.y"
    {
			    rsx_display(&(yyvsp[(5) - (6)].pcode));
			    FAILCHECK;
			}
    break;

  case 91:
/* Line 1792 of yacc.c  */
#line 856 "csqpy.y"
    {
			    rsx_width();
			    FAILCHECK;
			}
    break;

  case 93:
/* Line 1792 of yacc.c  */
#line 861 "csqpy.y"
    {
			    rsx_errorlog();
			    FAILCHECK;
			}
    break;

  case 97:
/* Line 1792 of yacc.c  */
#line 869 "csqpy.y"
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
    break;

  case 98:
/* Line 1792 of yacc.c  */
#line 884 "csqpy.y"
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
    break;

  case 99:
/* Line 1792 of yacc.c  */
#line 898 "csqpy.y"
    {
			    if (rsx_exit())
				FAILCHECK;
			}
    break;

  case 100:
/* Line 1792 of yacc.c  */
#line 902 "csqpy.y"
    {
			    rsx_evalexpr();
			    FAILCHECK;
			}
    break;

  case 103:
/* Line 1792 of yacc.c  */
#line 909 "csqpy.y"
    {
			    rsx_outputto("w");
			    FAILCHECK;
			}
    break;

  case 104:
/* Line 1792 of yacc.c  */
#line 913 "csqpy.y"
    {
			    rsx_pipeto();
			    FAILCHECK;
			}
    break;

  case 105:
/* Line 1792 of yacc.c  */
#line 917 "csqpy.y"
    {
			    rsx_writeto();
			    FAILCHECK;
			}
    break;

  case 106:
/* Line 1792 of yacc.c  */
#line 921 "csqpy.y"
    {
			    rsx_appenddefault();
			    FAILCHECK;
			}
    break;

  case 107:
/* Line 1792 of yacc.c  */
#line 925 "csqpy.y"
    {
			    rsx_outputto("a");
			    FAILCHECK;
			}
    break;

  case 109:
/* Line 1792 of yacc.c  */
#line 932 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_inputfrom;
			}
    break;

  case 110:
/* Line 1792 of yacc.c  */
#line 939 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_pipefrom;
			}
    break;

  case 111:
/* Line 1792 of yacc.c  */
#line 946 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_readfrom;
			}
    break;

  case 112:
/* Line 1792 of yacc.c  */
#line 954 "csqpy.y"
    {
			    if (pstate->style!=F_NONE)
				FAIL(RC_NOFMT)
			    rsx_flagreset(PF_ISHTML);
			}
    break;

  case 113:
/* Line 1792 of yacc.c  */
#line 959 "csqpy.y"
    { rsx_flagset(PF_ISHTML); }
    break;

  case 114:
/* Line 1792 of yacc.c  */
#line 961 "csqpy.y"
    { rsx_flagset(PF_ERRORCONT); }
    break;

  case 115:
/* Line 1792 of yacc.c  */
#line 962 "csqpy.y"
    { rsx_flagreset(PF_ERRORCONT); }
    break;

  case 120:
/* Line 1792 of yacc.c  */
#line 970 "csqpy.y"
    {
                            instruction_t *i;
			    int r;

                            i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[(4) - (7)].stack).val.string;
			    i->constant.flags|=EF_NOTONTSS;
			    if ((yyvsp[(5) - (7)].stack).type)
				i->constant.count++;
			    if ((yyvsp[(6) - (7)].stack).type)
				i->constant.count+=2;
			    switch ((yyvsp[(3) - (7)].stack).type)
			    {
			      case DEFAULT:
				r=rsx_defconnect(((yyvsp[(7) - (7)].tstart)<0)? NULL: TOKPTR((yyvsp[(7) - (7)].tstart)));
				break;
			      default:
				r=rsx_connect(((yyvsp[(7) - (7)].tstart)<0)? NULL: TOKPTR((yyvsp[(7) - (7)].tstart)));
			    }
			    if (r)
				FAILCHECK;
			}
    break;

  case 121:
/* Line 1792 of yacc.c  */
#line 995 "csqpy.y"
    {
			    int r;

			    switch ((yyvsp[(3) - (4)].stack).type)
			    {
			      case CONNECTION:
				r=rsx_setconnection();
				break;
			      case SOURCE:
				r=rsx_setsource((yyvsp[(3) - (4)].stack).val.string);
				break;
			      default:
				r=rsx_setdefault();
			    }
			    if (r)
				FAILCHECK;
			}
    break;

  case 122:
/* Line 1792 of yacc.c  */
#line 1012 "csqpy.y"
    {
			    int r;

			    switch ((yyvsp[(2) - (3)].stack).type)
			    {
			      case CONNECTION:
				r=rsx_disconnect();
				break;
			      case SOURCE:
				r=rsx_srcdisconnect((yyvsp[(2) - (3)].stack).val.string);
				break;
			      default:
				r=rsx_defdisconnect();
			    }
			    if (r)
				FAILCHECK;
			}
    break;

  case 123:
/* Line 1792 of yacc.c  */
#line 1030 "csqpy.y"
    { (yyval.stack).type=0; }
    break;

  case 124:
/* Line 1792 of yacc.c  */
#line 1031 "csqpy.y"
    { (yyval.stack).type=AS; }
    break;

  case 125:
/* Line 1792 of yacc.c  */
#line 1033 "csqpy.y"
    { (yyval.stack).type=0; }
    break;

  case 126:
/* Line 1792 of yacc.c  */
#line 1034 "csqpy.y"
    { (yyval.stack).type=USER; }
    break;

  case 127:
/* Line 1792 of yacc.c  */
#line 1036 "csqpy.y"
    { (yyval.tstart)=-1; }
    break;

  case 128:
/* Line 1792 of yacc.c  */
#line 1037 "csqpy.y"
    { (yyval.tstart)=(yyvsp[(1) - (1)].tstart); }
    break;

  case 129:
/* Line 1792 of yacc.c  */
#line 1039 "csqpy.y"
    { (yyval.stack).type=DEFAULT; }
    break;

  case 130:
/* Line 1792 of yacc.c  */
#line 1040 "csqpy.y"
    { (yyval.stack).type=CONNECTION; }
    break;

  case 131:
/* Line 1792 of yacc.c  */
#line 1042 "csqpy.y"
    { (yyval.stack)=(yyvsp[(2) - (2)].stack); }
    break;

  case 132:
/* Line 1792 of yacc.c  */
#line 1043 "csqpy.y"
    { (yyval.stack).type=CONNECTION; }
    break;

  case 133:
/* Line 1792 of yacc.c  */
#line 1045 "csqpy.y"
    {
			    (yyval.stack).val.string=NULL;
			    (yyval.stack).type=DEFAULT;
			}
    break;

  case 134:
/* Line 1792 of yacc.c  */
#line 1049 "csqpy.y"
    {
			    (yyval.stack).val.string=(yyvsp[(2) - (2)].var);
			    (yyval.stack).type=SOURCE;
			}
    break;

  case 136:
/* Line 1792 of yacc.c  */
#line 1059 "csqpy.y"
    {
			    endstmt_sql();
			    if (rsx_prepare(&(yyvsp[(10) - (11)].pcode)))
				FAILCHECK;
			}
    break;

  case 137:
/* Line 1792 of yacc.c  */
#line 1066 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executeout;
			}
    break;

  case 139:
/* Line 1792 of yacc.c  */
#line 1073 "csqpy.y"
    {
			    if (rsx_free())
				FAILCHECK;
			}
    break;

  case 140:
/* Line 1792 of yacc.c  */
#line 1079 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart); }
    break;

  case 141:
/* Line 1792 of yacc.c  */
#line 1080 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (2)].tstart); }
    break;

  case 142:
/* Line 1792 of yacc.c  */
#line 1081 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart); }
    break;

  case 144:
/* Line 1792 of yacc.c  */
#line 1085 "csqpy.y"
    { pstate->ssp->sqlstart=pstate->tokstart; }
    break;

  case 146:
/* Line 1792 of yacc.c  */
#line 1088 "csqpy.y"
    {
			    endstmt_sql();
			    if (rsx_sqlrun(STMTPTR(pstate->ssp)))
		    		FAILCHECK;
			}
    break;

  case 152:
/* Line 1792 of yacc.c  */
#line 1102 "csqpy.y"
    {
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    pstate->parseroptions|=AGGREGATEHACK;
	  }
    break;

  case 153:
/* Line 1792 of yacc.c  */
#line 1106 "csqpy.y"
    {
			    pstate->parseroptions&=~AGGREGATEHACK;
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->aggregates);
			    markend_sql((yyvsp[(1) - (3)].tstart));
			}
    break;

  case 158:
/* Line 1792 of yacc.c  */
#line 1119 "csqpy.y"
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
    break;

  case 159:
/* Line 1792 of yacc.c  */
#line 1129 "csqpy.y"
    {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
    break;

  case 167:
/* Line 1792 of yacc.c  */
#line 1147 "csqpy.y"
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
    break;

  case 168:
/* Line 1792 of yacc.c  */
#line 1157 "csqpy.y"
    {
			    instruction_t *e;
			    parsestack_t *p;

			    p=(parsestack_t *) fgw_stackpop(&pstate->parsestack);
			    p->instr->skip_to=rxx_setjump(pstate);
			}
    break;

  case 173:
/* Line 1792 of yacc.c  */
#line 1171 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrbreak;
			    if ((yyvsp[(1) - (2)].stack).type==REJECT)
				i->flags|=RT_HASREJECT;
			}
    break;

  case 174:
/* Line 1792 of yacc.c  */
#line 1180 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_aggrcont;
			    if ((yyvsp[(1) - (2)].stack).type==REJECT)
				i->flags|=RT_HASREJECT;
			}
    break;

  case 175:
/* Line 1792 of yacc.c  */
#line 1190 "csqpy.y"
    { (yyval.stack).type=0; }
    break;

  case 176:
/* Line 1792 of yacc.c  */
#line 1191 "csqpy.y"
    {
			    if (!pstate->ssp->stmt->reject)
				FAIL(RC_NRJCT)
			    (yyval.stack).type=REJECT;
			}
    break;

  case 178:
/* Line 1792 of yacc.c  */
#line 1198 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
    break;

  case 179:
/* Line 1792 of yacc.c  */
#line 1205 "csqpy.y"
    {
			    instruction_t *i;

			    if ((yyvsp[(2) - (2)].stack).count!=(yyvsp[(1) - (2)].fntok)->tl_parmcount && (yyvsp[(1) - (2)].fntok)->tl_parmcount>=0)
				FAIL(RC_WACNT)
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[(1) - (2)].fntok)->tl_func;
			}
    break;

  case 180:
/* Line 1792 of yacc.c  */
#line 1213 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			    i->flags|=RT_DITCHRES;
			}
    break;

  case 181:
/* Line 1792 of yacc.c  */
#line 1221 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign2;
			}
    break;

  case 182:
/* Line 1792 of yacc.c  */
#line 1229 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[(1) - (1)].instr), sizeof(instruction_t));
			}
    break;

  case 184:
/* Line 1792 of yacc.c  */
#line 1237 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
    break;

  case 185:
/* Line 1792 of yacc.c  */
#line 1244 "csqpy.y"
    {
			    pstate->parseroptions&=~FUNCHACK;
			}
    break;

  case 186:
/* Line 1792 of yacc.c  */
#line 1246 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[(2) - (5)].instr).func;
			    i->constant.type=(yyvsp[(2) - (5)].instr).constant.type;
			    i->constant.val.intvalue=
			        (yyvsp[(2) - (5)].instr).constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
    break;

  case 187:
/* Line 1792 of yacc.c  */
#line 1259 "csqpy.y"
    {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[(3) - (3)].instr).func;
			    i->constant.type=(yyvsp[(3) - (3)].instr).constant.type;
			    i->constant.val.intvalue=
			        (yyvsp[(3) - (3)].instr).constant.val.intvalue;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
		   }
    break;

  case 188:
/* Line 1792 of yacc.c  */
#line 1276 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_variable2;
			}
    break;

  case 189:
/* Line 1792 of yacc.c  */
#line 1282 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[(1) - (4)].instr).func;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			}
    break;

  case 190:
/* Line 1792 of yacc.c  */
#line 1292 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_assign;
			    i->flags=(yyvsp[(1) - (3)].instr).flags;
			}
    break;

  case 191:
/* Line 1792 of yacc.c  */
#line 1304 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 192:
/* Line 1792 of yacc.c  */
#line 1306 "csqpy.y"
    {
			    (yyval.instr).func=rxo_add;
			}
    break;

  case 193:
/* Line 1792 of yacc.c  */
#line 1309 "csqpy.y"
    {
			    (yyval.instr).func=rxo_sub;
			}
    break;

  case 194:
/* Line 1792 of yacc.c  */
#line 1312 "csqpy.y"
    {
			    (yyval.instr).func=rxo_mul;
			}
    break;

  case 195:
/* Line 1792 of yacc.c  */
#line 1315 "csqpy.y"
    {
			    (yyval.instr).func=rxo_div;
			}
    break;

  case 196:
/* Line 1792 of yacc.c  */
#line 1318 "csqpy.y"
    {
			    (yyval.instr).func=rxo_dpipe;
			}
    break;

  case 198:
/* Line 1792 of yacc.c  */
#line 1323 "csqpy.y"
    {
			    instruction_t *i;

			    if ((yyvsp[(1) - (2)].fntok)->tl_parmcount>=0)
				(yyvsp[(2) - (2)].insptr)->constant.val.intvalue=(yyvsp[(1) - (2)].fntok)->tl_parmcount;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=(yyvsp[(1) - (2)].fntok)->tl_func;
			}
    break;

  case 199:
/* Line 1792 of yacc.c  */
#line 1333 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			    i->constant.count=0;
			}
    break;

  case 200:
/* Line 1792 of yacc.c  */
#line 1341 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[(2) - (2)].stack).count;
			    i->constant.flags|=(yyvsp[(2) - (2)].stack).flags;
			}
    break;

  case 201:
/* Line 1792 of yacc.c  */
#line 1351 "csqpy.y"
    {
			    pstate->parseroptions|=RESVARHACK;
			}
    break;

  case 202:
/* Line 1792 of yacc.c  */
#line 1353 "csqpy.y"
    {
			    pstate->parseroptions&=~RESVARHACK;
			    (yyval.stack)=(yyvsp[(2) - (2)].stack);
			}
    break;

  case 203:
/* Line 1792 of yacc.c  */
#line 1358 "csqpy.y"
    {
			    (yyval.stack).count=(yyvsp[(1) - (3)].stack).count+1;
			    (yyval.stack).flags=(yyvsp[(1) - (3)].stack).flags|(yyvsp[(3) - (3)].stack).flags;
			}
    break;

  case 204:
/* Line 1792 of yacc.c  */
#line 1362 "csqpy.y"
    {
			    (yyval.stack).count=1;
			    (yyval.stack).flags=(yyvsp[(1) - (1)].stack).flags;
			}
    break;

  case 205:
/* Line 1792 of yacc.c  */
#line 1367 "csqpy.y"
    {
			    (yyval.stack).flags=0;
			}
    break;

  case 206:
/* Line 1792 of yacc.c  */
#line 1370 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 207:
/* Line 1792 of yacc.c  */
#line 1370 "csqpy.y"
    {
			    instruction_t *i;

			    pstate->parseroptions&=~FUNCHACK;
			    (yyval.stack).flags=0;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[(1) - (4)].instr), sizeof(instruction_t));
			    i->constant.totype=(yyvsp[(4) - (4)].instr).constant.totype;
			    i->constant.qual=(yyvsp[(4) - (4)].instr).constant.val.intvalue;
			}
    break;

  case 208:
/* Line 1792 of yacc.c  */
#line 1381 "csqpy.y"
    {
			    instruction_t *i;

			    (yyval.stack).flags=EF_VARIABLEDEST;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[(1) - (3)].instr), sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			    i->constant.flags|=EF_EXPANDHASH;
			}
    break;

  case 209:
/* Line 1792 of yacc.c  */
#line 1391 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    fgw_move(i, &(yyvsp[(1) - (3)].instr), sizeof(instruction_t));
			    i->flags|=RT_RESVAR;
			}
    break;

  case 210:
/* Line 1792 of yacc.c  */
#line 1398 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_identlist;
			    (yyval.stack).flags=EF_VARIABLELIST;
			}
    break;

  case 211:
/* Line 1792 of yacc.c  */
#line 1407 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[(1) - (1)].stack).count;
			    i->constant.flags=(yyvsp[(1) - (1)].stack).flags;
			    (yyval.stack).count=(yyvsp[(1) - (1)].stack).count;
			    (yyval.stack).flags=(yyvsp[(1) - (1)].stack).flags;
			}
    break;

  case 212:
/* Line 1792 of yacc.c  */
#line 1419 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_listcount;
			    i->constant.count=(yyvsp[(2) - (2)].stack).count;
			    i->constant.flags|=(yyvsp[(2) - (2)].stack).flags;
			}
    break;

  case 213:
/* Line 1792 of yacc.c  */
#line 1429 "csqpy.y"
    {
			    endstmt_sql();
			    if (rsx_nonredirected(&(yyvsp[(2) - (3)].pcode)))
				FAILCHECK;
			}
    break;

  case 214:
/* Line 1792 of yacc.c  */
#line 1434 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    i->func=rsx_redirect;
			}
    break;

  case 215:
/* Line 1792 of yacc.c  */
#line 1440 "csqpy.y"
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
    break;

  case 216:
/* Line 1792 of yacc.c  */
#line 1452 "csqpy.y"
    {
			    markend_sql((yyvsp[(1) - (1)].tstart)-1);
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM)
			    pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart);
			    pstate->ssp->stmt->options|=SO_INSCURS;
			    if (pstate->errorinfo)
				pstate->ssp->line_count=pstate->errorinfo->line_count;
			}
    break;

  case 218:
/* Line 1792 of yacc.c  */
#line 1462 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_executein;
			    markend_sql((yyvsp[(1) - (2)].tstart));
			    endstmt_sql();
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
    break;

  case 219:
/* Line 1792 of yacc.c  */
#line 1474 "csqpy.y"
    { (yyval.tstart)=(yyvsp[(1) - (1)].tstart); }
    break;

  case 220:
/* Line 1792 of yacc.c  */
#line 1475 "csqpy.y"
    { (yyval.tstart)=(yyvsp[(1) - (2)].tstart); }
    break;

  case 221:
/* Line 1792 of yacc.c  */
#line 1476 "csqpy.y"
    { (yyval.tstart)=(yyvsp[(1) - (1)].tstart); }
    break;

  case 223:
/* Line 1792 of yacc.c  */
#line 1479 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_autocommit;
			}
    break;

  case 224:
/* Line 1792 of yacc.c  */
#line 1487 "csqpy.y"
    {}
    break;

  case 225:
/* Line 1792 of yacc.c  */
#line 1488 "csqpy.y"
    {}
    break;

  case 226:
/* Line 1792 of yacc.c  */
#line 1489 "csqpy.y"
    {}
    break;

  case 227:
/* Line 1792 of yacc.c  */
#line 1491 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart); }
    break;

  case 228:
/* Line 1792 of yacc.c  */
#line 1492 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart); }
    break;

  case 229:
/* Line 1792 of yacc.c  */
#line 1493 "csqpy.y"
    { pstate->ssp->sqlstart=(yyvsp[(1) - (1)].tstart); }
    break;

  case 230:
/* Line 1792 of yacc.c  */
#line 1495 "csqpy.y"
    {
			    if (rqx_registerstatement(pstate, rqx_newstatement(pstate)))
				FAIL(RC_NOMEM);
			}
    break;

  case 232:
/* Line 1792 of yacc.c  */
#line 1501 "csqpy.y"
    {
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
	  }
    break;

  case 233:
/* Line 1792 of yacc.c  */
#line 1503 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_listcount;
                            i->constant.count=(yyvsp[(3) - (3)].stack).count;
			    i->constant.flags|=(yyvsp[(3) - (3)].stack).flags;
                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rxo_into;
			    rxx_swappcode(&pstate->pcode, &pstate->ssp->stmt->intovars);
			    markend_sql((yyvsp[(1) - (3)].tstart));
			}
    break;

  case 234:
/* Line 1792 of yacc.c  */
#line 1518 "csqpy.y"
    {
                            instruction_t *i;

                            i=rxx_newinstr(pstate);
                            FAILCHECK
                            i->func=rsx_curcon;
			}
    break;

  case 235:
/* Line 1792 of yacc.c  */
#line 1525 "csqpy.y"
    {
                            instruction_t *i;

			    markend_sql((yyvsp[(1) - (2)].tstart));
                            i=rxx_newinstr(pstate);
                            FAILCHECK
			    if ((yyvsp[(2) - (2)].stack).type==SOURCE)
			    {
				i->func=rsx_srcdefault;
				i->constant.type=CSTRINGTYPE;
				i->constant.val.string=(yyvsp[(2) - (2)].stack).val.string;
				i->constant.flags|=EF_NOTONTSS;
			    }
			    else if ((yyvsp[(2) - (2)].stack).type==CONNECTION)
				i->func=rsx_tmpcon;
			    else
				i->func=rsx_curdefault;
			}
    break;

  case 236:
/* Line 1792 of yacc.c  */
#line 1544 "csqpy.y"
    { pstate->parseroptions|=RESVARHACK; }
    break;

  case 237:
/* Line 1792 of yacc.c  */
#line 1545 "csqpy.y"
    { pstate->parseroptions&=~RESVARHACK; }
    break;

  case 238:
/* Line 1792 of yacc.c  */
#line 1546 "csqpy.y"
    { pstate->parseroptions|=AGGREGATEHACK; }
    break;

  case 239:
/* Line 1792 of yacc.c  */
#line 1547 "csqpy.y"
    { pstate->parseroptions&=~AGGREGATEHACK; }
    break;

  case 241:
/* Line 1792 of yacc.c  */
#line 1550 "csqpy.y"
    {
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
	  }
    break;

  case 242:
/* Line 1792 of yacc.c  */
#line 1553 "csqpy.y"
    {
			    markend_sql((yyvsp[(1) - (3)].tstart));
			    rxx_swappcode(&pstate->pcode,
					  &pstate->ssp->stmt->usingvars);
			}
    break;

  case 243:
/* Line 1792 of yacc.c  */
#line 1562 "csqpy.y"
    {
			    pstate->ssp->stmt->options|=SO_TABULAR;
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			}
    break;

  case 244:
/* Line 1792 of yacc.c  */
#line 1566 "csqpy.y"
    {
			    markend_sql((yyvsp[(1) - (2)].tstart));
			    pstate->ssp->stmt->width=pstate->width;
			    if (!(yyvsp[(2) - (2)].pcode).pcodehead)
				pstate->ssp->stmt->options|=SO_TABULAR;
			    memcpy(&(yyval.pcode), &(yyvsp[(2) - (2)].pcode), sizeof(pcode_t));
			}
    break;

  case 245:
/* Line 1792 of yacc.c  */
#line 1574 "csqpy.y"
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    pstate->ssp->stmt->fmt_type=FMT_BRIEF;
			}
    break;

  case 246:
/* Line 1792 of yacc.c  */
#line 1578 "csqpy.y"
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
    break;

  case 247:
/* Line 1792 of yacc.c  */
#line 1581 "csqpy.y"
    {
			    (yyval.pcode)=(yyvsp[(2) - (4)].pcode);
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
    break;

  case 248:
/* Line 1792 of yacc.c  */
#line 1585 "csqpy.y"
    {
			    memset(&(yyval.pcode), 0, sizeof(pcode_t));
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			}
    break;

  case 249:
/* Line 1792 of yacc.c  */
#line 1588 "csqpy.y"
    {
			    (yyval.pcode)=(yyvsp[(2) - (4)].pcode);
			    rxx_swappcode(&pstate->pcode, &(yyval.pcode));
			    pstate->ssp->stmt->fmt_type=FMT_DELIMITED;
			}
    break;

  case 250:
/* Line 1792 of yacc.c  */
#line 1594 "csqpy.y"
    { pstate->ssp->stmt->fmt_type=FMT_FULL; }
    break;

  case 251:
/* Line 1792 of yacc.c  */
#line 1595 "csqpy.y"
    { pstate->ssp->stmt->fmt_type=FMT_VERTICAL; }
    break;

  case 252:
/* Line 1792 of yacc.c  */
#line 1597 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    rxu_setnull(&i->constant, CINTTYPE);
		        }
    break;

  case 254:
/* Line 1792 of yacc.c  */
#line 1607 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_emptylist;
			}
    break;

  case 256:
/* Line 1792 of yacc.c  */
#line 1616 "csqpy.y"
    {
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    switch ((yyvsp[(1) - (1)].stack).type)
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
    break;

  case 257:
/* Line 1792 of yacc.c  */
#line 1638 "csqpy.y"
    {
			    (yyval.stack).type=(yyvsp[(2) - (2)].stack).type;
			}
    break;

  case 258:
/* Line 1792 of yacc.c  */
#line 1641 "csqpy.y"
    {
			    instruction_t *i;
	
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[(2) - (4)].var);
			    i->constant.flags|=EF_NOTONTSS;
			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rxo_constant;
			    memset(&(i->constant), 0, sizeof(exprstack_t));
			    i->constant.type=CSTRINGTYPE;
			    i->constant.val.string=(yyvsp[(4) - (4)].var);
			    i->constant.flags|=EF_NOTONTSS;
			    (yyval.stack).type=EXTRACTOR;
			}
    break;

  case 259:
/* Line 1792 of yacc.c  */
#line 1661 "csqpy.y"
    {
			    (yyval.stack).type=PATTERN;
			}
    break;

  case 260:
/* Line 1792 of yacc.c  */
#line 1664 "csqpy.y"
    {
			    (yyval.stack).type=DELIMITED;
			}
    break;

  case 261:
/* Line 1792 of yacc.c  */
#line 1667 "csqpy.y"
    {
			    (yyval.stack).type=~DELIMITED;
			}
    break;

  case 265:
/* Line 1792 of yacc.c  */
#line 1676 "csqpy.y"
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
    break;

  case 266:
/* Line 1792 of yacc.c  */
#line 1689 "csqpy.y"
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
    break;

  case 268:
/* Line 1792 of yacc.c  */
#line 1702 "csqpy.y"
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
    break;

  case 272:
/* Line 1792 of yacc.c  */
#line 1718 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectfile;
			}
    break;

  case 274:
/* Line 1792 of yacc.c  */
#line 1727 "csqpy.y"
    {
			    instruction_t *i;

			    i=rxx_newinstr(pstate);
			    FAILCHECK
			    i->func=rsx_rejectsize;
			}
    break;

  case 278:
/* Line 1792 of yacc.c  */
#line 1746 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}
    break;

  case 279:
/* Line 1792 of yacc.c  */
#line 1758 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyvsp[(1) - (2)].instr).func=rxo_variable;
				    (yyvsp[(1) - (2)].instr).flags=(yyvsp[(2) - (2)].instr).flags;
				    if (pstate->parseroptions & SAFECOPYHACK)
					(yyvsp[(1) - (2)].instr).flags|=RT_SAFECOPY;
				    fgw_move(i, &(yyvsp[(1) - (2)].instr), sizeof(instruction_t));
				}
    break;

  case 280:
/* Line 1792 of yacc.c  */
#line 1769 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
    break;

  case 281:
/* Line 1792 of yacc.c  */
#line 1776 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(1) - (4)].fntok)->tl_func;
				    i->constant.flags=EF_FUNCHASH;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_checkfunc;
				}
    break;

  case 282:
/* Line 1792 of yacc.c  */
#line 1790 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    i->func=(yyvsp[(2) - (2)].fntok)->tl_func;
				}
    break;

  case 283:
/* Line 1792 of yacc.c  */
#line 1799 "csqpy.y"
    {
		   parsestack_t p;

		   p.type=CASE;
		   fgw_stackpush(&pstate->parsestack, (char *) &p);
	       }
    break;

  case 284:
/* Line 1792 of yacc.c  */
#line 1804 "csqpy.y"
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
    break;

  case 285:
/* Line 1792 of yacc.c  */
#line 1819 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_dpipe;
				}
    break;

  case 286:
/* Line 1792 of yacc.c  */
#line 1826 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_picture;
				}
    break;

  case 287:
/* Line 1792 of yacc.c  */
#line 1833 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_add;
				}
    break;

  case 288:
/* Line 1792 of yacc.c  */
#line 1840 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_sub;
				}
    break;

  case 289:
/* Line 1792 of yacc.c  */
#line 1847 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_mul;
				}
    break;

  case 290:
/* Line 1792 of yacc.c  */
#line 1854 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_div;
				}
    break;

  case 291:
/* Line 1792 of yacc.c  */
#line 1861 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 292:
/* Line 1792 of yacc.c  */
#line 1861 "csqpy.y"
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_units;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=TU_IENCODE(5,
						(yyvsp[(4) - (4)].stack).type, (yyvsp[(4) - (4)].stack).type);
				}
    break;

  case 293:
/* Line 1792 of yacc.c  */
#line 1872 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 294:
/* Line 1792 of yacc.c  */
#line 1872 "csqpy.y"
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(4) - (4)].instr).func;
				    i->constant.type=(yyvsp[(4) - (4)].instr).constant.type;
				    i->constant.val.intvalue=
					(yyvsp[(4) - (4)].instr).constant.val.intvalue;
			}
    break;

  case 295:
/* Line 1792 of yacc.c  */
#line 1883 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 296:
/* Line 1792 of yacc.c  */
#line 1883 "csqpy.y"
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(6) - (7)].instr).func;
				    i->constant.type=(yyvsp[(6) - (7)].instr).constant.type;
				    i->constant.val.intvalue=
					(yyvsp[(6) - (7)].instr).constant.val.intvalue;
				}
    break;

  case 297:
/* Line 1792 of yacc.c  */
#line 1894 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_minus;
				}
    break;

  case 298:
/* Line 1792 of yacc.c  */
#line 1900 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    i->func=rxo_plus;
				}
    break;

  case 299:
/* Line 1792 of yacc.c  */
#line 1906 "csqpy.y"
    {
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    i->func=rxo_colvalue;
				}
    break;

  case 301:
/* Line 1792 of yacc.c  */
#line 1920 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    fgw_move(&i->constant, &(yyvsp[(1) - (1)].stack),
						sizeof(exprstack_t));
				    i->constant.flags|=EF_CONSTANT;
				    if ((yyvsp[(1) - (1)].stack).type==CSTRINGTYPE)
				    {
					fgw_tssdetach(&pstate->lexhead,
						      (yyvsp[(1) - (1)].stack).val.string);
					fgw_tssattach(&pstate->pcode.parsehead,
						      (yyvsp[(1) - (1)].stack).val.string);
				    }
				}
    break;

  case 302:
/* Line 1792 of yacc.c  */
#line 1937 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyvsp[(1) - (1)].instr).func=rxo_variable;
				    if (pstate->parseroptions & SAFECOPYHACK)
					(yyvsp[(1) - (1)].instr).flags|=RT_SAFECOPY;
				    fgw_move(i, &(yyvsp[(1) - (1)].instr), sizeof(instruction_t));
				}
    break;

  case 305:
/* Line 1792 of yacc.c  */
#line 1954 "csqpy.y"
    {
			    parsestack_t p;

			    p.type=IN;
			    fgw_stackpush(&pstate->parsestack, (char *) &p);
			}
    break;

  case 306:
/* Line 1792 of yacc.c  */
#line 1959 "csqpy.y"
    {
				    parsestack_t *p;
				    instruction_t *e;

				    e=rxx_setjump(pstate);
				    p=(parsestack_t *)
					fgw_stackpop(&pstate->parsestack);
				    p->instr->func=((yyvsp[(2) - (6)].stack).type==IN)?
							rxo_inend: rxo_niend;
				    for (;;)
				    {
					p=(parsestack_t *)
					    fgw_stackpop(&pstate->parsestack);
					if (p->type==IN)
					    break;
				        else if (p->type==~IN)
					{
					    p->instr->func=((yyvsp[(2) - (6)].stack).type==IN)?
							   rxo_in: rxo_ni;
					    p->instr->skip_to=e;
					}
				    }
				}
    break;

  case 307:
/* Line 1792 of yacc.c  */
#line 1982 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_between;
				    i->constant.type=((yyvsp[(2) - (5)].stack).type==BETWEEN);
				}
    break;

  case 308:
/* Line 1792 of yacc.c  */
#line 1990 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_matches;
				    i->constant.type=((yyvsp[(2) - (4)].stack).type==MATCHES);
				}
    break;

  case 309:
/* Line 1792 of yacc.c  */
#line 1998 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_like;
				    i->constant.type=((yyvsp[(2) - (4)].stack).type==LIKE);
				}
    break;

  case 310:
/* Line 1792 of yacc.c  */
#line 2006 "csqpy.y"
    {
#ifdef HAVE_REGEX
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_regexp;
				    i->constant.type=((yyvsp[(2) - (3)].stack).type==REGEXP);
#else
				    FAIL(RC_NLNKD)
#endif
				}
    break;

  case 311:
/* Line 1792 of yacc.c  */
#line 2018 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_or;
				}
    break;

  case 312:
/* Line 1792 of yacc.c  */
#line 2025 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_and;
				}
    break;

  case 313:
/* Line 1792 of yacc.c  */
#line 2032 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_not;
				}
    break;

  case 314:
/* Line 1792 of yacc.c  */
#line 2039 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_isnull;
				    i->constant.type=CINTTYPE;
				    i->constant.val.intvalue=((yyvsp[(3) - (3)].stack).type==NULLVALUE);
				}
    break;

  case 315:
/* Line 1792 of yacc.c  */
#line 2048 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_eq;
				}
    break;

  case 316:
/* Line 1792 of yacc.c  */
#line 2055 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ne;
				}
    break;

  case 317:
/* Line 1792 of yacc.c  */
#line 2062 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_le;
				}
    break;

  case 318:
/* Line 1792 of yacc.c  */
#line 2069 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_lt;
				}
    break;

  case 319:
/* Line 1792 of yacc.c  */
#line 2076 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_ge;
				}
    break;

  case 320:
/* Line 1792 of yacc.c  */
#line 2083 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_gt;
				}
    break;

  case 322:
/* Line 1792 of yacc.c  */
#line 2092 "csqpy.y"
    {
				    (yyval.instr).func=rxo_tointeger;
				    (yyval.instr).constant.totype=CINTTYPE;
				}
    break;

  case 323:
/* Line 1792 of yacc.c  */
#line 2096 "csqpy.y"
    {
				    (yyval.instr).func=rxo_todouble;
				    (yyval.instr).constant.totype=CFLOATTYPE;
				}
    break;

  case 324:
/* Line 1792 of yacc.c  */
#line 2100 "csqpy.y"
    {
				    (yyval.instr).func=rxo_todecimal;
				    (yyval.instr).constant.type=CDECIMALTYPE;
				    (yyval.instr).constant.totype=CDECIMALTYPE;
				}
    break;

  case 325:
/* Line 1792 of yacc.c  */
#line 2105 "csqpy.y"
    {
				    (yyval.instr).func=rxo_todecimal;
				    (yyval.instr).constant.type=CMONEYTYPE;
				    (yyval.instr).constant.totype=CMONEYTYPE;
				}
    break;

  case 326:
/* Line 1792 of yacc.c  */
#line 2110 "csqpy.y"
    {
				    (yyval.instr).func=rxo_todate;
				    (yyval.instr).constant.totype=CDATETYPE;
				}
    break;

  case 327:
/* Line 1792 of yacc.c  */
#line 2114 "csqpy.y"
    {
				    (yyval.instr).func=rxo_todatetime;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.totype=CDTIMETYPE;
				    (yyval.instr).constant.val.intvalue=(yyvsp[(2) - (2)].stack).val.intvalue;
				}
    break;

  case 328:
/* Line 1792 of yacc.c  */
#line 2120 "csqpy.y"
    {
				    (yyval.instr).func=rxo_tointerval;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.totype=CINVTYPE;
				    (yyval.instr).constant.val.intvalue=(yyvsp[(2) - (2)].stack).val.intvalue;
				}
    break;

  case 329:
/* Line 1792 of yacc.c  */
#line 2126 "csqpy.y"
    {
				    (yyval.instr).func=rxo_tostring;
				    (yyval.instr).constant.totype=CSTRINGTYPE;
				}
    break;

  case 330:
/* Line 1792 of yacc.c  */
#line 2130 "csqpy.y"
    {
				    (yyval.instr).func=rxo_tobyte;
				    (yyval.instr).constant.totype=FGWBYTETYPE;
				}
    break;

  case 331:
/* Line 1792 of yacc.c  */
#line 2135 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[(1) - (1)].instr), sizeof(instruction_t));
				}
    break;

  case 332:
/* Line 1792 of yacc.c  */
#line 2143 "csqpy.y"
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=(yyvsp[(2) - (2)].stack).type;
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[(2) - (2)].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[(2) - (2)].stack).flags;
				    if ((yyvsp[(2) - (2)].stack).count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[(1) - (2)].var), &d))
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
							       (yyvsp[(1) - (2)].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
							      (yyvsp[(1) - (2)].var), 32))
				    {
					if (e->type==T_HASH)
					     FAIL(RC_INHST)
					(yyval.instr).constant.val.identifier=e->magic;
				    }
				    else
					FAILCHECK
				    fgw_tssfree(&pstate->lexhead, (yyvsp[(1) - (2)].var));
				    (yyval.instr).flags=RT_RESVAR;
				}
    break;

  case 333:
/* Line 1792 of yacc.c  */
#line 2184 "csqpy.y"
    {
				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    (yyval.instr).func=rxo_column;
				}
    break;

  case 334:
/* Line 1792 of yacc.c  */
#line 2190 "csqpy.y"
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=(yyvsp[(2) - (2)].stack).type;
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[(2) - (2)].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[(2) - (2)].stack).flags;
				    if ((yyvsp[(2) - (2)].stack).count)
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[(1) - (2)].var), &d))
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
							   (yyvsp[(1) - (2)].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     (yyvsp[(1) - (2)].var), (pstate->parseroptions &
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
				    fgw_tssfree(&pstate->lexhead, (yyvsp[(1) - (2)].var));
				}
    break;

  case 335:
/* Line 1792 of yacc.c  */
#line 2234 "csqpy.y"
    {
				    fgw_hstentry *e;
				    int d;

				    memset(&(yyval.instr), 0, sizeof(instruction_t));
				    (yyval.instr).func=rxo_identifier;
				    (yyval.instr).flags|=((yyvsp[(2) - (2)].stack).type|RT_HASHVAR);
				    (yyval.instr).constant.type=FGWIDTYPE;
				    (yyval.instr).constant.count=(yyvsp[(2) - (2)].stack).count;
				    (yyval.instr).constant.flags=(yyvsp[(2) - (2)].stack).flags;
				    if ((yyvsp[(2) - (2)].stack).count || (pstate->parseroptions & RESVARHACK))
				    {
					if (e=fgw_hstget(pstate->vars, 0,
							 (yyvsp[(1) - (2)].var), &d))
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
							   (yyvsp[(1) - (2)].var), 8);
					    FAILCHECK
					}
				    }
				    else if (e=fgw_hstreserve(pstate->vars, 0,
					     (yyvsp[(1) - (2)].var), 0))
					(yyval.instr).constant.val.identifier=e->magic;
				    else
					(yyval.instr).constant.val.identifier=0;
				    fgw_tssfree(&pstate->lexhead, (yyvsp[(1) - (2)].var));
				}
    break;

  case 336:
/* Line 1792 of yacc.c  */
#line 2271 "csqpy.y"
    {
				    (yyval.stack).type=0;
				    (yyval.stack).count=0;
				}
    break;

  case 337:
/* Line 1792 of yacc.c  */
#line 2275 "csqpy.y"
    {
				    (yyval.stack).type=RT_HASHASH;
				    (yyval.stack).count=(yyvsp[(2) - (3)].stack).count;
				    (yyval.stack).flags=(yyvsp[(2) - (3)].stack).flags;
				}
    break;

  case 338:
/* Line 1792 of yacc.c  */
#line 2285 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_user;
				}
    break;

  case 339:
/* Line 1792 of yacc.c  */
#line 2298 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxf_wait;
				}
    break;

  case 340:
/* Line 1792 of yacc.c  */
#line 2311 "csqpy.y"
    {
				    instruction_t *i;

				    if (!(pstate->parseroptions & AGGREGATEHACK))
					FAILSYN
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_countstar;
				}
    break;

  case 341:
/* Line 1792 of yacc.c  */
#line 2320 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[(3) - (4)].instr), sizeof(instruction_t));
				    i->func=rxo_counthash;
				    i->flags|=RT_RESVAR;
				}
    break;

  case 342:
/* Line 1792 of yacc.c  */
#line 2329 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(3) - (4)].fntok)->tl_func;
				    i->constant.flags=EF_FUNCCOUNT;
				}
    break;

  case 343:
/* Line 1792 of yacc.c  */
#line 2337 "csqpy.y"
    {
				    instruction_t *i;
				    fgw_objtype *obj;

				    if (!(obj=rqx_getobject(pstate, (yyvsp[(1) - (4)].var), (yyvsp[(3) - (4)].var))))
					 FAIL(pstate->ca.sqlcode)
				    fgw_tssfree(&pstate->lexhead, (yyvsp[(1) - (4)].var));
				    fgw_tssfree(&pstate->lexhead, (yyvsp[(3) - (4)].var));
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=obj->obj;
				    i->src_id=obj->src_id;
				}
    break;

  case 344:
/* Line 1792 of yacc.c  */
#line 2350 "csqpy.y"
    {
				    instruction_t *i;

				    if ((yyvsp[(2) - (2)].insptr)->constant.count>0 && (yyvsp[(1) - (2)].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ((yyvsp[(1) - (2)].fntok)->tl_parmcount>0)
					(yyvsp[(2) - (2)].insptr)->constant.val.intvalue=(yyvsp[(1) - (2)].fntok)->tl_parmcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(1) - (2)].fntok)->tl_func;
				}
    break;

  case 345:
/* Line 1792 of yacc.c  */
#line 2361 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 346:
/* Line 1792 of yacc.c  */
#line 2361 "csqpy.y"
    {
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(3) - (3)].instr).func;
				    i->constant.type=(yyvsp[(3) - (3)].instr).constant.type;
				    i->constant.val=(yyvsp[(3) - (3)].instr).constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(1) - (3)].fntok)->tl_func;
				}
    break;

  case 347:
/* Line 1792 of yacc.c  */
#line 2386 "csqpy.y"
    { pstate->parseroptions|=FUNCHACK; }
    break;

  case 348:
/* Line 1792 of yacc.c  */
#line 2386 "csqpy.y"
    {
				    exprstack_t *s;
				    instruction_t *i;

				    pstate->parseroptions&=~FUNCHACK;
				    if ((yyvsp[(3) - (6)].stack).count>0 && (yyvsp[(1) - (6)].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(6) - (6)].instr).func;
				    i->constant.type=(yyvsp[(6) - (6)].instr).constant.type;
				    i->constant.val=(yyvsp[(6) - (6)].instr).constant.val;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_listcount;
				    i->constant.count=(yyvsp[(3) - (6)].stack).count+1;
				    i->constant.flags|=(yyvsp[(3) - (6)].stack).flags;
				    if ((yyvsp[(1) - (6)].fntok)->tl_parmcount>0)
					i->constant.val.intvalue=(yyvsp[(1) - (6)].fntok)->tl_parmcount+1;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(1) - (6)].fntok)->tl_func;
				}
    break;

  case 349:
/* Line 1792 of yacc.c  */
#line 2410 "csqpy.y"
    { (yyval.fntok)=(yyvsp[(1) - (1)].fntok); }
    break;

  case 350:
/* Line 1792 of yacc.c  */
#line 2411 "csqpy.y"
    { (yyval.fntok)=(yyvsp[(1) - (1)].fntok); }
    break;

  case 351:
/* Line 1792 of yacc.c  */
#line 2413 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				}
    break;

  case 352:
/* Line 1792 of yacc.c  */
#line 2421 "csqpy.y"
    {
				    (yyval.insptr)=rxx_newinstr(pstate);
				    FAILCHECK
				    (yyval.insptr)->func=rxo_emptylist;
				}
    break;

  case 353:
/* Line 1792 of yacc.c  */
#line 2426 "csqpy.y"
    {
				    (yyval.insptr)=(yyvsp[(2) - (3)].insptr);
				}
    break;

  case 354:
/* Line 1792 of yacc.c  */
#line 2430 "csqpy.y"
    {
				    (yyval.instr).flags=RT_HASSUB1;
				}
    break;

  case 355:
/* Line 1792 of yacc.c  */
#line 2433 "csqpy.y"
    {
				    (yyval.instr).flags=RT_HASSUB2;
				}
    break;

  case 356:
/* Line 1792 of yacc.c  */
#line 2437 "csqpy.y"
    {
				    (yyval.instr).func=rxo_constant;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.val.intvalue=-1;
				}
    break;

  case 357:
/* Line 1792 of yacc.c  */
#line 2442 "csqpy.y"
    {
				    if ((yyvsp[(1) - (3)].stack).type>(yyvsp[(3) - (3)].stack).type)
					FAIL(RC_IQUAL)
				    (yyval.instr).func=rxo_constant;
				    (yyval.instr).constant.type=CINTTYPE;
				    (yyval.instr).constant.val.intvalue=TU_DTENCODE((yyvsp[(1) - (3)].stack).type,
								     (yyvsp[(3) - (3)].stack).type);
				}
    break;

  case 358:
/* Line 1792 of yacc.c  */
#line 2451 "csqpy.y"
    {
				    (yyval.stack).type=CINTTYPE;
				    (yyval.stack).val.intvalue=-1;
				}
    break;

  case 359:
/* Line 1792 of yacc.c  */
#line 2455 "csqpy.y"
    {
				    if ((yyvsp[(1) - (3)].stack).type>(yyvsp[(3) - (3)].stack).type)
					FAIL(RC_IQUAL)
				    (yyval.stack).type=CINTTYPE;
				    (yyval.stack).val.intvalue=TU_DTENCODE((yyvsp[(1) - (3)].stack).type,
								(yyvsp[(3) - (3)].stack).type);
				}
    break;

  case 360:
/* Line 1792 of yacc.c  */
#line 2463 "csqpy.y"
    {
				    (yyval.stack)=(yyvsp[(1) - (1)].stack);
				}
    break;

  case 361:
/* Line 1792 of yacc.c  */
#line 2466 "csqpy.y"
    {
				    (yyval.stack).type=TU_F1;
				}
    break;

  case 362:
/* Line 1792 of yacc.c  */
#line 2470 "csqpy.y"
    {
				    (yyval.stack)=(yyvsp[(1) - (1)].stack);
				}
    break;

  case 363:
/* Line 1792 of yacc.c  */
#line 2473 "csqpy.y"
    {
				    (yyval.stack)=(yyvsp[(2) - (2)].stack);
				}
    break;

  case 364:
/* Line 1792 of yacc.c  */
#line 2477 "csqpy.y"
    {
				    (yyval.stack).type=TU_F5;
				}
    break;

  case 365:
/* Line 1792 of yacc.c  */
#line 2480 "csqpy.y"
    {
				    if ((yyvsp[(2) - (3)].stack).type!=CINTTYPE)
					FAILSYN;
				    switch((yyvsp[(2) - (3)].stack).val.intvalue)
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
    break;

  case 366:
/* Line 1792 of yacc.c  */
#line 2505 "csqpy.y"
    {
				    (yyval.stack).type=TU_YEAR;
				}
    break;

  case 367:
/* Line 1792 of yacc.c  */
#line 2508 "csqpy.y"
    {
				    (yyval.stack).type=TU_MONTH;
				}
    break;

  case 368:
/* Line 1792 of yacc.c  */
#line 2511 "csqpy.y"
    {
				    (yyval.stack).type=TU_DAY;
				}
    break;

  case 369:
/* Line 1792 of yacc.c  */
#line 2514 "csqpy.y"
    {
				    (yyval.stack).type=TU_HOUR;
				}
    break;

  case 370:
/* Line 1792 of yacc.c  */
#line 2517 "csqpy.y"
    {
				    (yyval.stack).type=TU_MINUTE;
				}
    break;

  case 371:
/* Line 1792 of yacc.c  */
#line 2520 "csqpy.y"
    {
				    (yyval.stack).type=TU_SECOND;
				}
    break;

  case 378:
/* Line 1792 of yacc.c  */
#line 2533 "csqpy.y"
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
    break;

  case 379:
/* Line 1792 of yacc.c  */
#line 2543 "csqpy.y"
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
    break;

  case 380:
/* Line 1792 of yacc.c  */
#line 2558 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_elseexpr;
				}
    break;

  case 382:
/* Line 1792 of yacc.c  */
#line 2566 "csqpy.y"
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
    break;

  case 383:
/* Line 1792 of yacc.c  */
#line 2576 "csqpy.y"
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
    break;

  case 385:
/* Line 1792 of yacc.c  */
#line 2592 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    rxu_setnull(&i->constant, CINTTYPE);
				}
    break;

  case 386:
/* Line 1792 of yacc.c  */
#line 2601 "csqpy.y"
    {
				    (yyval.stack).type=NULLVALUE;
				}
    break;

  case 387:
/* Line 1792 of yacc.c  */
#line 2604 "csqpy.y"
    {
				    (yyval.stack).type=~NULLVALUE;
				}
    break;

  case 390:
/* Line 1792 of yacc.c  */
#line 2611 "csqpy.y"
    {
			    (yyval.insptr)=rxx_newinstr(pstate);
			    FAILCHECK
			    (yyval.insptr)->func=rxo_listcount;
			    (yyval.insptr)->constant.count=(yyvsp[(1) - (1)].stack).count;
			    (yyval.insptr)->constant.flags=(yyvsp[(1) - (1)].stack).flags;
			}
    break;

  case 391:
/* Line 1792 of yacc.c  */
#line 2619 "csqpy.y"
    {
				    (yyval.stack).count=(yyvsp[(1) - (3)].stack).count+1;
				    (yyval.stack).flags=(yyvsp[(1) - (3)].stack).flags|(yyvsp[(3) - (3)].stack).flags;
				}
    break;

  case 392:
/* Line 1792 of yacc.c  */
#line 2623 "csqpy.y"
    {
				    (yyval.stack).count=1;
				    (yyval.stack).flags|=(yyvsp[(1) - (1)].stack).flags;
				}
    break;

  case 393:
/* Line 1792 of yacc.c  */
#line 2628 "csqpy.y"
    {
				    (yyval.stack).flags=0;
				}
    break;

  case 394:
/* Line 1792 of yacc.c  */
#line 2631 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=(yyvsp[(1) - (2)].fntok)->tl_func;
				    i->constant.flags|=(yyvsp[(2) - (2)].stack).flags;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
    break;

  case 395:
/* Line 1792 of yacc.c  */
#line 2640 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[(1) - (3)].instr), sizeof(instruction_t));
				    i->func=rxo_dohashstar;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
    break;

  case 396:
/* Line 1792 of yacc.c  */
#line 2651 "csqpy.y"
    {
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    fgw_move(i, &(yyvsp[(1) - (3)].instr), sizeof(instruction_t));
				}
    break;

  case 397:
/* Line 1792 of yacc.c  */
#line 2657 "csqpy.y"
    {
				    instruction_t *i;
	
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_varlist;
				    if (pstate->parseroptions & SAFECOPYHACK)
					 i->flags|=RT_SAFECOPY;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
    break;

  case 398:
/* Line 1792 of yacc.c  */
#line 2667 "csqpy.y"
    {
				    instruction_t *ic, *i;

				    ic=rxx_newinstr(pstate);
				    FAILCHECK
				    ic->func=rxo_listcount;
				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    switch ((yyvsp[(3) - (3)].stack).type)
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
    break;

  case 399:
/* Line 1792 of yacc.c  */
#line 2700 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_to;
				    (yyval.stack).flags=EF_VARIABLELIST;
				}
    break;

  case 400:
/* Line 1792 of yacc.c  */
#line 2709 "csqpy.y"
    {
				    if ((yyvsp[(2) - (2)].insptr)->constant.count && (yyvsp[(1) - (2)].fntok)->tl_parmcount==0)
					FAIL(RC_WACNT)
				    if ((yyvsp[(1) - (2)].fntok)->tl_parmcount>0)
					(yyvsp[(2) - (2)].insptr)->constant.val.intvalue=(yyvsp[(1) - (2)].fntok)->tl_parmcount;
				    (yyval.fntok)=(yyvsp[(1) - (2)].fntok);
				}
    break;

  case 401:
/* Line 1792 of yacc.c  */
#line 2716 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_emptylist;
				    (yyval.fntok)=(yyvsp[(1) - (1)].fntok);
				}
    break;

  case 402:
/* Line 1792 of yacc.c  */
#line 2725 "csqpy.y"
    {
				    (yyval.stack).flags=0;
				}
    break;

  case 403:
/* Line 1792 of yacc.c  */
#line 2728 "csqpy.y"
    {
				    (yyval.stack).flags=EF_FUNCHASH;
				}
    break;

  case 404:
/* Line 1792 of yacc.c  */
#line 2732 "csqpy.y"
    {
				    (yyval.stack).type=IN;
				}
    break;

  case 405:
/* Line 1792 of yacc.c  */
#line 2735 "csqpy.y"
    {
				    (yyval.stack).type=~IN;
				}
    break;

  case 406:
/* Line 1792 of yacc.c  */
#line 2739 "csqpy.y"
    {
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
    break;

  case 407:
/* Line 1792 of yacc.c  */
#line 2749 "csqpy.y"
    {
				    parsestack_t p;
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    p.type=~IN;
				    p.instr=i;
				    fgw_stackpush(&pstate->parsestack, (char *) &p);
				}
    break;

  case 408:
/* Line 1792 of yacc.c  */
#line 2760 "csqpy.y"
    {
				    (yyval.stack).type=BETWEEN;
				}
    break;

  case 409:
/* Line 1792 of yacc.c  */
#line 2763 "csqpy.y"
    {
				    (yyval.stack).type=~BETWEEN;
				}
    break;

  case 410:
/* Line 1792 of yacc.c  */
#line 2767 "csqpy.y"
    {
				    (yyval.stack).type=MATCHES;
				}
    break;

  case 411:
/* Line 1792 of yacc.c  */
#line 2770 "csqpy.y"
    {
				    (yyval.stack).type=~MATCHES;
				}
    break;

  case 412:
/* Line 1792 of yacc.c  */
#line 2774 "csqpy.y"
    {
				    (yyval.stack).type=LIKE;
				}
    break;

  case 413:
/* Line 1792 of yacc.c  */
#line 2777 "csqpy.y"
    {
				    (yyval.stack).type=~LIKE;
				}
    break;

  case 414:
/* Line 1792 of yacc.c  */
#line 2781 "csqpy.y"
    {
				    (yyval.stack).type=REGEXP;
				}
    break;

  case 415:
/* Line 1792 of yacc.c  */
#line 2784 "csqpy.y"
    {
				    (yyval.stack).type=~REGEXP;
				}
    break;

  case 417:
/* Line 1792 of yacc.c  */
#line 2789 "csqpy.y"
    {
				    instruction_t *i;

				    i=rxx_newinstr(pstate);
				    FAILCHECK
				    i->func=rxo_constant;
				    i->constant.type=CSTRINGTYPE;
				    i->constant.flags|=EF_NOTONTSS;
				    i->constant.val.string="\\";
				}
    break;

  case 418:
/* Line 1792 of yacc.c  */
#line 2806 "csqpy.y"
    {
				    if (yychar==TOK_ERROR)
					YYERROR;
				}
    break;

  case 419:
/* Line 1792 of yacc.c  */
#line 2813 "csqpy.y"
    {
				    (yyval.tstart)=pstate->tokstart;

/* hack alert - we want the parsing to terminate, but we don't want to accept yet
   hence we fool the parser into thinking that the input has ended */
				    yychar=0;
				    DRAIN_STMT;
				}
    break;


/* Line 1792 of yacc.c  */
#line 6494 "csqpy.c"
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

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
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

  /* Do not reclaim the symbols of the rule which action triggered
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
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

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
  /* Do not reclaim the symbols of the rule which action triggered
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
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}


/* Line 2055 of yacc.c  */
#line 2821 "csqpy.y"


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
