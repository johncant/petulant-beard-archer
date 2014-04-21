#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

#LT_INIT([dlopen])
#LT_CONFIG_LTDL_DIR([libltdl])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_INSTALL

# Checks for libraries.
#PKG_CHECK_MODULES(GTKMM, [gtkmm-3.0])
PKG_CHECK_MODULES(GTKMM, [gtkmm-2.4])
PKG_CHECK_MODULES(gl, [gl]) # TODO - check version.
PKG_CHECK_MODULES(glew, [glew]) # TODO - check version.
PKG_CHECK_MODULES(opencv, [opencv]) # TODO - check version.
AX_BOOST_BASE([1.55],,AC_MSG_ERROR([Could not find boost 1.55 or greater]))
AX_BOOST_UNIT_TEST_FRAMEWORK

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
#LTDL_INIT([subproject])
AC_OUTPUT
