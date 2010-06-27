AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(GTKDATEENTRY_CFLAGS) \
              $(GTKFORM_CFLAGS)

LIBS = $(GTKDATEENTRY_LIBS) \
       $(GTKFORM_LIBS)

if LIBGTKFORM_FOUND
GTKFORM_C = gtkformwidgetdateentry.c
GTKFORM_H = gtkformwidgetdateentry.h
libgtkformmodulesdir = `pkg-config --variable=modulesdir libgtkform`
else
GTKFORM_C =
GTKFORM_H =
libgtkformmodulesdir =
endif

lib_LTLIBRARIES = libgtkdateentry.la

libgtkdateentry_la_SOURCES = gtkdateentry.c \
                             $(GTKFORM_C)

libgtkdateentry_la_LDFLAGS = -no-undefined

include_HEADERS = gtkdateentry.h \
                  $(GTKFORM_H)

install-exec-hook:
	if test "x$(LIBGTKFORM_FOUND)" = "xyes"; then \
		cd $(libgtkformmodulesdir) && \
		ln -s -f $(libdir)/libgtkdateentry.so .; \
	fi

uninstall-hook:
	if test "x$(LIBGTKFORM_FOUND)" = "xyes"; then \
		rm -f $(libgtkformmodulesdir)/libgtkdateentry.so
	fi
