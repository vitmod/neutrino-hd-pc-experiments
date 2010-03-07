#include <stdio.h>

#include "init_td.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#include <directfb.h>

static const char * FILENAME = "init_td.cpp";

static bool initialized = false;

/* the super interface */
static IDirectFB *dfb;
/* the primary surface */
static IDirectFBSurface *primary;
static IDirectFBSurface *dest;
static IDirectFBDisplayLayer *layer;

#define DFBCHECK(x...)                                                \
	err = x;                                                      \
	if (err != DFB_OK) {                                          \
		fprintf(stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
		DirectFBErrorFatal(#x, err );                         \
	}

static void dfb_init()
{
	int argc = 0;
	DFBResult err;
	DFBSurfaceDescription dsc;
	DFBSurfacePixelFormat pixelformat;
	int SW, SH;

	DFBCHECK(DirectFBInit(&argc, NULL));
	/* neutrino does its own VT handling */
	DirectFBSetOption("no-vt-switch", NULL);
	DirectFBSetOption("no-vt", NULL);
	/* signal handling seems to interfere with neutrino */
	DirectFBSetOption("no-sighandler", NULL);
	/* if DirectFB grabs the remote, neutrino does not get events */
	DirectFBSetOption("disable-module", "tdremote");
	DirectFBSetOption("disable-module", "keyboard");
	DirectFBSetOption("disable-module", "linux_input");
	DFBCHECK(DirectFBCreate(&dfb));

	err = dfb->SetCooperativeLevel(dfb, DFSCL_FULLSCREEN);
	if (err)
		DirectFBError("Failed to get exclusive access", err);

	dsc.flags = DSDESC_CAPS;
	dsc.caps = DSCAPS_PRIMARY;

	DFBCHECK(dfb->CreateSurface( dfb, &dsc, &primary ));
	/* set pixel alpha mode */
	dfb->GetDisplayLayer(dfb, DLID_PRIMARY, &layer);
	DFBCHECK(layer->SetCooperativeLevel(layer, DLSCL_EXCLUSIVE));
	DFBDisplayLayerConfig conf;
	DFBCHECK(layer->GetConfiguration(layer, &conf));
	conf.flags   = DLCONF_OPTIONS;
	conf.options = (DFBDisplayLayerOptions)((conf.options & ~DLOP_OPACITY) | DLOP_ALPHACHANNEL);
	DFBCHECK(layer->SetConfiguration(layer, &conf));

	primary->GetPixelFormat(primary, &pixelformat);
	primary->GetSize(primary, &SW, &SH);
	primary->Clear(primary, 0, 0, 0, 0);
	primary->GetSubSurface(primary, NULL, &dest);
	dest->Clear(dest, 0, 0, 0, 0);
}

static void dfb_deinit()
{
	dest->Release(dest);
	primary->Release(primary);
	layer->Release(layer);
	dfb->Release(dfb);
}

void init_td_api()
{
	fprintf(stderr, "%s:%s begin, initialized = %d\n", FILENAME, __FUNCTION__, (int)initialized);
	if (!initialized)
	{
		/* DirectFB does setpgid(0,0), which disconnects us from controlling terminal
		   and thus disables e.g. ctrl-C. work around that. */
		pid_t pid = getpgid(0);
		dfb_init();
		if (setpgid(0, pid))
			perror("setpgid");
	}
	initialized = true;
	fprintf(stderr, "%s:%s end\n", FILENAME, __FUNCTION__);
}

void shutdown_td_api()
{
	fprintf(stderr, "%s:%s, initialized = %d\n", FILENAME, __FUNCTION__, (int)initialized);
	if (initialized)
		dfb_deinit();
	initialized = false;
}
