INCLUDES = \
	-I$(top_srcdir)/src \
	-I$(top_srcdir)/lib \
	-I$(top_srcdir)/src/zapit/include \
	-I$(top_srcdir)/lib/connection \
	-I$(top_srcdir)/lib/libeventserver \
	@DIRECTFB_CFLAGS@

noinst_LIBRARIES = libtriple.a

AM_CPPFLAGS = -fno-rtti -fno-exceptions

libtriple_a_SOURCES = \
	dmx_td.cpp \
	video_td.cpp \
	audio_td.cpp \
	init_td.cpp \
	playback_td.cpp \
	pwrmngr.cpp \
	record_td.cpp

#libtriple_a_LIBADD = $(top_builddir)/src/driver/libneutrino_driver.a

#	cool.cpp
#	dvb-ci.cpp \
#	dvbci_session.cpp \
#	dvbci_mmi.cpp \
#	dvbci_camgr.cpp \
#	dvbci_appmgr.cpp \
#	dvbci_datetimemgr.cpp \
#	dvbci_resmgr.cpp

