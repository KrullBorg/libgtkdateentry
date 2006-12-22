INCLUDES = $(WARN_CFLAGS) \
           $(DISABLE_DEPRECATED_CFLAGS) \
           $(GTK_CFLAGS) \
           $(GTKMASKEDENTRY_CFLAGS)

LIBS = $(GTK_LIBS) \
       $(GTKMASKEDENTRY_LIBS)

lib_LTLIBRARIES = libgtkdateentry.la

libgtkdateentry_la_SOURCES = gtkdateentry.c

libgtkdateentry_la_LDFLAGS = -no-undefined

include_HEADERS = gtkdateentry.h
