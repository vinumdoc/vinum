AC_INIT([vinum], [1.0])

AC_PROG_AWK
AC_CONFIG_AUX_DIR([build-aux])
AC_REQUIRE_AUX_FILE([tap-driver.sh])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

CFLAGS=""
AC_PROG_CC

AC_PROG_YACC
AC_PROG_LEX(noyywrap)

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
 Makefile
 src/Makefile
 src/tests/Makefile
])
AC_OUTPUT
