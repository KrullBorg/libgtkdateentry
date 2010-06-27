AM_CPPFLAGS = $(WARN_CFLAGS) \
              $(DISABLE_DEPRECATED_CFLAGS) \
              $(GTKDATEENTRY_CFLAGS) \
              $(GTKFORM_CFLAGS) \
              -I$(top_srcdir)/src

LIBS = $(GTKDATEENTRY_LIBS) \
        $(GTKFORM_LIBS)

LDADD = $(top_builddir)/src/libgtkdateentry.la

if LIBGTKFORM_FOUND
GTKFORM_NOINST = plugin_gtkform
else
GTKFORM_NOINST = 
endif

noinst_PROGRAMS = date_entry \
	$(GTKFORM_NOINST)

plugin_gtkform_SOURCES = plugin_gtkform.c
