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

if PLATFORM_WIN32
libmodulesext = -0.dll
libmodulesdir = $(bindir)
else
libmodulesext = .so
libmodulesdir = $(libdir)
endif

lib_LTLIBRARIES = libgtkdateentry.la

libgtkdateentry_la_SOURCES = gtkdateentry.c \
                             $(GTKFORM_C)

libgtkdateentry_la_LDFLAGS = -no-undefined

include_HEADERS = gtkdateentry.h \
                  $(GTKFORM_H)

if LIBGTKFORM_FOUND
install-exec-hook:
	cd $(libgtkformmodulesdir) && \
		ln -s -f $(libmodulesdir)/libgtkdateentry$(libmodulesext) .
endif

if LIBGTKFORM_FOUND
uninstall-hook:
	rm -f $(libgtkformmodulesdir)/libgtkdateentry$(libmodulesext)
endif
