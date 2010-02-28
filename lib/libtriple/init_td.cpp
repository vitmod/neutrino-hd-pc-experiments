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


/* the super interface */
static IDirectFB *dfb;
/* the primary surface */
static IDirectFBSurface *primary;
static IDirectFBSurface *dest;

#define DFBCHECK(x...)                                                \
	err = x;                                                      \
	if (err != DFB_OK) {                                          \
		fprintf(stderr, "%s <%d>:\n\t", __FILE__, __LINE__ ); \
		DirectFBErrorFatal(#x, err );                         \
	}

void init_td_api()
{
	fprintf(stderr, "%s:%s begin\n", FILENAME, __FUNCTION__);
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
	DFBCHECK(DirectFBCreate(&dfb));

	err = dfb->SetCooperativeLevel(dfb, DFSCL_FULLSCREEN);
	if (err)
		DirectFBError("Failed to get exclusive access", err);

	dsc.flags = DSDESC_CAPS;
	dsc.caps = DSCAPS_PRIMARY;

	DFBCHECK(dfb->CreateSurface( dfb, &dsc, &primary ));
	primary->GetPixelFormat( primary, &pixelformat );
	primary->GetSize( primary, &SW, &SH );
	primary->Clear( primary, 0, 0, 0, 0xFF );
	primary->GetSubSurface (primary, NULL, &dest);
	dest->Clear( dest, 0, 0, 0, 0xFF );
	fprintf(stderr, "%s:%s end\n", FILENAME, __FUNCTION__);
}

void shutdown_td_api()
{
	fprintf(stderr, "%s:%s\n", FILENAME, __FUNCTION__);
	dest->Release( dest );
	primary->Release( primary );
	dfb->Release( dfb );
}
