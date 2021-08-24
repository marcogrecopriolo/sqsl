dnl	CONFIGURE.ac - Master autoconf defs
dnl	
dnl	The Structured Query Scripting Language
dnl	Copyright (C) 1992-2020 Marco Greco (marco@4glworks.com)
dnl	
dnl	Initial release: Feb 05
dnl	Current release: Jun 20
dnl
dnl	This library is free software; you can redistribute it and/or
dnl	modify it under the terms of the GNU Lesser General Public
dnl	License as published by the Free Software Foundation; either
dnl	version 2.1 of the License, or (at your option) any later version.
dnl	
dnl	This library is distributed in the hope that it will be useful,
dnl	but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
dnl	Lesser General Public License for more details.
dnl	
dnl	You should have received a copy of the GNU Lesser General Public
dnl	License along with this library; if not, write to the Free Software
dnl	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

AC_INIT(sqsl, 0.14)
AC_CONFIG_AUX_DIR(./utils)
m4_include([m4/ax_have_qt.m4])
AM_INIT_AUTOMAKE
AC_SUBST(PACKAGE_DATE, esyscmd(date "+\"%Y-%m-%d %H:%M:%S %z\""))
AC_PREFIX_DEFAULT(`pwd`/distbin/sqsl)

#
# need a local tmp
#
mkdir tmp 2>&-

# AC_PROG_AWK	# apparently already done by AM_INIT_AUTOMAKE

# since we pass arguments starting with - to fglpp
# we need to understand if we have to use -- (gawk, for instance)
# not to have it confused!
awkver=`$AWK --version 2>&- | grep GNU`
if test -n "$awkver"
then
    AC_SUBST(AWKDEL, "--")
fi

#
# Determine the operating system
#
AC_MSG_CHECKING(operating system)
OS=`uname -s`
AC_MSG_RESULT($OS)

#
# and the c compiler (AC_PROG_CC doesn't cut it I'm afraid)
#
AC_CHECK_PROGS(CC, ${INFORMIXC:+"$INFORMIXC"} ${CC:+"$CC"} cc gcc, no)
if test "$CC" = "no"
then
    AC_ERROR("a c compiler is required")
fi
#
# we need to make sure we have an ANSI compiler (AC_PROG_CC_STDC()
# does way more thab that
#
AC_MSG_CHECKING(for an ANSI C compiler)
echo 'int main(int argc, char **argv){}' > /tmp/ansi$$.c
if test -n "`$CC /tmp/ansi$$.c -o /tmp/ansi$$ 2>&1`"
then
    AC_ERROR("an ANSI C compiler is required")
else
    AC_MSG_RESULT(done)
fi
rm /tmp/ansi$$*

AC_CHECK_SIZEOF(long)

#
# now based on the operating system and c compiler in use, set a whole bunch
# of things
#
case "$OS" in
  Windows_NT)
    AC_SUBST(CC_SO_FLAG, "-W/dll")
    AC_SUBST(DNT, "-DNT -DMVS")
    AC_SUBST(DLLEXT, ".dll")
    AC_SUBST(OSLIBS, 'libsqslos.a')
    AC_SUBST(OSLIBADD, '$(top_srcdir)/oslib/coslc.obj')
    AC_ERROR("MKS currently not supported")
    ;;
  CYGWIN*)
    AC_SUBST(CC_SO_FLAG, "-shared")
    AC_SUBST(DLLEXT, ".dll")
    AC_ARG_WITH(cygwin,
	        [  --with-cygwin			uses the native cygwin API,
  --without-cygwin		uses the native windows API (default) ],
		[
#
# Run this if -with or -without was specified
#
		    if test "$withval" != "no" 
		    then
			if test "$withval" != "dynamic"
			then
			    use_cygwin=static
			else
			    use_cygwin=dynamic
			fi
		    else
			use_cygwin=no
		    fi
		],
		[
#
# nothing in the comamand line means no
#
		    use_cygwin=no
		])

    case "$use_cygwin" in
      static)
	AC_SUBST(DNT, "-DNT -DCYGSTATIC")
	AC_SUBST(CC_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(LD_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(OSLIBS, 'libsqslos.a')
	AC_SUBST(OSLIBADD, '$(top_srcdir)/oslib/coslc.o')
	;;

      dynamic)
#
#  if you are perusing configure and found this, be aware that this has been
#  put together for testing purposes, and chances are that your child
#  processes will segv right after the fork() call
#
	AC_SUBST(DNT, "-DNT -DCYGDYNAMIC")
	AC_SUBST(CC_EXTRA, "-mno-cygwin")
	AC_SUBST(LD_EXTRA, "-mno-cygwin -Wl[,]--wrap=main")
	AC_SUBST(CC_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(LD_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(OSLIBD, 'libsqslos$(DLLEXT)')
	AC_SUBST(OSLINK, '$(top_srcdir)/oslib/libsqslos$(DLLEXT)')
	cygstatic="no"
	;;

      *)
	AC_SUBST(DNT, "-DNT")
	AC_SUBST(CC_EXTRA, "-mno-cygwin")
	AC_SUBST(LD_EXTRA, "-mno-cygwin -Wl[,]--wrap=main")
	AC_SUBST(CC_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(LD_SRC_EXTRA, "-mno-cygwin")
	AC_SUBST(CC_OSL_EXTRA, "-mno-cygwin")
	AC_SUBST(OSLIBS, 'libsqslos.a')
	AC_SUBST(OSLIBADD, '$(top_srcdir)/oslib/coslc.o')
	cygstatic="no"
    esac
    ;;
  *)
    case "$OS" in
      HP-UX)
	AC_SUBST(CC_SO_FLAG, "-b")
	cc_reloc="+Z"
	cc_static="-noshared"
	;;
      AIX)
	AC_SUBST(CC_SO_FLAG, "-G")
	cc_xtra="-qlanglvl=ansi"
	cc_static="-bstatic"
	;;
      SunOS)
	AC_SUBST(CC_SO_FLAG, "-G")
	cc_reloc="-xcode=pic32"
	cc_xtra="-Xa"
	cc_static="-B static"
	;;
      *)
	AC_SUBST(CC_SO_FLAG, "-G")
	;;
    esac
#
# we need to be able to export symbols to dynamically loaded shared objects
# ac_cv_c_compiler_gnu is unreliable, so we do it our own way
# to top it off, not all compilers return non zero on invalid options
# hence the stderr hack...
#
    echo "int main(){}" > /tmp/dyn$$.c
    if test -z "`$CC -rdynamic /tmp/dyn$$.c -o /tmp/dyn$$ 2>&1`"
    then
	AC_SUBST(CC_LD_FLAG, "-rdynamic")
	AC_SUBST(CC_SO_FLAG, "-shared")
	AC_SUBST(CC_RELOC, "-fPIC")
	cc_static="-static"
	noimpl="-Wno-implicit-function-declaration"
    else
	AC_SUBST(CC, "$CC $cc_xtra")
	AC_SUBST(CC_RELOC, "$cc_reloc")
    fi
    echo "int main() { dlerrno(); }" > /tmp/dyn$$.c
    if test -z "`$CC -ldl /tmp/dyn$$.c -o /tmp/dyn$$ 2>&1`"
    then
	AC_SUBST(CC_OSL_EXTRA, "-DHAVE_DLERRNO")
    fi
    rm /tmp/dyn$$*
    AC_SUBST(OSLIBS, 'libsqslos.a')
    AC_SUBST(OSLIBADD, '$(top_srcdir)/oslib/coslc.o')
    AC_SUBST(DLLEXT, ".so")
    ;;
esac

#...and pass it along to esql/c4gl
AC_SUBST(INFORMIXC, "$CC $cc_xtra")

# where platforms don't export symbols to dynamically loaded shared objects
# (read, NT, what else?), we use functional pointers
if test -n "$DNT"
then
    AC_SUBST(DNEED_XSL, "-DNEED_XSL")
fi

#
# static linking required?
#
AC_ARG_ENABLE(static-link, 
  [  --enable-static-link		build statically linked binaries
  --disable-static-link		(default) build dynamically linked binaries ],
  [ if test "$enableval" = "full"
    then
	AC_SUBST(CC_LD_STATIC, "$cc_static")
    fi
    if test "$enableval" != "no"
    then
	AC_SUBST(ESQL_STATIC, -static)
    fi ])

#
# look for socket stuff
#
if test -z "$DNT"
then
    old_libs=$LIBS
    AC_SUBST(LIBS, "")
    AC_SEARCH_LIBS(socket, ['socket' 'net'])
    AC_SEARCH_LIBS(gethostbyname, ['nsl'])
    AC_SUBST(SOCKETLIBS, $LIBS)
    AC_SUBST(LIBS, $old_libs)
    AC_SUBST(NETSO, net)
else
    AC_SUBST(SOCKETLIBS, '-lwsock32')
    AC_SUBST(NETSO, net)
fi

#
# check for readline library + header files
# this will do an automatic AC_PROG_CC and other things. Pity I'll have to
# undo some
# FIXME this is crying out for macros
#
# first, try user hints
#
AC_ARG_WITH(readline,
  [  --with-readline=<lib_specs>	location and name of readline library
  --without-readline		disable readline detection ],
  [
#
# Run this if -with or -without was specified
#
    if test "$withval" != "no"
    then
	if test "$withval" != "yes"
	then
	    AC_CACHE_CHECK([for a readline compatible library],
				ac_cv_lib_readline, [
				ORIG_LIBS=$LIBS
				LIBS="$CC_EXTRA $ORIG_LIBS $withval"
				ORIG_LDFLAGS=$LDFLAGS
				LDFLAGS="$CC_LD_STATIC"
				AC_TRY_LINK_FUNC(readline,
						ac_cv_lib_readline=$withval)
				AC_TRY_LINK_FUNC(add_history,
						ac_cv_lib_add_history="yes")
				LIBS="$ORIG_LIBS"
				LDFLAGS="$ORIG_LDFLAGS"
				if test -n "$ac_cv_lib_readline" -a \
					-n "$ac_cv_lib_add_history"
				then
				    AC_SUBST(READLINE_DEFINE, -DREADLINE_LIBRARY)
				    AC_SUBST(READLINE_LIBRARY, "$withval")
				else
				    ac_cv_lib_readline="no"
				fi
			      ])
	else
	    READLINE_WANTED=yes
	fi
    else
	 READLINE_WANTED=no
    fi
  ], [
#
# no suggestion means go look for it
#
    if test "$cygstatic" != "no"
    then
	READLINE_WANTED=yes
    fi
])
if test "$READLINE_WANTED" = "yes"
then
#
# if not specified by caller, search in standard places
#
    AC_CACHE_CHECK([for a readline compatible library],
			ac_cv_lib_readline, [
			ORIG_LIBS=$LIBS
			ORIG_LDFLAGS=$LDFLAGS
			for readline_lib in readline edit editline
			do
			    for termcap_lib in "" termcap curses ncurses
			    do
				if test -z "$termcap_lib"
				then
				    TRY_LIB="-l$readline_lib"
				else
				    TRY_LIB="-l$readline_lib -l$termcap_lib"
				fi
				LIBS="$ORIG_LIBS $TRY_LIB"
				LDFLAGS="$CC_LD_STATIC"
				AC_TRY_LINK_FUNC(readline,
						ac_cv_lib_readline=$TRY_LIB)
				AC_TRY_LINK_FUNC(add_history,
						ac_cv_lib_add_history="yes")
				LIBS="$ORIG_LIBS"
				LDFLAGS="$ORIG_LDFLAGS"
				if test -n "$ac_cv_lib_readline" -a \
					-n "$ac_cv_lib_add_history"
				then
				    READLINE_LIBRARY="$TRY_LIB"
				    break
				fi
			    done
			    if test -n "$ac_cv_lib_readline"
			    then
				break
			    fi
			done
			if test -z "$ac_cv_lib_readline"
			then
			    ac_cv_lib_readline="no"
			fi
		      ])
#
# skip headers check - we use readline at a very basic levels, and really
# can't be bothered to go through all sort of hoops to understand where they
# are...
#
    if test "$ac_cv_lib_readline" != "no"
    then
	AC_SUBST(READLINE_DEFINE, -DREADLINE_LIBRARY)
	AC_SUBST(READLINE_LIBRARY)
    fi
fi

#
# Check for regex
# FIXME ditto
#
# first, try user hints
#
AC_ARG_WITH(regex,
  [  --with-regex=<lib_specs>	location and name of regex library
  --without-regex		disable regex detection ],
  [
#
# Run this if -with or -without was specified
#
    if test "$withval" != "no"
    then
	if test "$withval" != "yes"
	then
	    AC_CACHE_CHECK([for a regex compatible library],
				ac_cv_lib_regex, [
				ORIG_LIBS=$LIBS
				LIBS="$CC_EXTRA $ORIG_LIBS $withval"
				ORIG_LDFLAGS=$LDFLAGS
				LDFLAGS="$CC_LD_STATIC"
				ac_cv_lib_regex="no"
				AC_TRY_LINK_FUNC(regexec,
						ac_cv_lib_regex=$withval)
				LIBS="$ORIG_LIBS"
				LDFLAGS="$ORIG_LDFLAGS"
#
# look for regex.h
# FIXME ditto
#
				ac_cv_regex_h="no"
				if test `expr substr "$ac_cv_lib_regex" 1 1` \
					!= "/"
				then
				    ac_rh_dir=/usr
				else
				    ac_rh_dir=`dirname "$ac_cv_lib_regex"`
				fi
				while test "$ac_rh_dir" != "/" -a \
				           "$ac_rh_dir" != "."
				do
				    if test -f "$ac_rh_dir"/include/regex.h
				    then
					ac_cv_regex_h="$ac_rh_dir"/include
					break
				    fi
				    ac_rh_dir=`dirname "$ac_rh_dir"`
				done
				if test "$ac_cv_lib_regex" != "no" -a \
					"$ac_cv_regex_h" != "no"
				then
				    AC_SUBST(REGEX_DEFINE,
					     "-DHAVE_REGEX -I$ac_cv_regex_h")
				    AC_SUBST(REGEX_LIBRARY, "$withval")
				else
				    ac_cv_lib_regex="no"
				fi
			      ])
	else
	    REGEX_WANTED=yes
	fi
    else
	 REGEX_WANTED=no
    fi
  ], [
#
# no suggestion means go look for it
#
    if test "$cygstatic" != "no"
    then
	REGEX_WANTED=yes
    fi
])
if test "$REGEX_WANTED" = "yes"
then
#
# if not specified by caller, search in standard places
#
    AC_CACHE_CHECK([for a regex compatible library],
			ac_cv_lib_regex, [
			ORIG_LIBS=$LIBS
			ORIG_LDFLAGS=$LDFLAGS
			for regex_lib in ""
			do
			    if test -z "$regex_lib"
			    then
				TRY_LIB=""
			    else
				TRY_LIB="-l$regex_lib"
			    fi
			    LIBS="$ORIG_LIBS $TRY_LIB"
			    LDFLAGS="$CC_LD_STATIC"
			    ac_cv_lib_regex="no"
			    AC_TRY_LINK_FUNC(regexec,
					     ac_cv_lib_regex="yes")
			    LIBS="$ORIG_LIBS"
			    LDFLAGS="$ORIG_LDFLAGS"
			    if test -n "$ac_cv_lib_regex"
			    then
#
# look for regex.h
# FIXME ditto
#
				ac_cv_regex_h="no"
				if test `expr substr "$ac_cv_lib_regex" 1 1` \
					!= "/"
				then
				    ac_rh_dir=/usr
				else
				    ac_rh_dir=`dirname "$ac_cv_lib_regex"`
				fi
				while test "$ac_rh_dir" != "/" -a \
				           "$ac_rh_dir" != "."
				do
				    if test -f "$ac_rh_dir"/include/regex.h
				    then
					ac_cv_regex_h="$ac_rh_dir"/include
					break
				    ac_rh_dir=`dirname "$ac_rh_dir"`
				    fi
				done
				REGEX_LIBRARY="$TRY_LIB"
				break
			    fi
			done
			if test "$ac_cv_lib_regex" == "no" -o \
				"$ac_cv_regex_h" == "no"
			then
			    ac_cv_lib_regex="no"
			fi
		      ])
    if test "$ac_cv_lib_regex" != "no"
    then
	AC_SUBST(REGEX_DEFINE, "-DHAVE_REGEX -I$ac_cv_regex_h")
	AC_SUBST(REXEC_LIBRARY)
	AC_SUBST(REGEXTEST, regex.sqsl)
    fi
fi

#
# AC_PROG_CC sets a whole bunch of flags which throw several spanners in the
# works, so undo some of AC_PROG_CC work's and start anew
#
AC_SUBST(CC, $INFORMIXC)

AC_PROG_RANLIB
#
# check for bison, fallback on yacc (AC_PROG_YACC just doesn't cut it)
#
AC_CHECK_PROG(YACCCMP, bison, yes)
if test "$YACCCMP" = "yes"
then
    AC_SUBST(YACCCMP, 'bison -b $* -p $* -o $*.c $<')
else
    AC_CHECK_PROG(YACCCMP, byacc, yes)
    if test "$YACCCMP" = "yes"
    then
	AC_SUBST(YACCCMP, 'byacc -b $* -p $* -o $*.c $<')
    else
	AC_CHECK_PROG(YACCCMP, yacc, yes)
	if test "$YACCCMP" = "yes"
	then
	    cat <<! > /tmp/t$$.y
%token T
%pure-parser
%%

grammar: T {}
;
!
	    if test -z "`yacc /tmp/t$$.y 2>&1`"
	    then
		AC_SUBST(YACCCMP, 'yacc $<; mv y.tab.c $*.c')
	    else
		AC_SUBST(YACCCMP, '${top_srcdir}/tools/yacc $< $*.c')
		AC_MSG_WARN(This yacc does not generate pure parsers
			    Please check the readme file)
	    fi
	    rm y.tab.c /tmp/t$$.y 2>&1
	else
	    AC_MSG_ERROR(One of bison or yacc is required to build sqsl)
	fi
    fi
fi
#
# this is to keep automake quiet
#
AC_SUBST(YACC, yacc)

#
# need at least one between Informix esql/c and Informix 4gl compiled
# FIXME some esql/c die if awk terminates early
# FIXME we rely on esql / c4gl to link dl
#
AC_CHECK_PROG(ESQLVER, esql,
  `esql -V | $AWK '/Version/ { $0=$NF; FS="."; $0=$0; print $1 $2 }'`)
AC_CHECK_PROG(I4GLVER, c4gl,
  `c4gl -V | $AWK '/Version/ { $0=$NF; FS="."; $0=$0; print $1 $2 }'`)
if test -z "$ESQLVER" -a -z "$I4GLVER"
then
    AC_MSG_ERROR(One of Informix 4gl or esql is required to build sqsl)

dnl FIXME c4gl and esql can't handle -D defines with embedded spaces
elif test -z "$ESQLVER"
then
    AC_SUBST(ESQL, "c4gl -DI4GLVER=$I4GLVER")
    AC_SUBST(ESQLINC, $INFORMIXDIR/incl/tools)
    if test "$I4GLVER" -lt 600
    then
	AC_SUBST(IFMXSO, ifmx4)
    else
	AC_SUBST(IFMXSO, ifmx)
    fi
    AC_SUBST(IFMXTEST, ifmx.sqsl)
else
    AC_SUBST(ESQLDEFS, "-DESQLVER=$ESQLVER")
    case "$OS" in
      CYGWIN*)
	AC_SUBST(ESQL, '${top_srcdir}/tools/esqlcyg -DESQLVER='"$ESQLVER")
#
# it's a hack, but it allows to keep things simple
#
	esqlinc=`cygpath -u "$INFORMIXDIR/incl/esql"`
	AC_SUBST(ESQLINC, "\"$esqlinc\" -D\"__int64=long long\"")
	;;
      *)
	AC_SUBST(ESQL, "esql -DESQLVER=$ESQLVER")
	AC_SUBST(ESQLINC, $INFORMIXDIR/incl/esql)
	;;
    esac
    AC_SUBST(IFMXSO, ifmx)
    AC_SUBST(IFMXTEST, ifmx.sqsl)
fi
if test -n "$I4GLVER"
then
    AC_SUBST(C4GL, "c4gl -DI4GLVER=$I4GLVER")
    AC_SUBST(I4GLUI, "i4gl i4gldemo")
    if test $I4GLVER -gt 499
    then
	AC_SUBST(SQ_freecurs, free)
    else
	AC_SUBST(SQ_freecurs, "--free")
    fi
    mkdir tmp 2>&-
fi

#
# look for Aubit 4gl
#
AC_CHECK_PROG(A4GLVER, 4glpc,
  `4glpc --version | $AWK '/Version/ { print $2 }'`)
if test -n "$A4GLVER"
then
    if test -z "$ESQLVER"
    then
	AC_MSG_ERROR(The Aubit 4gl client requires Informix esql)
    fi
    case "$OS" in
      CYGWIN*)
	AC_SUBST(AUBITINCLDIR, `cygpath -u $AUBITDIR`/incl)
	AC_SUBST(A4GL_EC_COMP, 'sh ${top_srcdir}/tools/esqlcyg')
#
# MinGW sets the default stack @ 2M, and this triggers all sorts of unexpected
# segvs. 10M much better.
#
	AC_SUBST(A4GL_LINK,
		'sh ${top_srcdir}/tools/esqlcyg -Wl[,]--stack=10485760')
	AC_SUBST(A4GL_SETTINGSDIR, '\dummy')
	if test "$cygstatic" != "no"
	then
	    AC_MSG_WARN(Aubit is not supported on native cygwin ports)
	else
	    AC_SUBST(A4GLUI, "aubit4gl aubitdemo")
	fi
	;;
      *)
	AC_SUBST(A4GL_SETTINGSDIR, "$AUBITDIR/tools/4glpc/settings")
	AC_SUBST(AUBITINCLDIR, $AUBITDIR/incl)
	AC_SUBST(A4GLUI, "aubit4gl aubitdemo")
	;;
    esac
    AC_SUBST(A4GL, "4glpc -DA4GL -K")
    AC_SUBST(A4GL_SQLTYPE, "esql")
    AC_SUBST(A4GL_LEXTYPE, "EC")
    AC_SUBST(A4GL_LEXDIALECT, "INFORMIX")
fi
#
# (AUBEXT needs to be set anyhow even if Aubit is not installed, or EXEEXT
#  messes up make dist)
#
# EXEEXT is a pain, if you have executable having different extensions in
# different places (That's a requirement of Aubit, BTW)
# It'd be cool if automake would be willing to allow the redefinition of
# output variables, in makefiles. Instead, it issues a warning and forces
# the value pre redefinition.
# Or I could live with AC_UNSUBST'ing an output variable. Not a feature of
# Configure.
# Or maybe you could have multiple AC_OUTPUTs, and re-AC_SUBST output variables
# in between AC_OUTPUT invocations. Oh! only one invocation, at the end of the
# file.
# Well, I'll take matters into my own hands then...
# This is a bit poor, but it does the job.
# 
    if test -z "$DNT"
    then
	AC_SUBST(AUBEXT, ".4ae")
    else
	AC_SUBST(AUBEXT, ".exe")
    fi
#
# check for qt
#
    AX_HAVE_QT
    if test "$have_qt" = yes
    then
	AC_SUBST(QTDEMO, qtdemo)
    fi
#
# look for DB2
#
AC_CHECK_PROG(DB2VER, db2,
	     `db2level | $AWK '{ 
				for (i=1; i<=NF; i++)
				    if ($i ~/\"*SQL[[[0-9]]]+\"*$/)
				    {
					gsub("\\\"", "", $i);
					gsub("SQL", "", $i);
					printf "%6d"[,] $i; exit
				    }
				}'`)
#	     `db2level | $AWK '{ 
#				for (i=1; i<=NF; i++)
#				    if ($i ~/\"*SQL[[[0-9]]]+\"*$/)
#					{ gsub("\\"", "", $i);
#					  gsub("SQL", "", $i);
#					  print $i; exit }
#				}'`)
if test -n "$DB2VER"
then
#
# DB2PATH seems to be the default location on NT
#
    ac_sql_lib_dir=$DB2PATH
#
# if not, try DB2DIR, INSTHOME and the instance owner home directory from
# the passwd file
#
    if test -z "$ac_sql_lib_dir"
    then
	ac_sql_lib_dir=${DB2DIR:-$INSTHOME/sqllib}
    fi
    if test "$ac_sql_lib_dir" = "/sqllib" 
    then
	ac_sql_lib_dir=`$AWK -F ":" "/$DB2INSTANCE/ { print \\$6; exit }" \
	    /etc/passwd`/sqllib
    fi
    if test "$ac_sql_lib_dir" = "/sqllib" 
    then
	ac_sql_lib_dir=`ypcat passwd | $AWK -F ":" "/$DB2INSTANCE/ \
			{ print \\$6; exit }" `/sqllib
    fi
    if test "$ac_sql_lib_dir" = "/sqllib" 
    then
	$ECHO "Cannot determine DB2 instance directory"
    else
	db2so="db2cli"
	if test "$DB2VER" -ge 8000
	then
	    db2so="$db2so db2sqc"
	elif test -z "$DNT" -o "$cygstatic" != "no"
	then
	    db2so="$db2so db2sqc"
	    AC_SUBST(SQC_CC_EXTRA, '-DSQC_NEEDSFORK')
	    AC_SUBST(SQC_LD_EXTRA, '$(OSLIBADD) $(OSLINK)')
	fi
	if test "$ESQLVER" -ge 290 -a "$ESQLVER" -lt 350
	then
	    db2so="$db2so db2esql"
	fi
	AC_SUBST(DB2SO, "$db2so")

dnl FIXME, need to check if can prep
dnl FIXME, need to check for (BINDADD and IMPLICIT_SCHEMA) or DBADM authority
	case "$OS" in
	  CYGWIN*)
	    AC_SUBST(SQLLIBDIR, `cygpath -u "$ac_sql_lib_dir"`)
	    AC_SUBST(DB2LINK, '$(top_srcdir)/tools/ldcyg')
	    AC_SUBST(DB2EXTRA, '$(top_srcdir)/libdb2.a')
	    ;;
	  *)
	    AC_SUBST(SQLLIBDIR, $ac_sql_lib_dir)
	    AC_SUBST(DB2LINK, '$(CC) $(CC_SO_FLAG) -L$(SQLLIBDIR)/lib -ldb2')
	    ;;
	esac
#	AC_SUBST(DB2IFMXINCL, "-I$ESQLINC")	#FIXME
	AC_SUBST(DB2IFMXINCL, "-DFGWSOURCE")	#FIXME
	AC_SUBST(DB2BUILDDB, sample)		#FIXME
    fi
fi
    
#
# look for solid
#
AC_CHECK_PROG(SOLIDVER, solsql, `solsql -x | $AWK '{ print $NF; exit }'`)
if test -n "$SOLIDVER"
then
    case "$OS" in
      CYGWIN*)
	soliddir=`cygpath -u "$(which solsql)"`
	soliddir=`dirname "$soliddir"`

dnl that's right, twice!
	AC_SUBST(SOLIDDIR, $(dirname "$soliddir"))
	AC_SUBST(SOLIDLINK, '$(top_srcdir)/tools/ldcyg')
	AC_SUBST(SOLIDEXTRA, '$(top_srcdir)/solidimpodbcu.a')
	AC_SUBST(SOLIDDLL, `ls "$SOLIDDIR"/bin/soc*.dll 2>&-`)
	soldir=`cygpath -w "$SOLIDDIR"`
	echo "set SOLDIR=$soldir" > tools/mvs/solid.bat
	cat tools/mvs/solid.bat.in >> tools/mvs/solid.bat
	;;
      Windows_NT)
	AC_SUBST(SOLIDDEFS, "-DSS_NT")
	;;
      *)
	AC_SUBST(SOLIDDIR, $(dirname "$(dirname ""$(which solsql)"")"))
	AC_SUBST(SOLIDLINK, '$(CC) $(CC_SO_FLAG) -lpthread')
	solidso=`ls $SOLIDDIR/lib/libsocl*.so 2>&-`
	if test -n $solidso
	then
	    AC_SUBST(SOLIDEXTRA, $solidso)
	else
	    AC_SUBST(SOLIDEXTRA, '$(SOLIDDIR)/lib/solidodbcu.a')
	fi
	if test $ac_cv_sizeof_long -eq 8
	then
	    AC_SUBST(SOLIDDEFS, "-DUNIX_64BIT")
	fi
	;;
    esac
    AC_SUBST(SOLIDSO, solid)
fi

# look for libcouchbase
AC_MSG_CHECKING(for a couchbase library)

# there seems to be nothing wrong with linking the couchbase driver with DLLs, even on windows
# for now we only support standard locations
cb=`find /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /usr/bin /usr/local/bin -name 'libcouchbase*' -print 2>&-`
echo 'int main(int argc, char **argv){int i=lcb_n1ql_query(); }' > /tmp/cb$$.c
for l in $cb
do
    res=`$CC $noimpl $l /tmp/cb$$.c -o /tmp/cb$$ 2>&1`
    if test -z "$res"
    then
	AC_SUBST(CBSO, cb)
	AC_SUBST(CBLIBDIR, `dirname $l`)
	AC_SUBST(CBINCLDIR, `dirname $CBLIBDIR`/include)
	break
    fi
done
if test -z $CBSO
then
    AC_MSG_RESULT(no)
else
    AC_MSG_RESULT($CBLIBDIR)
fi
rm /tmp/cb$$*


# Here's the thing - i4gl and aubit4gl share source files with sqsl
# and it would be nice that changes to one side get reflected to
# the others, but
# - git doesn't do hard links, hence we need symlinks
# - automake does not like links at all: for starters it calls tar
# with the -h flag, and even playing with the am__tar* macros doesn't
# really help as make dist under the covers uses the distdir target,
# which builds a new wholy dereference installation source for make dist
# to play with.
# There's a rationale to this - the target OS may not support symlinks at
# all, and having straight files is a much safer bet.
# Hence, enter the hack - we list all our links in a nice space delimited
# here document and have configure reinstate them.
# We probably shouldn't include this in the configure script, but I think
# this is better than distributing an extra file, in that we don't have to
# check for file format correctness.
# amend as necessary

# older Aubit stuff (messages and forms) seems unhappy to deal with links
# on windows, so linking is not enabled on windows.
# Tough!
case "$OS" in
  CYGWIN*)
    ;;
  Windows_NT)
    ;;
  *)
    AC_MSG_CHECKING(needed symlinks)
    result="done"
    while read -u 3 d s
    do
	if test ! -h $s
	then
	    mv $s $s.old
	    success=`ln -s $d $s`

	    # if it didn't work out, reinstate and give up
	    if test -n "$success"
	    then
		mv $s.old $s
		result="linking error"
		break
	    else
		rm $s.old
	    fi
	fi
    done 3<<!
m4_include([linkslist])
!
    AC_MSG_RESULT($result)
    ;;
esac

AC_OUTPUT(Makefile cli/Makefile sqsl/Makefile
	  i4gldemo/Makefile i4gl/Makefile 
	  aubitdemo/Makefile aubit4gl/Makefile
	  qtdemo/Makefile
	  ifmx/Makefile ifmx4/Makefile
	  db2sqc/Makefile db2cli/Makefile db2esql/Makefile
	  solid/Makefile net/Makefile
	  cb/Makefile json/Makefile
	  oslib/Makefile doc/Makefile demo/Makefile
	  test/Makefile
	  tools/Makefile tools/mvs/Makefile
)
