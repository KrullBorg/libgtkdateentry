SUBDIRS = icons

glademodulesdir = `pkg-config --variable=moduledir gladeui-1.0`

gladecatalogsdir = `pkg-config --variable=catalogdir gladeui-1.0`

gladecatalogs_DATA = gtkdateentry.xml

EXTRA_DIST = $(gladecatalogs_DATA)

if PLATFORM_WIN32
libmodulesext = -0.dll
libmodulesdir = $(bindir)
else
libmodulesext = .so
libmodulesdir = $(libdir)
endif

install-exec-hook:
	cd $(glademodulesdir) && \
	ln -s -f $(libmodulesdir)/libgtkdateentry$(libmodulesext) .

uninstall-hook:
	rm -f $(glademodulesdir)/libgtkdateentry$(libmodulesext)
