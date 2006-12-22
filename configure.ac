AC_PREREQ(2.59)
AC_INIT([GtkDateEntry], [0.0.2], [azagli@inwind.it])
AC_CONFIG_SRCDIR([src/gtkdateentry.c])

AM_INIT_AUTOMAKE

AM_MAINTAINER_MODE

AC_CANONICAL_SYSTEM

AC_LIBTOOL_WIN32_DLL

AC_PROG_CC
AC_PROG_LIBTOOL

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_STRUCT_TM

GTK_DOC_CHECK(1.0)

GTK_REQUIRED=2.6.8
GTKMASKEDENTRY_REQUIRED=0.0.1

dnl Check for pkgconfig first
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, yes, no)

dnl Give error and exit if we don't have pkgconfig
if test "x$HAVE_PKGCONFIG" = "xno"; then
  AC_MSG_ERROR(you need to have pkgconfig installed !)
fi

PKG_CHECK_MODULES(GTK, gtk+-2.0 >= $GTK_REQUIRED)
PKG_CHECK_MODULES(GTKMASKEDENTRY, libgtkmaskedentry >= $GTKMASKEDENTRY_REQUIRED)

AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

AC_SUBST(GTKMASKEDENTRY_CFLAGS)
AC_SUBST(GTKMASKEDENTRY_LIBS)

# Output files
AC_CONFIG_FILES([
  libgtkdateentry.pc
  Makefile 
  src/Makefile
  tests/Makefile
  docs/Makefile
  docs/reference/Makefile
])
AC_OUTPUT
