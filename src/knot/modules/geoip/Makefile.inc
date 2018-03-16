knot_modules_geoip_la_SOURCES = knot/modules/geoip/geoip.c
EXTRA_DIST +=                     knot/modules/geoip/geoip.rst

if STATIC_MODULE_geoip
libknotd_la_SOURCES += $(knot_modules_geoip_la_SOURCES)
endif

if SHARED_MODULE_geoip
knot_modules_geoip_la_LDFLAGS = $(KNOTD_MOD_LDFLAGS)
knot_modules_geoip_la_CPPFLAGS = $(KNOTD_MOD_CPPFLAGS)
pkglib_LTLIBRARIES += knot/modules/geoip.la
endif
