AM_CXXFLAGS = -fno-rtti -fno-exceptions

INCLUDES = \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/src/zapit/include \
	-I$(top_srcdir)/lib/connection \
	-I$(top_srcdir)/lib/libeventserver \
	-I$(top_srcdir)/lib/libnet \
	-I$(top_srcdir)/lib/libconfigfile \
	-I$(top_srcdir)/lib/libmd5sum \
	-I$(top_srcdir)/lib/xmltree \
	@FREETYPE_CFLAGS@

if BOXTYPE_COOL
INCLUDES += -I$(top_srcdir)/lib/libcoolstream
endif
if BOXTYPE_TRIPLE
INCLUDES += -I$(top_srcdir)/lib/libtriple
endif

noinst_LIBRARIES = libneutrino_system.a

libneutrino_system_a_SOURCES = \
	localize.cpp setting_helpers.cpp debug.cpp \
	ping.c flashtool.cpp httptool.cpp \
	settings.cpp lastchannel.cpp \
	configure_network.cpp fsmounter.cpp
