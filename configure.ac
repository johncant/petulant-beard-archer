#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([FULL-PACKAGE-NAME], [VERSION], [BUG-REPORT-ADDRESS])
AC_CONFIG_SRCDIR([src])
AC_CONFIG_HEADERS([config.h])
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
PKG_CHECK_MODULES(gl, [gl >= 3.2]) # TODO - check version.

# Lower limit on xcb-glx is a result of glXCreateContextAttribsARB
PKG_CHECK_MODULES(glx, [xcb-glx >= 1.4])
PKG_CHECK_MODULES(glew, [glew]) # TODO - check version.
PKG_CHECK_MODULES(opencv, [opencv >= 2.0]) # TODO - check version.

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Checks for library functions.

AC_CONFIG_FILES([Makefile])
#LTDL_INIT([subproject])
AC_OUTPUT
