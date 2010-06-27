AC_PREREQ(2.59)
AC_INIT([GtkDateEntry], [0.0.3], [azagli@libero.it])
AC_CONFIG_SRCDIR([src/gtkdateentry.c])
AC_CONFIG_HEADER([config.h])

AM_INIT_AUTOMAKE

AM_MAINTAINER_MODE

AC_CANONICAL_SYSTEM

AC_LIBTOOL_WIN32_DLL

AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

GTK_DOC_CHECK(1.0)

PKG_CHECK_MODULES(GTKDATEENTRY, [gtk+-2.0 >= 2.6.8
                                 libgtkmaskedentry >= 0.0.1])
PKG_CHECK_EXISTS(gladeui-1.0 >= 3.2.0, [GLADEUI_FOUND=yes], [GLADEUI_FOUND=no])

AC_SUBST(GTKDATEENTRY_CFLAGS)
AC_SUBST(GTKDATEENTRY_LIBS)

# Output files
AC_CONFIG_FILES([
  libgtkdateentry.pc
  Makefile 
  src/Makefile
  tests/Makefile
  docs/Makefile
  docs/reference/Makefile
  docs/reference/version.xml
])

if test $GLADEUI_FOUND = yes; then
AC_CONFIG_FILES([
  glade/Makefile
  glade/icons/Makefile
  glade/icons/16x16/Makefile
  glade/icons/22x22/Makefile
])
fi

AC_OUTPUT
