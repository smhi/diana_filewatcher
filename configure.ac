#-----------------------------------------------------------------------------
# Pre-Conf
#-----------------------------------------------------------------------------

AC_PREREQ(2.59)

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------
AC_INIT([filewatcher], [1.1.0])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([1.11 foreign parallel-tests])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

#-----------------------------------------------------------------------------
# Checks
#-----------------------------------------------------------------------------

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h sys/time.h])
AC_CHECK_HEADERS([string vector sstream fstream])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

METNO_PROG_PKG_CONFIG
METNO_REQUIRE_QT4([ 
	Core
	Gui
	Network
 ])
METNO_REQUIRE_PKG([MILOGGER], [miLogger], [5.0.3])
METNO_REQUIRE_PKG([PUTOOLS], [puTools], [5.1.4])
METNO_REQUIRE_PKG([QUTILITIES], [qUtilities], [5.1.4])
METNO_REQUIRE_PKG([COSERVER], [coserver], [2.0.4])



AC_OUTPUT(Makefile src/Makefile)
