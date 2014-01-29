ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-gtk-doc

if GLADEUI_FOUND
SUBDIRS_GLADE = glade
else
SUBDIRS_GLADE =
endif
SUBDIRS = src tests docs $(SUBDIRS_GLADE)

EXTRA_DIST = libgtkdateentry.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libgtkdateentry.pc

distclean-local:
	if test "$(srcdir)" = "."; then :; else \
		rm -f ChangeLog; \
	fi

if PLATFORM_WIN32
install-data-hook:
	\cp -f -r $(prefix)/* /mingw
endif

ChangeLog:
	@echo Creating $@
	@if test -d "$(srcdir)/.git"; then \
	 (GIT_DIR=$(top_srcdir)/.git ./missing --run git log --stat -M -C --name-status --date=short --no-color) | fmt --split-only > $@.tmp \
	 && mv -f $@.tmp $@ \
	 || ($(RM) $@.tmp; \
		echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
		 (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
		test -f $@ || \
		 (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
		echo A git checkout and git-log is required to generate this file >> $@); \
	fi

.PHONY: ChangeLog
