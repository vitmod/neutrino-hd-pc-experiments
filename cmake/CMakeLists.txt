cmake_minimum_required(VERSION 2.6)
# configure is so 90s... :)
# let's try something new
# concentrate all things build in here


project(neutrino-hd-pc)

set(TOPDIR ${CMAKE_SOURCE_DIR}/..)


set(LIBTUXBOX_CONNECTION_SOURCES
    ${TOPDIR}/lib/connection/basicsocket.cpp
    ${TOPDIR}/lib/connection/basicmessage.h
    ${TOPDIR}/lib/connection/basicclient.cpp
    ${TOPDIR}/lib/connection/basicsocket.h
    ${TOPDIR}/lib/connection/basicserver.cpp
    ${TOPDIR}/lib/connection/basicserver.h
    ${TOPDIR}/lib/connection/basicclient.h
    ${TOPDIR}/lib/connection/messagetools.cpp
    ${TOPDIR}/lib/connection/messagetools.h
)


set(LIBCONTROLDCLIENT_SOURCES
    ${TOPDIR}/lib/controldclient/controldMsg.h
    ${TOPDIR}/lib/controldclient/controldclient.h
    ${TOPDIR}/lib/controldclient/controldtypes.h
    ${TOPDIR}/lib/controldclient/controldclient.cpp
)


set(LIBTUXBOX_CONFIGFILE_SOURCES
    ${TOPDIR}/lib/libconfigfile/configfile.h
    ${TOPDIR}/lib/libconfigfile/configfile.cpp
)


set(LIBCOOLSTREAM_SOURCES
    ${TOPDIR}/lib/libcoolstream/dmx_cs.h
    ${TOPDIR}/lib/libcoolstream/dvb-ci.h
    ${TOPDIR}/lib/libcoolstream/record_cs.h
    ${TOPDIR}/lib/libcoolstream/pwrmngr.h
    ${TOPDIR}/lib/libcoolstream/init_cs.h
    ${TOPDIR}/lib/libcoolstream/video_cs.h
    ${TOPDIR}/lib/libcoolstream/playback.h
    ${TOPDIR}/lib/libcoolstream/cs_types.h
    ${TOPDIR}/lib/libcoolstream/playback_cs.h
    ${TOPDIR}/lib/libcoolstream/audio_cs.h
)


### libdvbapi
set(LIBDVBAPI_SOURCES
    ${TOPDIR}/lib/libdvbapi/init_cs.h
    ${TOPDIR}/lib/libdvbapi/audio_dvbapi.h
    ${TOPDIR}/lib/libdvbapi/audio_dvbapi.cpp
    ${TOPDIR}/lib/libdvbapi/video_dvbapi.h
    ${TOPDIR}/lib/libdvbapi/video_dvbapi.cpp
    ${TOPDIR}/lib/libdvbapi/playback_dvbapi.h
    ${TOPDIR}/lib/libdvbapi/playback_dvbapi.cpp
    ${TOPDIR}/lib/libdvbapi/record_dvbapi.h
    ${TOPDIR}/lib/libdvbapi/record_dvbapi.cpp
    ${TOPDIR}/lib/libdvbapi/dmx_dvbapi.h
    ${TOPDIR}/lib/libdvbapi/dmx_dvbapi.cpp
)


set(LIBDVBSUB_SOURCES
    ${TOPDIR}/lib/libdvbsub/dvbsub.h
    ${TOPDIR}/lib/libdvbsub/dvbsub.cpp
    ${TOPDIR}/lib/libdvbsub/dvbsubtitle.h
    ${TOPDIR}/lib/libdvbsub/dvbsubtitle.cpp
    ${TOPDIR}/lib/libdvbsub/tools.h
    ${TOPDIR}/lib/libdvbsub/tools.cpp
    ${TOPDIR}/lib/libdvbsub/PacketQueue.hpp
    ${TOPDIR}/lib/libdvbsub/PacketQueue.cpp
    ${TOPDIR}/lib/libdvbsub/helpers.hpp
    ${TOPDIR}/lib/libdvbsub/helpers.cpp
    ${TOPDIR}/lib/libdvbsub/Debug.hpp
    ${TOPDIR}/lib/libdvbsub/Debug.cpp
)

set(LIBTUXBOX_EVENTSERVER_SOURCES
    ${TOPDIR}/lib/libeventserver/eventserver.h
    ${TOPDIR}/lib/libeventserver/eventserver.cpp
)

set(LIBTUXBOX_MD5SUM_SOURCES
    ${TOPDIR}/lib/libmd5sum/md5.h
    ${TOPDIR}/lib/libmd5sum/getline.h
    ${TOPDIR}/lib/libmd5sum/sys2.h
    ${TOPDIR}/lib/libmd5sum/libmd5sum.h
    ${TOPDIR}/lib/libmd5sum/system.h
    ${TOPDIR}/lib/libmd5sum/libmd5sum.c
    ${TOPDIR}/lib/libmd5sum/md5.c
)


set(LIBTUXBOX_NET_SOURCES
    ${TOPDIR}/lib/libnet/libnet.c
    ${TOPDIR}/lib/libnet/libnet.h
    ${TOPDIR}/lib/libnet/network_interfaces.cpp
    ${TOPDIR}/lib/libnet/network_interfaces.h
)


set(LIBTRIPLE_SOURCES
    ${TOPDIR}/lib/libtriple/dvbci_session.cpp
    ${TOPDIR}/lib/libtriple/dvbci_camgr.h
    ${TOPDIR}/lib/libtriple/init_td.cpp
    ${TOPDIR}/lib/libtriple/audio_td.cpp
    ${TOPDIR}/lib/libtriple/dvb-ci.h
    ${TOPDIR}/lib/libtriple/dvbci_mmi.h
    ${TOPDIR}/lib/libtriple/dmx_td.h
    ${TOPDIR}/lib/libtriple/ringbuffer.h
    ${TOPDIR}/lib/libtriple/record_td.cpp
    ${TOPDIR}/lib/libtriple/dmx_td.cpp
    ${TOPDIR}/lib/libtriple/dvbci_mmi.cpp
    ${TOPDIR}/lib/libtriple/dvbci_resmgr.cpp
    ${TOPDIR}/lib/libtriple/dvbci_camgr.cpp
    ${TOPDIR}/lib/libtriple/pwrmngr.h
    ${TOPDIR}/lib/libtriple/pwrmngr.cpp
    ${TOPDIR}/lib/libtriple/playback_td.h
    ${TOPDIR}/lib/libtriple/dvbci_session.h
    ${TOPDIR}/lib/libtriple/init_cs.h
    ${TOPDIR}/lib/libtriple/audio_td.h
    ${TOPDIR}/lib/libtriple/dvbci_datetimemgr.cpp
    ${TOPDIR}/lib/libtriple/ringbuffer.c
    ${TOPDIR}/lib/libtriple/td-compat/td-value-compat.h
    ${TOPDIR}/lib/libtriple/td-compat/td-audio-compat.h
    ${TOPDIR}/lib/libtriple/td-compat/td-video-compat.h
    ${TOPDIR}/lib/libtriple/td-compat/td-demux-compat.h
    ${TOPDIR}/lib/libtriple/td-compat/td-frontend-compat.h
    ${TOPDIR}/lib/libtriple/playback.h
    ${TOPDIR}/lib/libtriple/record_td.h
    ${TOPDIR}/lib/libtriple/cool.cpp
    ${TOPDIR}/lib/libtriple/dvbci_appmgr.h
    ${TOPDIR}/lib/libtriple/dvbci_datetimemgr.h
    ${TOPDIR}/lib/libtriple/dvbci_appmgr.cpp
    ${TOPDIR}/lib/libtriple/dvbci_resmgr.h
    ${TOPDIR}/lib/libtriple/playback_td.cpp
    ${TOPDIR}/lib/libtriple/video_td.h
    ${TOPDIR}/lib/libtriple/video_td.cpp
    ${TOPDIR}/lib/libtriple/init_td.h
    ${TOPDIR}/lib/libtriple/dvb-ci.cpp
)


set(LIBTUXTXT_SOURCES
    ${TOPDIR}/lib/libtuxtxt/tuxtxt_common.h
    ${TOPDIR}/lib/libtuxtxt/teletext.h
    ${TOPDIR}/lib/libtuxtxt/libtuxtxt.cpp
    ${TOPDIR}/lib/libtuxtxt/tuxtxt_def.h
    ${TOPDIR}/lib/libtuxtxt/tuxtxt.cpp
    ${TOPDIR}/lib/libtuxtxt/tuxtxt.h
    ${TOPDIR}/lib/libtuxtxt/tuxtxt2.conf
)


set(LIBUPNPCLIENT_SOURCES
    ${TOPDIR}/lib/libupnpclient/upnpclient.h
    ${TOPDIR}/lib/libupnpclient/UPNPSocket.cpp
    ${TOPDIR}/lib/libupnpclient/UPNPDevice.cpp
    ${TOPDIR}/lib/libupnpclient/UPNPService.cpp
)


set(SECTIONSDCLIENT_SOURCES
    ${TOPDIR}/lib/sectionsdclient/sectionsdtypes.h
    ${TOPDIR}/lib/sectionsdclient/sectionsdMsg.h
    ${TOPDIR}/lib/sectionsdclient/sectionsdclient.h
    ${TOPDIR}/lib/sectionsdclient/sectionsdclient.cpp
)


set(SECTIONSDCONTROL_SOURCES
    ${TOPDIR}/lib/sectionsdclient/sectionsdcontrol.cpp
)


set(TIMERDCLIENT_SOURCES
    ${TOPDIR}/lib/timerdclient/timerdmsg.h
    ${TOPDIR}/lib/timerdclient/timerdtypes.h
    ${TOPDIR}/lib/timerdclient/timerdclient.cpp
    ${TOPDIR}/lib/timerdclient/timerdclient.h
)

set(LIBTUXBOX_XMLTREE_SOURCES
    ${TOPDIR}/lib/xmltree/hashtab.h
    ${TOPDIR}/lib/xmltree/hashtab.c
    ${TOPDIR}/lib/xmltree/xmlparse.h
    ${TOPDIR}/lib/xmltree/xmlparse.cpp
    ${TOPDIR}/lib/xmltree/xmlrole.h
    ${TOPDIR}/lib/xmltree/xmlrole.c
    ${TOPDIR}/lib/xmltree/xmltok.h
    ${TOPDIR}/lib/xmltree/xmltok.c
    ${TOPDIR}/lib/xmltree/xmltree.h
    ${TOPDIR}/lib/xmltree/xmltree.cpp
    ${TOPDIR}/lib/xmltree/xmlinterface.h
    ${TOPDIR}/lib/xmltree/xmlinterface.cpp

#    ${TOPDIR}/lib/xmltree/xmltimpl.c
    ${TOPDIR}/lib/xmltree/asciitab.h
    ${TOPDIR}/lib/xmltree/xmltimpl.h
    ${TOPDIR}/lib/xmltree/iasctab.h
    ${TOPDIR}/lib/xmltree/xmltok/latin1tab.h
    ${TOPDIR}/lib/xmltree/xmltok/utf8tab.h
    ${TOPDIR}/lib/xmltree/xmltok/xmldef.h
    ${TOPDIR}/lib/xmltree/xmltok/nametab.h
)

set(LIBNEUTRINO_DAEMONC_SOURCES
    ${TOPDIR}/src/daemonc/remotecontrol.h
    ${TOPDIR}/src/daemonc/remotecontrol.cpp
)

set(LIBNEUTRINO_DRIVER_AUDIODEC_SOURCES
    ${TOPDIR}/src/driver/audiodec/basedec.h
    ${TOPDIR}/src/driver/audiodec/basedec.cpp
    ${TOPDIR}/src/driver/audiodec/cdrdec.h
    ${TOPDIR}/src/driver/audiodec/cdrdec.cpp
    ${TOPDIR}/src/driver/audiodec/mp3dec.h
    ${TOPDIR}/src/driver/audiodec/mp3dec.cpp
    ${TOPDIR}/src/driver/audiodec/oggdec.h
    ${TOPDIR}/src/driver/audiodec/oggdec.cpp
    ${TOPDIR}/src/driver/audiodec/wavdec.h
    ${TOPDIR}/src/driver/audiodec/wavdec.cpp
    ${TOPDIR}/src/driver/audiodec/tag.h
    ${TOPDIR}/src/driver/audiodec/tag.c
    ${TOPDIR}/src/driver/audiodec/vis.cpp

#    ${TOPDIR}/src/driver/audiodec/fft.c
#    ${TOPDIR}/src/driver/audiodec/crc.h
#    ${TOPDIR}/src/driver/audiodec/int_fft.c
#    ${TOPDIR}/src/driver/audiodec/fft.h
#    ${TOPDIR}/src/driver/audiodec/crc.c
)


set(LIBNEUTRINO_DRIVER_PICTUREVIEWER_SOURCES
    ${TOPDIR}/src/driver/pictureviewer/pictureviewer.h
    ${TOPDIR}/src/driver/pictureviewer/pictureviewer.cpp
    ${TOPDIR}/src/driver/pictureviewer/png.cpp
    ${TOPDIR}/src/driver/pictureviewer/bmp.cpp
    ${TOPDIR}/src/driver/pictureviewer/jpeg.cpp
    ${TOPDIR}/src/driver/pictureviewer/gif.cpp
    ${TOPDIR}/src/driver/pictureviewer/crw.cpp

#    ${TOPDIR}/src/driver/pictureviewer/resize.cpp
#    ${TOPDIR}/src/driver/pictureviewer/config.h
#    ${TOPDIR}/src/driver/pictureviewer/make15color_errdiff.cpp
#    ${TOPDIR}/src/driver/pictureviewer/picv_client_server.h
#    ${TOPDIR}/src/driver/pictureviewer/fb_display.cpp
#    ${TOPDIR}/src/driver/pictureviewer/fb_display.h
)


# "classical" framebuffer device
set(LIBNEUTRINO_FRAMEBUFFER_FBDEV_SOURCES
    ${TOPDIR}/src/driver/framebuffer.h
    ${TOPDIR}/src/driver/framebuffer.cpp
)


# hw accelerated framebuffer
set(LIBNEUTRINO_FRAMEBUFFER_GL_SOURCES
    ${TOPDIR}/src/driver/glthread.h
    ${TOPDIR}/src/driver/glthread.cpp
)

# sw decoder (using dvr output)
set(SWDECODER_SOURCES
    ${TOPDIR}/src/driver/decodethread.h
    ${TOPDIR}/src/driver/decodethread.cpp
)

set(LIBNEUTRINO_DRIVER_SOURCES
    ${TOPDIR}/src/driver/encoding.h
    ${TOPDIR}/src/driver/encoding.cpp
    ${TOPDIR}/src/driver/fontrenderer.h
    ${TOPDIR}/src/driver/fontrenderer.cpp
    ${TOPDIR}/src/driver/fb_window.h
    ${TOPDIR}/src/driver/fb_window.cpp
    ${TOPDIR}/src/driver/rcinput.h
    ${TOPDIR}/src/driver/rcinput.cpp
    ${TOPDIR}/src/driver/vcrcontrol.h
    ${TOPDIR}/src/driver/vcrcontrol.cpp
    ${TOPDIR}/src/driver/audioplay.h
    ${TOPDIR}/src/driver/audioplay.cpp
    ${TOPDIR}/src/driver/stream2file.h
    ${TOPDIR}/src/driver/stream2file.cpp
    ${TOPDIR}/src/driver/screen_max.h
    ${TOPDIR}/src/driver/screen_max.cpp
    ${TOPDIR}/src/driver/slotbuffer.h
    ${TOPDIR}/src/driver/slotbuffer.c
    ${TOPDIR}/src/driver/file.h
    ${TOPDIR}/src/driver/file.cpp
    ${TOPDIR}/src/driver/audiometadata.h
    ${TOPDIR}/src/driver/audiometadata.cpp
    ${TOPDIR}/src/driver/audiofile.h
    ${TOPDIR}/src/driver/audiofile.cpp
    ${TOPDIR}/src/driver/shutdown_count.h
    ${TOPDIR}/src/driver/shutdown_count.cpp
    ${TOPDIR}/src/driver/genpsi.h
    ${TOPDIR}/src/driver/genpsi.c
    ${TOPDIR}/src/driver/streamts.cpp
    ${TOPDIR}/src/driver/rfmod.h
    ${TOPDIR}/src/driver/rfmod.cpp

## FIXME: dummy header...
    ${TOPDIR}/src/driver/dummy/vfd.h

## readded for tripledragon
#    ${TOPDIR}/src/driver/lcdd.h
#    ${TOPDIR}/src/driver/lcdd.cpp
#    ${TOPDIR}/src/driver/lcdfontrenderer.cpp
#    ${TOPDIR}/src/driver/newclock.h
#    ${TOPDIR}/src/driver/newclock.cpp

## added for coolstream
#    ${TOPDIR}/src/driver/vfd.h
#    ${TOPDIR}/src/driver/vfd.cpp

## FIXME
#    ${TOPDIR}/src/driver/showframe.c # dreamspecific and not used?

## not sure
#    ${TOPDIR}/src/driver/capture.cpp
#    ${TOPDIR}/src/driver/capture.h
#    ${TOPDIR}/src/driver/wav.h
#    ${TOPDIR}/src/driver/bigclock.h
#    ${TOPDIR}/src/driver/bigclock.cpp
#    ${TOPDIR}/src/driver/aviaext.cpp
#    ${TOPDIR}/src/driver/pig.h
#    ${TOPDIR}/src/driver/pig.cpp
)

set(LIBNEUTRINO_DRIVER_RINGBUFFER_SOURCES
    ${TOPDIR}/src/driver/ringbuffer.h
    ${TOPDIR}/src/driver/ringbuffer.c
)


set(LIBNEUTRINO_DRIVER_NETFILE_SOURCES
    ${TOPDIR}/src/driver/netfile.h
    ${TOPDIR}/src/driver/netfile.cpp
)


set(LIBNEUTRINO_GUI_BEDIT_SOURCES
    ${TOPDIR}/src/gui/bedit/bouqueteditor_bouquets.h
    ${TOPDIR}/src/gui/bedit/bouqueteditor_bouquets.cpp
    ${TOPDIR}/src/gui/bedit/bouqueteditor_channels.h
    ${TOPDIR}/src/gui/bedit/bouqueteditor_channels.cpp
    ${TOPDIR}/src/gui/bedit/bouqueteditor_chanselect.h
    ${TOPDIR}/src/gui/bedit/bouqueteditor_chanselect.cpp
)


set(LIBNEUTRINO_GUI_WIDGET_SOURCES
    ${TOPDIR}/src/gui/widget/buttons.h
    ${TOPDIR}/src/gui/widget/buttons.cpp
    ${TOPDIR}/src/gui/widget/colorchooser.h
    ${TOPDIR}/src/gui/widget/colorchooser.cpp
    ${TOPDIR}/src/gui/widget/keychooser.h
    ${TOPDIR}/src/gui/widget/keychooser.cpp
    ${TOPDIR}/src/gui/widget/menue.h
    ${TOPDIR}/src/gui/widget/menue.cpp
    ${TOPDIR}/src/gui/widget/stringinput.h
    ${TOPDIR}/src/gui/widget/stringinput.cpp
    ${TOPDIR}/src/gui/widget/stringinput_ext.h
    ${TOPDIR}/src/gui/widget/stringinput_ext.cpp
    ${TOPDIR}/src/gui/widget/messagebox.h
    ${TOPDIR}/src/gui/widget/messagebox.cpp
    ${TOPDIR}/src/gui/widget/hintbox.h
    ${TOPDIR}/src/gui/widget/hintbox.cpp
    ${TOPDIR}/src/gui/widget/listbox.h
    ${TOPDIR}/src/gui/widget/listbox.cpp
    ${TOPDIR}/src/gui/widget/listbox_legacy.h
    ${TOPDIR}/src/gui/widget/listbox_legacy.cpp
    ${TOPDIR}/src/gui/widget/drawable.h
    ${TOPDIR}/src/gui/widget/drawable.cpp
    ${TOPDIR}/src/gui/widget/hintboxext.h
    ${TOPDIR}/src/gui/widget/hintboxext.cpp
    ${TOPDIR}/src/gui/widget/helpbox.h
    ${TOPDIR}/src/gui/widget/helpbox.cpp
    ${TOPDIR}/src/gui/widget/mountchooser.h
    ${TOPDIR}/src/gui/widget/mountchooser.cpp
    ${TOPDIR}/src/gui/widget/listframe.h
    ${TOPDIR}/src/gui/widget/listframe.cpp
    ${TOPDIR}/src/gui/widget/msgbox.h
    ${TOPDIR}/src/gui/widget/msgbox.cpp
    ${TOPDIR}/src/gui/widget/textbox.h
    ${TOPDIR}/src/gui/widget/textbox.cpp
    ${TOPDIR}/src/gui/widget/progressbar.h
    ${TOPDIR}/src/gui/widget/progressbar.cpp

#    ${TOPDIR}/src/gui/widget/lcdcontroler.h # is this not used?
#    ${TOPDIR}/src/gui/widget/lcdcontroler.cpp
#    ${TOPDIR}/src/gui/widget/rgbcsynccontroler.h
#    ${TOPDIR}/src/gui/widget/rgbcsynccontroler.cpp
)


set(LIBNEUTRINO_GUI_WIDGET2_SOURCES
    ${TOPDIR}/src/gui/widget/progressstatus.h
    ${TOPDIR}/src/gui/widget/progresswindow.h
    ${TOPDIR}/src/gui/widget/progresswindow.cpp
    ${TOPDIR}/src/gui/widget/icons.h
    ${TOPDIR}/src/gui/widget/vfdcontroler.h
    ${TOPDIR}/src/gui/widget/vfdcontroler.cpp
)


set(LIBNEUTRINO_GUI_SOURCES
    ${TOPDIR}/src/gui/bouquetlist.h
    ${TOPDIR}/src/gui/bouquetlist.cpp
    ${TOPDIR}/src/gui/channellist.h
    ${TOPDIR}/src/gui/channellist.cpp
    ${TOPDIR}/src/gui/eventlist.h
    ${TOPDIR}/src/gui/eventlist.cpp
    ${TOPDIR}/src/gui/pluginlist.h
    ${TOPDIR}/src/gui/pluginlist.cpp
    ${TOPDIR}/src/gui/favorites.h
    ${TOPDIR}/src/gui/favorites.cpp
    ${TOPDIR}/src/gui/sleeptimer.h
    ${TOPDIR}/src/gui/sleeptimer.cpp
    ${TOPDIR}/src/gui/screensetup.h
    ${TOPDIR}/src/gui/screensetup.cpp
    ${TOPDIR}/src/gui/epgview.h
    ${TOPDIR}/src/gui/epgview.cpp
    ${TOPDIR}/src/gui/infoviewer.h
    ${TOPDIR}/src/gui/infoviewer.cpp
    ${TOPDIR}/src/gui/scan.h
    ${TOPDIR}/src/gui/scan.cpp
    ${TOPDIR}/src/gui/update.h
    ${TOPDIR}/src/gui/update.cpp
    ${TOPDIR}/src/gui/filebrowser.h
    ${TOPDIR}/src/gui/filebrowser.cpp
    ${TOPDIR}/src/gui/audioplayer.h
    ${TOPDIR}/src/gui/audioplayer.cpp
    ${TOPDIR}/src/gui/nfs.h
    ${TOPDIR}/src/gui/nfs.cpp
    ${TOPDIR}/src/gui/pictureviewer.h
    ${TOPDIR}/src/gui/pictureviewer.cpp
    ${TOPDIR}/src/gui/movieplayer.h
    ${TOPDIR}/src/gui/movieplayer.cpp
    ${TOPDIR}/src/gui/rc_lock.h
    ${TOPDIR}/src/gui/rc_lock.cpp
    ${TOPDIR}/src/gui/bookmarkmanager.h
    ${TOPDIR}/src/gui/bookmarkmanager.cpp
    ${TOPDIR}/src/gui/timeosd.h
    ${TOPDIR}/src/gui/timeosd.cpp
    ${TOPDIR}/src/gui/epgplus.h
    ${TOPDIR}/src/gui/epgplus.cpp
    ${TOPDIR}/src/gui/epg_menu.h
    ${TOPDIR}/src/gui/epg_menu.cpp
    ${TOPDIR}/src/gui/streaminfo2.h
    ${TOPDIR}/src/gui/streaminfo2.cpp
    ${TOPDIR}/src/gui/dboxinfo.h
    ${TOPDIR}/src/gui/dboxinfo.cpp
    ${TOPDIR}/src/gui/plugins.h
    ${TOPDIR}/src/gui/plugins.cpp
    ${TOPDIR}/src/gui/imageinfo.h
    ${TOPDIR}/src/gui/imageinfo.cpp
    ${TOPDIR}/src/gui/audio_select.h
    ${TOPDIR}/src/gui/audio_select.cpp
    ${TOPDIR}/src/gui/moviebrowser.h
    ${TOPDIR}/src/gui/moviebrowser.cpp
    ${TOPDIR}/src/gui/movieinfo.h
    ${TOPDIR}/src/gui/movieinfo.cpp

)


set(LIBNEUTRINO_GUI2_SOURCES
    ${TOPDIR}/src/gui/color.h
    ${TOPDIR}/src/gui/color.cpp
    ${TOPDIR}/src/gui/alphasetup.h
    ${TOPDIR}/src/gui/alphasetup.cpp
    ${TOPDIR}/src/gui/motorcontrol.h
    ${TOPDIR}/src/gui/motorcontrol.cpp
    ${TOPDIR}/src/gui/hdd_menu.h
    ${TOPDIR}/src/gui/hdd_menu.cpp
    ${TOPDIR}/src/gui/infoclock.h
    ${TOPDIR}/src/gui/infoclock.cpp
    ${TOPDIR}/src/gui/upnpbrowser.h
    ${TOPDIR}/src/gui/upnpbrowser.cpp

## for coolstream
#    ${TOPDIR}/src/gui/cam_menu.h
#    ${TOPDIR}/src/gui/cam_menu.cpp

## not referenced in Makefile.am
#    ${TOPDIR}/src/gui/scale.h # obsolete code
#    ${TOPDIR}/src/gui/scale.cpp
#    ${TOPDIR}/src/gui/ch_mosaic.cpp # was experimental code
#    ${TOPDIR}/src/gui/ch_mosaic.h
#    ${TOPDIR}/src/gui/customcolor.h
#    ${TOPDIR}/src/gui/ringbuffer.c
#    ${TOPDIR}/src/gui/dmx_tspidbandwidth.c # not used?
)


set(LIBTIMERLIST_SOURCES
    ${TOPDIR}/src/gui/timerlist.h
    ${TOPDIR}/src/gui/timerlist.cpp
)


## readded for tripledragon
set(LIBLCDDISPLAY_SOURCES
    ${TOPDIR}/src/lcddisplay/lcddisplay.h
    ${TOPDIR}/src/lcddisplay/lcddisplay.cpp
    ${TOPDIR}/src/lcddisplay/fontrenderer.h
    ${TOPDIR}/src/lcddisplay/fontrenderer.cpp
)


set(LIBNHTTPD_TUXBOXAPI_COOL_SOURCES
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/neutrinoapi.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/neutrinoapi.cpp
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/neutrinoyparser.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/neutrinoyparser.cpp
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/controlapi.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream/controlapi.cpp
)


set(LIBNHTTPD_TUXBOXAPI_DBOX_SOURCES
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/neutrinoapi.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/neutrinoapi.cpp
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/neutrinoyparser.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/neutrinoyparser.cpp
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/lcdapi.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/lcdapi.cpp
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/controlapi.h
    ${TOPDIR}/src/nhttpd/tuxboxapi/dbox/controlapi.cpp
)


set(LIBYHTTPD_SOURCES
    ${TOPDIR}/src/nhttpd/yhttpd_core/ytypes_globals.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/ylogging.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/ylogging.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/helper.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/helper.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/ylanguage.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/ylanguage.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/ywebserver.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/ywebserver.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/yconnection.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/yconnection.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/yrequest.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/yrequest.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/yresponse.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/yresponse.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/yhook.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/yhook.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_core/ysocket.h
    ${TOPDIR}/src/nhttpd/yhttpd_core/ysocket.cpp
)

set(LIBYHTTPDMODS_SOURCES
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_yparser.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_yparser.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_testhook.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_testhook.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_auth.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_auth.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_cache.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_cache.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_sendfile.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_sendfile.cpp
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_weblog.h
    ${TOPDIR}/src/nhttpd/yhttpd_mods/mod_weblog.cpp
)

set(LIBNHTTPD_SOURCES
    ${TOPDIR}/src/nhttpd/yconfig.h
    ${TOPDIR}/src/nhttpd/yhttpd.cpp
    ${TOPDIR}/src/nhttpd/yhttpd.h
)


set(LIBSECTIONSD_SOURCES
    ${TOPDIR}/src/sectionsd/SIbouquets.hpp
    ${TOPDIR}/src/sectionsd/SInetworks.hpp
    ${TOPDIR}/src/sectionsd/SIservices.hpp
    ${TOPDIR}/src/sectionsd/FreesatTables.hpp

    ${TOPDIR}/src/sectionsd/sectionsd.cpp
    ${TOPDIR}/src/sectionsd/dmxapi.h
    ${TOPDIR}/src/sectionsd/dmxapi.cpp
    ${TOPDIR}/src/sectionsd/debug.h
    ${TOPDIR}/src/sectionsd/debug.cpp
    ${TOPDIR}/src/sectionsd/dmx.h
    ${TOPDIR}/src/sectionsd/dmx.cpp
    ${TOPDIR}/src/sectionsd/SIsections.hpp
    ${TOPDIR}/src/sectionsd/SIsections.cpp
    ${TOPDIR}/src/sectionsd/SIevents.hpp
    ${TOPDIR}/src/sectionsd/SIevents.cpp
    ${TOPDIR}/src/sectionsd/SIutils.hpp
    ${TOPDIR}/src/sectionsd/SIutils.cpp
    ${TOPDIR}/src/sectionsd/SIlanguage.hpp
    ${TOPDIR}/src/sectionsd/SIlanguage.cpp
    ${TOPDIR}/src/sectionsd/edvbstring.h
    ${TOPDIR}/src/sectionsd/edvbstring.cpp

#    ${TOPDIR}/src/sectionsd/sdt.cpp
#    ${TOPDIR}/src/sectionsd/epgMini.cpp
#    ${TOPDIR}/src/sectionsd/bat.cpp
#    ${TOPDIR}/src/sectionsd/epg.cpp
#    ${TOPDIR}/src/sectionsd/nit.cpp
)

set(LIBNEUTRINO_SYSTEM_SOURCES
    ${TOPDIR}/src/system/locals.h
    ${TOPDIR}/src/system/locals_intern.h

    ${TOPDIR}/src/system/localize.h
    ${TOPDIR}/src/system/localize.cpp
    ${TOPDIR}/src/system/setting_helpers.h
    ${TOPDIR}/src/system/setting_helpers.cpp
    ${TOPDIR}/src/system/debug.h
    ${TOPDIR}/src/system/debug.cpp
    ${TOPDIR}/src/system/ping-linux.h
    ${TOPDIR}/src/system/ping.h
    ${TOPDIR}/src/system/ping-config.h
    ${TOPDIR}/src/system/ping.c
    ${TOPDIR}/src/system/flashtool.h
    ${TOPDIR}/src/system/flashtool.cpp
    ${TOPDIR}/src/system/httptool.h
    ${TOPDIR}/src/system/httptool.cpp
    ${TOPDIR}/src/system/settings.h
    ${TOPDIR}/src/system/settings.cpp
    ${TOPDIR}/src/system/lastchannel.h
    ${TOPDIR}/src/system/lastchannel.cpp
    ${TOPDIR}/src/system/configure_network.h
    ${TOPDIR}/src/system/configure_network.cpp
    ${TOPDIR}/src/system/fsmounter.h
    ${TOPDIR}/src/system/fsmounter.cpp

## empty code - obsolete
#    ${TOPDIR}/src/system/timer.h
#    ${TOPDIR}/src/system/timer.cpp
)

set(LIBTIMERD_SOURCES
    ${TOPDIR}/src/timerd/debug.h
    ${TOPDIR}/src/timerd/timermanager.h
    ${TOPDIR}/src/timerd/timermanager.cpp
    ${TOPDIR}/src/timerd/timerd.cpp
)

set(LIBZAPITCLIENT_SOURCES
    ${TOPDIR}/src/zapit/include/zapit/client/msgtypes.h
    ${TOPDIR}/src/zapit/include/zapit/client/zapittools.h
    ${TOPDIR}/src/zapit/include/zapit/client/zapittypes.h
    ${TOPDIR}/src/zapit/include/zapit/client/zapitclient.h
    ${TOPDIR}/src/zapit/lib/zapitclient.cpp
    ${TOPDIR}/src/zapit/lib/zapittools.cpp
)

set(LIBZAPIT_SOURCES
    ${TOPDIR}/src/zapit/include/audio.h
    ${TOPDIR}/src/zapit/include/dmx.h
    ${TOPDIR}/src/zapit/include/video.h
    ${TOPDIR}/src/zapit/include/zapit/descriptors.h
    ${TOPDIR}/src/zapit/include/zapit/bat.h
    ${TOPDIR}/src/zapit/include/zapit/debug.h
    ${TOPDIR}/src/zapit/include/zapit/bouquets.h
    ${TOPDIR}/src/zapit/include/zapit/ci.h
    ${TOPDIR}/src/zapit/include/zapit/dvbstring.h
    ${TOPDIR}/src/zapit/include/zapit/types.h
    ${TOPDIR}/src/zapit/include/zapit/transponder.h
    ${TOPDIR}/src/zapit/include/zapit/getservices.h
    ${TOPDIR}/src/zapit/include/zapit/cam.h
    ${TOPDIR}/src/zapit/include/zapit/satconfig.h
    ${TOPDIR}/src/zapit/include/zapit/pmt.h
    ${TOPDIR}/src/zapit/include/zapit/nit.h
    ${TOPDIR}/src/zapit/include/zapit/channel.h
    ${TOPDIR}/src/zapit/include/zapit/settings.h
    ${TOPDIR}/src/zapit/include/zapit/sdt.h
    ${TOPDIR}/src/zapit/include/zapit/pat.h
    ${TOPDIR}/src/zapit/include/zapit/zapit.h
    ${TOPDIR}/src/zapit/include/zapit/scan.h
    ${TOPDIR}/src/zapit/include/zapit/frontend_c.h
    ${TOPDIR}/src/zapit/include/linux/dvb/net.h
    ${TOPDIR}/src/zapit/include/linux/dvb/audio.h
    ${TOPDIR}/src/zapit/include/linux/dvb/osd.h
    ${TOPDIR}/src/zapit/include/linux/dvb/ca.h
    ${TOPDIR}/src/zapit/include/linux/dvb/dmx.h
    ${TOPDIR}/src/zapit/include/linux/dvb/version.h
    ${TOPDIR}/src/zapit/include/linux/dvb/video.h
    ${TOPDIR}/src/zapit/include/linux/dvb/frontend.h
#    ${TOPDIR}/src/zapit/src/bat.cpp
#    ${TOPDIR}/src/zapit/src/transponder.cpp

    ${TOPDIR}/src/zapit/src/bouquets.cpp
    ${TOPDIR}/src/zapit/src/channel.cpp
    ${TOPDIR}/src/zapit/src/dvbstring.cpp
    ${TOPDIR}/src/zapit/src/getservices.cpp
    ${TOPDIR}/src/zapit/src/scan.cpp
    ${TOPDIR}/src/zapit/src/zapit.cpp
    ${TOPDIR}/src/zapit/src/ca.h
    ${TOPDIR}/src/zapit/src/cam.cpp
    ${TOPDIR}/src/zapit/src/ci.cpp
    ${TOPDIR}/src/zapit/src/frontend.cpp
    ${TOPDIR}/src/zapit/src/descriptors.cpp
    ${TOPDIR}/src/zapit/src/nit.cpp
    ${TOPDIR}/src/zapit/src/pat.cpp
    ${TOPDIR}/src/zapit/src/pmt.cpp
    ${TOPDIR}/src/zapit/src/sdt.cpp
)


set(UDPSTREAMPES_SOURCES
    ${TOPDIR}/src/zapit/src/udpstreampes.h
    ${TOPDIR}/src/zapit/src/udpstreampes.cpp
)


set(PZAPIT_SOURCES
    ${TOPDIR}/src/zapit/src/pzapit.cpp
)


set(NEUTRINO_SOURCES
    ${TOPDIR}/src/global.h
    ${TOPDIR}/src/videosettings.h
    ${TOPDIR}/src/plugin.h
    ${TOPDIR}/src/neutrinoMessages.h
    ${TOPDIR}/src/mymenu.h

    ${TOPDIR}/src/neutrino.h
    ${TOPDIR}/src/neutrino.cpp
    ${TOPDIR}/src/neutrino_menue.cpp
)


include_directories(
    ${CMAKE_CURRENT_BINARY_DIR} # for config.h
    ${TOPDIR}/lib/libconfigfile
    ${TOPDIR}/src
    ${TOPDIR}/src/driver
    ${TOPDIR}/src/driver/audiodec
    ${TOPDIR}/src/gui
    ${TOPDIR}/src/nhttpd
    ${TOPDIR}/src/nhttpd/yhttpd_core
    ${TOPDIR}/src/nhttpd/yhttpd_mods
    ${TOPDIR}/src/nhttpd/tuxboxapi/coolstream
    ${TOPDIR}/src/sectionsd
    ${TOPDIR}/src/timerd
    ${TOPDIR}/lib
    ${TOPDIR}/lib/connection
    ${TOPDIR}/lib/libdvbapi
    ${TOPDIR}/lib/libeventserver
    ${TOPDIR}/lib/libmd5sum
    ${TOPDIR}/lib/libnet
    ${TOPDIR}/src/zapit/include
    ${TOPDIR}/lib/xmltree
    ${TOPDIR}/lib/xmltree/xmltok
    ${TOPDIR}/lib/libupnpclient
    /usr/include/freetype2 # FIXME: use freetype search
)

#
# configure step
# unfortunately a lot of hard-coded paths exist
# several to /var/tuxbox so better sudo and ln -s a bit :)
#
set(HAVE_PC_HARDWARE 1)
set(HAVE_DVB_API_VERSION 3)
set(CONFIGDIR "share/neutrino")
set(FONTDIR "share/neutrino/fonts")
set(DATADIR "share")
set(PLUGINDIR "plugins")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
set(USE_OPENGL 1) # since the framebuffer header is different we cannot just make it a link time option!

#
# create the libraries
#

# dynamic (SHARED) or static (STATIC linkage for helper shared libraries
set(NEUTRINO_LINKTYPE SHARED)

add_library(tuxbox_connection ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_CONNECTION_SOURCES})
add_library(controldclient ${NEUTRINO_LINKTYPE} ${LIBCONTROLDCLIENT_SOURCES})
add_library(tuxbox_configfile ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_CONFIGFILE_SOURCES})
#add_library(coolstream ${NEUTRINO_LINKTYPE} ${LIBCOOLSTREAM_SOURCES})
add_library(dvbapi ${NEUTRINO_LINKTYPE} ${LIBDVBAPI_SOURCES})
add_library(dvbsub ${NEUTRINO_LINKTYPE} ${LIBDVBSUB_SOURCES})
add_library(tuxbox_eventserver ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_EVENTSERVER_SOURCES})
add_library(tuxbox_md5sum ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_MD5SUM_SOURCES})
add_library(tuxbox_net ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_NET_SOURCES})
#add_library(triple ${NEUTRINO_LINKTYPE} ${LIBTRIPLE_SOURCES})
add_library(tuxtxt ${NEUTRINO_LINKTYPE} ${LIBTUXTXT_SOURCES})
add_library(upnpclient ${NEUTRINO_LINKTYPE} ${LIBUPNPCLIENT_SOURCES})
add_library(sectionsdclient ${NEUTRINO_LINKTYPE} ${SECTIONSDCLIENT_SOURCES})
add_library(sectionsdcontrol ${NEUTRINO_LINKTYPE} ${SECTIONSDCONTROL_SOURCES})
add_library(timerdclient ${NEUTRINO_LINKTYPE} ${TIMERDCLIENT_SOURCES})
add_library(tuxbox_xmltree ${NEUTRINO_LINKTYPE} ${LIBTUXBOX_XMLTREE_SOURCES})
add_library(neutrino_daemonc ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DAEMONC_SOURCES})
add_library(neutrino_driver_audiodec ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DRIVER_AUDIODEC_SOURCES})
add_library(neutrino_driver_pictureviewer ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DRIVER_PICTUREVIEWER_SOURCES})
add_library(neutrino_driver ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DRIVER_SOURCES})
add_library(neutrino_driver_ringbuffer ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DRIVER_RINGBUFFER_SOURCES})
add_library(neutrino_driver_netfile ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_DRIVER_NETFILE_SOURCES})
add_library(neutrino_gui_bedit ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_GUI_BEDIT_SOURCES})
add_library(neutrino_gui_widget ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_GUI_WIDGET_SOURCES})
add_library(neutrino_gui_widget2 ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_GUI_WIDGET2_SOURCES})
add_library(neutrino_gui ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_GUI_SOURCES})
add_library(neutrino_gui2 ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_GUI2_SOURCES})
add_library(timerlist ${NEUTRINO_LINKTYPE} ${LIBTIMERLIST_SOURCES})
#add_library(lcddisplay ${NEUTRINO_LINKTYPE} ${LIBLCDDISPLAY_SOURCES}) # dbox2 dependent
add_library(nhttpd_tuxboxapi_cool ${NEUTRINO_LINKTYPE} ${LIBNHTTPD_TUXBOXAPI_COOL_SOURCES})
##add_library(nhttpd_tuxboxapi_dbox ${NEUTRINO_LINKTYPE} ${LIBNHTTPD_TUXBOXAPI_DBOX_SOURCES}) # FIXME: this is in the sources but unused by Makefile.am
add_library(yhttpd ${NEUTRINO_LINKTYPE} ${LIBYHTTPD_SOURCES})
add_library(yhttpdmods ${NEUTRINO_LINKTYPE} ${LIBYHTTPDMODS_SOURCES})
add_library(nhttpd ${NEUTRINO_LINKTYPE} ${LIBNHTTPD_SOURCES})
add_library(sectionsd ${NEUTRINO_LINKTYPE} ${LIBSECTIONSD_SOURCES})
add_library(neutrino_system ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_SYSTEM_SOURCES})
add_library(timerd ${NEUTRINO_LINKTYPE} ${LIBTIMERD_SOURCES})
add_library(zapitclient ${NEUTRINO_LINKTYPE} ${LIBZAPITCLIENT_SOURCES})
add_library(zapit ${NEUTRINO_LINKTYPE} ${LIBZAPIT_SOURCES})
add_library(swdecoder ${NEUTRINO_LINKTYPE} ${SWDECODER_SOURCES})

if(USE_OPENGL)
    add_definitions(-DUSE_OPENGL)
    add_library(neutrino_framebuffer_gl ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_FRAMEBUFFER_GL_SOURCES})
endif(USE_OPENGL)
    add_library(neutrino_framebuffer ${NEUTRINO_LINKTYPE} ${LIBNEUTRINO_FRAMEBUFFER_FBDEV_SOURCES})

add_definitions(-DDO_NOT_INCLUDE_STUFF_NOT_NEEDED_FOR_SECTIONSD)

#
# create the executables
#
add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
add_definitions(-DHAVE_CONFIG_H)

#add_executable(udpstreampes ${UDPSTREAMPES_SOURCES)
#add_executable(pzapit ${PZAPIT_SOURCES)

add_executable(neutrino ${NEUTRINO_SOURCES})
target_link_libraries(neutrino
        pthread
        neutrino_daemonc
        neutrino_gui_bedit
        timerlist
        neutrino_gui
        neutrino_gui_widget
        neutrino_gui_widget2
        neutrino_driver_pictureviewer
        neutrino_system
        neutrino_driver
        #movieinfo?
        neutrino_driver_audiodec
        neutrino_driver_netfile
        neutrino_gui2
        sectionsd
        timerd
        zapit
        yhttpd
        nhttpd_tuxboxapi_cool
        yhttpdmods
        nhttpd

        sectionsdclient
        timerdclient
        zapitclient
        tuxbox_configfile
        tuxbox_connection
        tuxbox_eventserver
        tuxbox_xmltree
        tuxbox_net
        tuxbox_md5sum
        tuxtxt
        dvbsub
        upnpclient

        #controldclient
        #sectionsdcontrol
        #neutrino_driver_ringbuffer
        
        dvbapi
		swdecoder

        neutrino_framebuffer_gl
        neutrino_framebuffer
        
        mad
        png
        vorbisfile
        curl
        id3tag
        freetype
        jpeg
        ungif
        avcodec
        avformat
        swscale
        glut
        GLEW
        boost_thread
)

