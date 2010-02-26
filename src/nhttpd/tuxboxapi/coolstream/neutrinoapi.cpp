//=============================================================================
// NHTTPD
// NeutrionAPI
//
// Aggregates: NeutrinoYParser, NeutrinoControlAPI
// Defines Interfaces to:CControldClient, CSectionsdClient, CZapitClient,
//			CTimerdClient,CLCDAPI
// Place for common used Neutrino-functions used by NeutrinoYParser, NeutrinoControlAPI
//=============================================================================

// C
#include <cstdlib>
#include <cstring>

// C++
#include <string>
#include <fstream>
#include <map>

// tuxbox
#include <neutrinoMessages.h>

#include <zapit/client/zapitclient.h>
#include <zapit/channel.h>
#include <zapit/bouquets.h>
extern tallchans allchans;
extern CBouquetManager *g_bouquetManager;

// yhttpd
#include "ylogging.h"

// nhttpd
#include "neutrinoapi.h"

void sectionsd_getChannelEvents(CChannelEventList &eList, const bool tv_mode = true, t_channel_id *chidlist = NULL, int clen = 0);

//=============================================================================
// No Class Helpers
//=============================================================================

static std::map<std::string, std::string> iso639;
#ifndef initialize_iso639_map
bool _initialize_iso639_map(void)
{
	std::string s, t, u, v;
	std::ifstream in("/share/iso-codes/iso-639.tab");
	if (in.is_open())
	{
		while (in.peek() == '#')
			getline(in, s);
		while (in >> s >> t >> u >> std::ws)
		{
			getline(in, v);
			iso639[s] = v;
			if (s != t)
				iso639[t] = v;
		}
		in.close();
		return true;
	}
 	else
		return false;
}
#endif
//-----------------------------------------------------------------------------
const char * _getISO639Description(const char * const iso)
{
	std::map<std::string, std::string>::const_iterator it = iso639.find(std::string(iso));
	if (it == iso639.end())
		return iso;
	else
		return it->second.c_str();
}

//=============================================================================
// Initialization of static variables
//=============================================================================
std::string CNeutrinoAPI::Dbox_Hersteller[4]	= {"none", "Nokia", "Philips", "Sagem"};
std::string CNeutrinoAPI::videooutput_names[5]	= {"CVBS", "RGB with CVBS", "S-Video", "YUV with VBS", "YUV with CVBS"};
std::string CNeutrinoAPI::videoformat_names[4]	= {"automatic", "16:9", "4:3 (LB)", "4:3 (PS)"};
std::string CNeutrinoAPI::audiotype_names[5] 	= {"none", "single channel","dual channel","joint stereo","stereo"};

//=============================================================================
// Constructor & Destructor
//=============================================================================
CNeutrinoAPI::CNeutrinoAPI()
{
	Sectionsd = new CSectionsdClient();
	Zapit = new CZapitClient();
	Timerd = new CTimerdClient();

	NeutrinoYParser = new CNeutrinoYParser(this);
	ControlAPI = new CControlAPI(this);

	UpdateBouquets();

	EventServer = new CEventServer;
	EventServer->registerEvent2( NeutrinoMessages::SHUTDOWN, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::STANDBY_ON, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::STANDBY_OFF, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::STANDBY_TOGGLE, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::EVT_POPUP, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::EVT_EXTMSG, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::CHANGEMODE, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::EVT_START_PLUGIN, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::LOCK_RC, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
	EventServer->registerEvent2( NeutrinoMessages::UNLOCK_RC, CEventServer::INITID_HTTPD, "/tmp/neutrino.sock");
}
//-------------------------------------------------------------------------

CNeutrinoAPI::~CNeutrinoAPI(void)
{
	if (NeutrinoYParser)
		delete NeutrinoYParser;
	if (ControlAPI)
		delete ControlAPI;
	if (Sectionsd)
		delete Sectionsd;
	if (Zapit)
		delete Zapit;
	if (Timerd)
		delete Timerd;
	if (EventServer)
		delete EventServer;
}

//-------------------------------------------------------------------------

void CNeutrinoAPI::UpdateBouquets(void)
{
#if 0 //FIXME
	BouquetList.clear();
	Zapit->getBouquets(BouquetList, true, true);
	for (unsigned int i = 1; i <= BouquetList.size(); i++)
		UpdateBouquet(i);

	UpdateChannelList();
#endif
}

//-------------------------------------------------------------------------
void CNeutrinoAPI::ZapTo(const char * const target)
{
	t_channel_id channel_id;

	sscanf(target,
		SCANF_CHANNEL_ID_TYPE,
		&channel_id);

	ZapToChannelId(channel_id);
}
//-------------------------------------------------------------------------
void CNeutrinoAPI::ZapToChannelId(t_channel_id channel_id)
{
	if (channel_id == Zapit->getCurrentServiceID())
	{
		//printf("Kanal ist aktuell\n");
		return;
	}

	if (Zapit->zapTo_serviceID(channel_id) != CZapitClient::ZAP_INVALID_PARAM)
		Sectionsd->setServiceChanged(channel_id&0xFFFFFFFFFFFFULL, false);
}
//-------------------------------------------------------------------------

void CNeutrinoAPI::ZapToSubService(const char * const target)
{
	t_channel_id channel_id;

	sscanf(target,
		SCANF_CHANNEL_ID_TYPE,
		&channel_id);

	if (Zapit->zapTo_subServiceID(channel_id) != CZapitClient::ZAP_INVALID_PARAM)
		Sectionsd->setServiceChanged(channel_id&0xFFFFFFFFFFFFULL, false);
}
//-------------------------------------------------------------------------
t_channel_id CNeutrinoAPI::ChannelNameToChannelId(std::string search_channel_name)
{
//FIXME depending on mode missing
	//int mode = Zapit->getMode();
	t_channel_id channel_id = (t_channel_id)-1;
	CStringArray channel_names = ySplitStringVector(search_channel_name, ",");
	for (tallchans_iterator it = allchans.begin(); it != allchans.end(); it++) {
		std::string channel_name = it->second.getName();
		for(unsigned int j=0;j<channel_names.size();j++)
		{
			if(channel_names[j].length() == channel_name.length() &&
				equal(channel_names[j].begin(), channel_names[j].end(),
				channel_name.begin(), nocase_compare)) //case insensitive  compare
			{
				channel_id = it->second.channel_id;
				break;
			}
		}
	}
	return channel_id;
}

//-------------------------------------------------------------------------
// Get functions
//-------------------------------------------------------------------------

bool CNeutrinoAPI::GetStreamInfo(int bitInfo[10])
{
	char *key, *tmpptr, buf[100];
	long value;
	int pos = 0;

	memset(bitInfo, 0, sizeof(bitInfo));

	FILE *fd = fopen("/proc/bus/bitstream", "rt");

	if (fd == NULL)
	{
		dprintf("error while opening proc-bitstream\n" );
		return false;
	}

	fgets(buf,35,fd);//dummy
	while(!feof(fd))
	{
		if(fgets(buf,35,fd)!=NULL)
		{
			buf[strlen(buf)-1]=0;
			tmpptr=buf;
			key=strsep(&tmpptr,":");
			value=strtoul(tmpptr,NULL,0);
			bitInfo[pos]= value;
			pos++;
		}
	}
	fclose(fd);

	return true;
}

//-------------------------------------------------------------------------

bool CNeutrinoAPI::GetChannelEvents(void)
{
	sectionsd_getChannelEvents(eList);
	CChannelEventList::iterator eventIterator;

	ChannelListEvents.clear();

	if (eList.begin() == eList.end())
		return false;

	for (eventIterator = eList.begin(); eventIterator != eList.end(); eventIterator++)
		ChannelListEvents[(*eventIterator).get_channel_id()] = &(*eventIterator);

	return true;
}

//-------------------------------------------------------------------------

std::string CNeutrinoAPI::GetServiceName(t_channel_id channel_id)
{
	tallchans_iterator it = allchans.find(channel_id);
	if (it != allchans.end())
		return it->second.getName();
	else
		return "";
}

//-------------------------------------------------------------------------

CZapitClient::BouquetChannelList *CNeutrinoAPI::GetBouquet(unsigned int, int)
{
	//FIXME
	printf("CNeutrinoAPI::GetChannelList still used !\n");
	return NULL;
}

//-------------------------------------------------------------------------

CZapitClient::BouquetChannelList *CNeutrinoAPI::GetChannelList(int)
{
//FIXME
	printf("CNeutrinoAPI::GetChannelList still used !\n");
	return NULL;
}

//-------------------------------------------------------------------------
void CNeutrinoAPI::UpdateBouquet(unsigned int)
{
	//FIXME
}

//-------------------------------------------------------------------------
void CNeutrinoAPI::UpdateChannelList(void)
{
	//FIXME
}

//-------------------------------------------------------------------------

std::string CNeutrinoAPI::timerEventType2Str(CTimerd::CTimerEventTypes type)
{
	std::string result;
	switch (type) {
	case CTimerd::TIMER_SHUTDOWN:
		result = "Shutdown";
		break;
	case CTimerd::TIMER_NEXTPROGRAM:
		result = "Next program";
		break;
	case CTimerd::TIMER_ZAPTO:
		result = "Zap to";
		break;
	case CTimerd::TIMER_STANDBY:
		result = "Standby";
		break;
	case CTimerd::TIMER_RECORD:
		result = "Record";
		break;
	case CTimerd::TIMER_REMIND:
		result = "Reminder";
		break;
	case CTimerd::TIMER_EXEC_PLUGIN:
		result = "Execute plugin";
		break;
	case CTimerd::TIMER_SLEEPTIMER:
		result = "Sleeptimer";
		break;
	default:
		result = "Unknown";
		break;
	}
	return result;
}

//-------------------------------------------------------------------------

std::string CNeutrinoAPI::timerEventRepeat2Str(CTimerd::CTimerEventRepeat rep)
{
	std::string result;
	switch (rep) {
	case CTimerd::TIMERREPEAT_ONCE:
		result = "once";
		break;
	case CTimerd::TIMERREPEAT_DAILY:
		result = "daily";
		break;
	case CTimerd::TIMERREPEAT_WEEKLY:
		result = "weekly";
		break;
	case CTimerd::TIMERREPEAT_BIWEEKLY:
		result = "2-weekly";
		break;
	case CTimerd::TIMERREPEAT_FOURWEEKLY:
		result = "4-weekly";
		break;
	case CTimerd::TIMERREPEAT_MONTHLY:
		result = "monthly";
		break;
	case CTimerd::TIMERREPEAT_BYEVENTDESCRIPTION:
		result = "event";
		break;
	case CTimerd::TIMERREPEAT_WEEKDAYS:
		result = "weekdays";
		break;
	default:
		if (rep > CTimerd::TIMERREPEAT_WEEKDAYS)
		{
			if (rep & 0x0200)
				result += "Mo ";
			if (rep & 0x0400)
				result += "Tu ";
			if (rep & 0x0800)
				result += "We ";
			if (rep & 0x1000)
				result += "Th ";
			if (rep & 0x2000)
				result += "Fr ";
			if (rep & 0x4000)
				result += "Sa ";
			if (rep & 0x8000)
				result += "Su ";
		}
		else
			result = "Unknown";
	}
	return result;
}

