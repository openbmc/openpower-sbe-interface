
include_HEADERS = \
			 sbe_interfaces.hpp
				

libsbeinterfacedir = ${libdir}/sbe-interface
libsbeinterface_LTLIBRARIES = libsbeinterface.la

libsbeinterface_la_SOURCES = \
	sbe_interfaces.cpp

libsbeinterface_la_LDFLAGS = $(SYSTEMD_LIBS) $(libmapper_LIBS) -version-info 0:0:0 -shared
libsbeinterface_la_CXXFLAGS = $(SYSTEMD_CFLAGS) $(libmapper_CFLAGS)

	

