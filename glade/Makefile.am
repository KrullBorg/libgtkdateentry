SUBDIRS = icons

moduledir = `pkg-config --variable=moduledir gladeui-1.0`

catalogsdir = `pkg-config --variable=catalogdir gladeui-1.0`

catalogs_DATA = gtkdateentry.xml

EXTRA_DIST = $(catalogs_DATA)

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
