/*
 * $Id: scan.cpp,v 1.134 2004/02/10 21:01:14 metallica Exp $
 *
 * (C) 2002-2003 Andreas Oberritter <obi@tuxbox.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>

/* libevent */
#include <eventserver.h>

#include <zapit/bouquets.h>
#include <zapit/client/zapitclient.h>
#include <zapit/debug.h>
#include <zapit/getservices.h>
#include <zapit/nit.h>
#include <zapit/scan.h>
#include <zapit/sdt.h>
#include <zapit/settings.h>
#include <zapit/satconfig.h>
#include <zapit/frontend_c.h>
#include <xmlinterface.h>

extern CBouquetManager *g_bouquetManager;
extern transponder_list_t transponders; //  defined in zapit.cpp
extern tallchans allchans;   //  defined in zapit.cpp
extern int found_transponders;
extern int found_channels;
extern std::map <t_channel_id, uint8_t> service_types;
extern uint32_t  found_tv_chans;
extern uint32_t  found_radio_chans;
extern uint32_t  found_data_chans;
extern t_channel_id live_channel_id;
extern CZapitClient::bouquetMode bouquetMode;
extern CEventServer *eventServer;
extern diseqc_t diseqcType;
extern int useGotoXX;
extern int motorRotationSpeed;
extern CFrontend *frontend;
extern xmlDocPtr scanInputParser;

void SaveServices(bool tocopy);
int zapit(const t_channel_id channel_id, bool in_nvod, bool forupdate = 0, bool nowait = 0);
void *nit_thread(void * data);

static int prov_found;
static bool cable = false;
static bool terrestrial = false;
short abort_scan;
short scan_runs;
short curr_sat;
static int status = 0;
uint32_t processed_transponders;
uint32_t failed_transponders;
uint32_t  actual_freq;
uint32_t actual_polarisation;
int scan_mode = 0; /* 0 = NIT, 1 = fast */
int scan_sat_mode = 0;
CBouquetManager* scanBouquetManager;

std::map <transponder_id_t, transponder> scantransponders;// TP list to scan
std::map <transponder_id_t, transponder> scanedtransponders;// global TP list for current scan
std::map <transponder_id_t, transponder> nittransponders;

#define TIMER_START()			\
        static struct timeval tv, tv2;	\
        static unsigned int msec;	\
        gettimeofday(&tv, NULL)

#define TIMER_STOP(label)			\
        gettimeofday(&tv2, NULL);		\
	msec = tv2.tv_sec - tv.tv_sec;		\
        printf("%s: %d sec\n", label, msec)

bool tuneFrequency(FrontendParameters *feparams, uint8_t polarization, t_satellite_position satellitePosition)
{
	frontend->setInput(satellitePosition, feparams->frequency, polarization);
	int ret = frontend->driveToSatellitePosition(satellitePosition, false); //true);
	if(ret > 0) {
		printf("[scan] waiting %d seconds for motor to turn satellite dish.\n", ret);
		eventServer->sendEvent(CZapitClient::EVT_SCAN_PROVIDER, CEventServer::INITID_ZAPIT, (void *) "moving rotor", 13);
		for(int i = 0; i < ret; i++) {
			sleep(1);
			if(abort_scan)
				return false;
		}
	}
	return frontend->tuneFrequency(feparams, polarization, false);
}

int add_to_scan(transponder_id_t TsidOnid, FrontendParameters *feparams, uint8_t polarity, bool fromnit = 0)
{
	DBG("add_to_scan: freq %d pol %d tpid %llx\n", feparams->frequency, polarity, TsidOnid);
//if(fromnit) printf("add_to_scan: freq %d pol %d tpid %llx\n", feparams->frequency, polarity, TsidOnid);
	freq_id_t freq;
	if(cable || terrestrial)
		freq = feparams->frequency/100;
	else
		freq = feparams->frequency/1000;

	uint8_t poltmp1 = polarity & 1;
	uint8_t poltmp2;

	stiterator tI;

	tI = scanedtransponders.find(TsidOnid);

	if (tI != scanedtransponders.end()) {
		poltmp2 = tI->second.polarization & 1;

		if(poltmp2 != poltmp1) {
			t_transport_stream_id transport_stream_id = tI->second.transport_stream_id;
			t_original_network_id original_network_id = tI->second.original_network_id;
			uint16_t freq1 = GET_FREQ_FROM_TPID(tI->first);
			t_satellite_position satellitePosition = GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xFFF;
			if(GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xF000)
				satellitePosition = -satellitePosition;

			freq1++;
			TsidOnid = CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(
				freq1, satellitePosition, original_network_id, transport_stream_id);
				printf("[scan] add_to_scan: SAME freq %d pol1 %d pol2 %d tpid %llx\n", feparams->frequency, poltmp1, poltmp2, TsidOnid);
			feparams->frequency = feparams->frequency+1000;
			tI = scanedtransponders.find(TsidOnid);
		}
	}
        else for (tI = scanedtransponders.begin(); tI != scanedtransponders.end(); tI++) {
		poltmp2 = tI->second.polarization & 1;
                if((abs(GET_FREQ_FROM_TPID(tI->first) - freq) <= 3))
			if(poltmp2 == poltmp1)
                        	break;
        }
	if(tI == scanedtransponders.end()) {
		DBG("[scan] insert tp-id %llx freq %d rate %d\n", TsidOnid, feparams->frequency, terrestrial ? 0 : cable ? feparams->u.qam.symbol_rate : feparams->u.qpsk.symbol_rate );
		if(fromnit) {
			if(nittransponders.find(TsidOnid) == nittransponders.end()) {
				printf("[scan] insert tp-id %llx freq %d pol %d rate %d\n", TsidOnid, feparams->frequency, polarity, terrestrial ? 0 : cable? feparams->u.qam.symbol_rate : feparams->u.qpsk.symbol_rate );
				nittransponders.insert (
						std::pair <transponder_id_t, transponder> (
							TsidOnid,
							transponder ( (TsidOnid >> 16) &0xFFFF,
								TsidOnid &0xFFFF, *feparams, polarity)));
			}
		}
		else {
			found_transponders++;
			scantransponders.insert (
					std::pair <transponder_id_t, transponder> (
						TsidOnid,
						transponder ( (TsidOnid >> 16) &0xFFFF, TsidOnid &0xFFFF, *feparams, polarity)));
			scanedtransponders.insert (
					std::pair <transponder_id_t, transponder> (
						TsidOnid,
						transponder ( (TsidOnid >> 16) &0xFFFF, TsidOnid &0xFFFF, *feparams, polarity)));
		}
		return 0;
	}
	else
		tI->second.feparams.u.qpsk.fec_inner = feparams->u.qpsk.fec_inner;
	return 1;
}

static uint32_t  fake_tid, fake_nid;
int build_bf_transponder(FrontendParameters *feparams, t_satellite_position satellitePosition)
{
DBG("[scan] freq= %d, rate= %d, position %d\n", feparams->frequency, feparams->u.qam.symbol_rate, satellitePosition);
	freq_id_t freq;
	if(cable || terrestrial)
		freq = feparams->frequency/100;
	else
		freq = feparams->frequency/1000;

        if(scan_mode) {
                fake_tid++; fake_nid++;
                return add_to_scan(
                        CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(freq /*feparams->frequency/1000*/, satellitePosition, fake_nid, fake_tid),
                        feparams, 0);
        } else {
                if (!tuneFrequency(feparams, 0, 0))
                        return -1;

                return add_to_scan(get_sdt_TsidOnid(satellitePosition, freq /*0, feparams->frequency/1000*/), feparams, 0);
        }
}

#define NIT_THREAD
int get_sdts(t_satellite_position satellitePosition)
{
	transponder_id_t TsidOnid = 0;
	stiterator tI;
	stiterator stI;
	std::map <transponder_id_t, transponder>::iterator sT;

	printf("[scan] scanning tp from sat/service\n");
_repeat:
	for (tI = scantransponders.begin(); tI != scantransponders.end(); tI++) {
		if(abort_scan)
			return 0;

#if 0
		sT = scanedtransponders.find(tI->first);
		if(sT != scanedtransponders.end())
			continue;
#endif
		printf("[scan] scanning: %llx\n", tI->first);

		if(cable || terrestrial)
			actual_freq = tI->second.feparams.frequency;
		else
			actual_freq = tI->second.feparams.frequency/1000;

		processed_transponders++;
		eventServer->sendEvent(CZapitClient::EVT_SCAN_REPORT_NUM_SCANNED_TRANSPONDERS, CEventServer::INITID_ZAPIT, &processed_transponders, sizeof(processed_transponders));
		eventServer->sendEvent(CZapitClient::EVT_SCAN_PROVIDER, CEventServer::INITID_ZAPIT, (void *) " ", 2);
		eventServer->sendEvent(CZapitClient::EVT_SCAN_SERVICENAME, CEventServer::INITID_ZAPIT, (void *) " ", 2);
		eventServer->sendEvent(CZapitClient::EVT_SCAN_REPORT_FREQUENCY,CEventServer::INITID_ZAPIT, &actual_freq,sizeof(actual_freq));

		if (!(cable || terrestrial)) {
			actual_polarisation = ((tI->second.feparams.u.qpsk.symbol_rate/1000) << 16) | (tI->second.feparams.u.qpsk.fec_inner << 8) | (uint)tI->second.polarization;
			eventServer->sendEvent(CZapitClient::EVT_SCAN_REPORT_FREQUENCYP,CEventServer::INITID_ZAPIT,&actual_polarisation,sizeof(actual_polarisation));
		}

		if (!tuneFrequency(&(tI->second.feparams), tI->second.polarization, satellitePosition)) {
			failed_transponders++;
			continue;
		}
		if(abort_scan)
			return 0;

#if 0
		if(scan_mode) {
			TsidOnid = get_sdt_TsidOnid(satellitePosition, tI->second.feparams.frequency/1000);
			DBG("[scan] actual tp-id %llx\n", TsidOnid);
			if(!TsidOnid)
				continue;

			tI->second.transport_stream_id = (TsidOnid >> 16)&0xFFFF;
			tI->second.original_network_id = TsidOnid &0xFFFF;
		}
#endif
		if(abort_scan)
			return 0;

#ifdef NIT_THREAD
		pthread_t nthread;
		if(!scan_mode)
			if(pthread_create(&nthread, 0, nit_thread,  (void*)satellitePosition)) {
				ERROR("pthread_create");
				nthread = 0;
			}
#endif
		freq_id_t freq;
		if(cable || terrestrial)
			freq = tI->second.feparams.frequency/100;
		else
			freq = tI->second.feparams.frequency/1000;
		//INFO("parsing SDT (tsid:onid %04x:%04x)", tI->second.transport_stream_id, tI->second.original_network_id);
		status = parse_sdt(&tI->second.transport_stream_id, &tI->second.original_network_id, satellitePosition, freq /*tI->second.feparams.frequency/1000*/);
		if(status < 0) {
			printf("[scan] SDT failed !\n");
			continue;
		}

		TsidOnid = CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(
				freq /*tI->second.feparams.frequency/1000*/, satellitePosition, tI->second.original_network_id,
				tI->second.transport_stream_id);

		//scanedtransponders.insert(std::pair <transponder_id_t,bool> (TsidOnid, true));

		stI = transponders.find(TsidOnid);
		if(stI == transponders.end())
			transponders.insert (
					std::pair <transponder_id_t, transponder> (
						TsidOnid,
						transponder (
							tI->second.transport_stream_id,
							tI->second.feparams,
							tI->second.polarization,
							tI->second.original_network_id
							)
						)
					);
		else
			stI->second.feparams.u.qpsk.fec_inner = tI->second.feparams.u.qpsk.fec_inner;

#ifdef NIT_THREAD
		if(!scan_mode && nthread) {
			if(pthread_join(nthread, NULL))
				perror("pthread_join !!!!!!!!!");
		}
#else
		if(!scan_mode) {
			printf("[scan] trying to parse NIT\n");
			status = parse_nit(satellitePosition, freq /*tI->second.feparams.frequency/1000*/);
			if(status < 0)
				printf("[scan] NIT failed !\n");
		}
#endif
		printf("[scan] tpid ready: %llx\n", TsidOnid);
	}
	if(!scan_mode) {
		printf("[scan] found %d transponders (%d failed) and %d channels\n", found_transponders, failed_transponders, found_channels);
		scantransponders.clear();
		for (tI = nittransponders.begin(); tI != nittransponders.end(); tI++) {
//int add_to_scan(transponder_id_t TsidOnid, FrontendParameters *feparams, uint8_t polarity, bool fromnit = 0)
			add_to_scan(tI->first, &tI->second.feparams, tI->second.polarization, false);
		}
		nittransponders.clear();
		printf("\n\n[scan] found %d additional transponders from nit\n", scantransponders.size());
		if(scantransponders.size()) {
			eventServer->sendEvent ( CZapitClient::EVT_SCAN_NUM_TRANSPONDERS, CEventServer::INITID_ZAPIT,
				&found_transponders, sizeof(found_transponders));
			goto _repeat;
		}
	}
	return 0;
}

int scan_transponder(xmlNodePtr transponder, uint8_t diseqc_pos, t_satellite_position satellitePosition, bool /*satfeed*/)
{
	uint8_t polarization = 0;
	uint8_t system = 0, modulation = 1;
	int xml_fec;
	FrontendParameters feparams;
	memset(&feparams, 0x00, sizeof(FrontendParameters));

	if(terrestrial) {
		feparams.frequency = xmlGetNumericAttribute(transponder, "centre_frequency", 0);
	} else {
		feparams.frequency = xmlGetNumericAttribute(transponder, "frequency", 0);
	}

	freq_id_t freq;
        if(cable || terrestrial) {
                if (feparams.frequency > 1000*1000)
                        feparams.frequency = feparams.frequency/1000; //transponderlist was read from tuxbox
                //feparams.frequency = (int) 1000 * (int) round ((double) feparams.frequency / (double) 1000);
                freq = feparams.frequency/100;
        }
        else  {
		feparams.frequency = (int) 1000 * (int) round ((double) feparams.frequency / (double) 1000);
                freq = feparams.frequency/1000;
	}

	feparams.inversion = INVERSION_AUTO;

	if (cable) {
		feparams.u.qam.symbol_rate = xmlGetNumericAttribute(transponder, "symbol_rate", 0);
		feparams.u.qam.fec_inner = (fe_code_rate_t) xmlGetNumericAttribute(transponder, "fec_inner", 0);
		feparams.u.qam.modulation = (fe_modulation_t) xmlGetNumericAttribute(transponder, "modulation", 0);
		diseqc_pos = 0;
	}
	else if (frontend->getInfo()->type == FE_QPSK) {
		feparams.u.qpsk.symbol_rate = xmlGetNumericAttribute(transponder, "symbol_rate", 0);
		polarization = xmlGetNumericAttribute(transponder, "polarization", 0);
		system = xmlGetNumericAttribute(transponder, "system", 0);
		modulation = xmlGetNumericAttribute(transponder, "modulation", 0);
		xml_fec = xmlGetNumericAttribute(transponder, "fec_inner", 0);
		xml_fec = CFrontend::getCodeRate(xml_fec, system);
		if(modulation == 2)
			xml_fec += 9;
		feparams.u.qpsk.fec_inner = (fe_code_rate_t) xml_fec;
	} else if (frontend->getInfo()->type == FE_OFDM) {
		feparams.u.ofdm.bandwidth = static_cast<fe_bandwidth_t>(xmlGetNumericAttribute(transponder, "bandwidth", 0));
		feparams.u.ofdm.constellation = static_cast<fe_modulation_t>(xmlGetNumericAttribute(transponder, "constellation", 0));
		feparams.u.ofdm.code_rate_LP = static_cast<fe_code_rate_t>(xmlGetNumericAttribute(transponder, "code_rate_lp", 0));
		feparams.u.ofdm.code_rate_HP = static_cast<fe_code_rate_t>(xmlGetNumericAttribute(transponder, "code_rate_hp", 0));
		feparams.u.ofdm.guard_interval = static_cast<fe_guard_interval_t>(xmlGetNumericAttribute(transponder, "guard_interval", 0));
		feparams.u.ofdm.transmission_mode = static_cast<fe_transmit_mode_t>(xmlGetNumericAttribute(transponder, "transmission_mode", 0));
		feparams.u.ofdm.hierarchy_information = static_cast<fe_hierarchy_t>(xmlGetNumericAttribute(transponder, "hierarchy_information", 0));
		feparams.inversion = static_cast<fe_spectral_inversion_t>(xmlGetNumericAttribute(transponder, "inversion", 0));
	}
#if 0
        if (cable && satfeed) {
                /* build special transponder for cable with satfeed */
                status = build_bf_transponder(&feparams, satellitePosition);
        } else
#endif
	{
		/* read network information table */
		fake_tid++; fake_nid++;
		status = add_to_scan(
			CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(freq /*feparams.frequency/1000*/, satellitePosition, fake_nid, fake_tid),
			&feparams, polarization);
	}
	return 0;
}

void scan_provider(xmlNodePtr search, t_satellite_position satellitePosition, uint8_t diseqc_pos, bool satfeed)
{
	xmlNodePtr tps = NULL;
	transponder_list_t::iterator tI;
	found_transponders = 0;
	processed_transponders = 0;

	TIMER_START();
	sat_iterator_t sit = satellitePositions.find(satellitePosition);
	if(sit == satellitePositions.end()) {
		printf("[zapit] WARNING satellite position %d not found!\n", satellitePosition);
		return;
	}

	eventServer->sendEvent(CZapitClient::EVT_SCAN_REPORT_NUM_SCANNED_TRANSPONDERS, CEventServer::INITID_ZAPIT, &processed_transponders, sizeof(processed_transponders));
	eventServer->sendEvent(CZapitClient::EVT_SCAN_SATELLITE, CEventServer::INITID_ZAPIT, sit->second.name.c_str(), sit->second.name.size() + 1);
	tps = search->xmlChildrenNode;
	/* TPs from current service list */
	for(tI = transponders.begin(); tI != transponders.end(); tI++) {
		if(abort_scan)
			return;
		t_satellite_position satpos = GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xFFF;
		if(GET_SATELLITEPOSITION_FROM_TRANSPONDER_ID(tI->first) & 0xF000)
			satpos = -satpos;
		if(satpos != satellitePosition)
			continue;
		add_to_scan(tI->first, &tI->second.feparams, tI->second.polarization);
	}
	/* read all transponders */
	while ((tps = xmlGetNextOccurence(tps, "transponder")) != NULL) {
		if(abort_scan)
			return;
		scan_transponder(tps, diseqc_pos, satellitePosition, satfeed);

		/* next transponder */
		tps = tps->xmlNextNode;
	}
	eventServer->sendEvent ( CZapitClient::EVT_SCAN_NUM_TRANSPONDERS, CEventServer::INITID_ZAPIT,
					&found_transponders, sizeof(found_transponders));

	status = get_sdts(satellitePosition);

	/* channels from PAT do not have service_type set.
	 * some channels set the service_type in the BAT or the NIT.
	 * should the NIT be parsed on every transponder? */
	std::map <t_channel_id, uint8_t>::iterator stI;
	for (stI = service_types.begin(); stI != service_types.end(); stI++) {
		tallchans_iterator scI = allchans.find(stI->first);

		if (scI != allchans.end()) {
			if (scI->second.getServiceType() != stI->second) {
				switch (scI->second.getServiceType()) {
					case ST_DIGITAL_TELEVISION_SERVICE:
					case ST_DIGITAL_RADIO_SOUND_SERVICE:
					case ST_NVOD_REFERENCE_SERVICE:
					case ST_NVOD_TIME_SHIFTED_SERVICE:
						break;
					default:
						INFO("setting service_type of channel_id " PRINTF_CHANNEL_ID_TYPE " from %02x to %02x", stI->first, scI->second.getServiceType(), stI->second);
						DBG("setting service_type of channel_id " PRINTF_CHANNEL_ID_TYPE " %s from %02x to %02x", stI->first, scI->second.getName().c_str(), scI->second.getServiceType(), stI->second);
						scI->second.setServiceType(stI->second);
						break;
				}
			}
		}
	}
	TIMER_STOP("[scan] scanning took");
}

void stop_scan(const bool success)
{
	/* notify client about end of scan */
	scan_runs = 0;
	eventServer->sendEvent(success ? CZapitClient::EVT_SCAN_COMPLETE : CZapitClient::EVT_SCAN_FAILED, CEventServer::INITID_ZAPIT);
	if (scanBouquetManager) {
		scanBouquetManager->clearAll();
		delete scanBouquetManager;
		scanBouquetManager = NULL;
	}
}

void *start_scanthread(void *scanmode)
{
	scan_list_iterator_t spI;
	char providerName[80] = "";
	const char *frontendType;
	uint8_t diseqc_pos = 0;
	scanBouquetManager = new CBouquetManager();
	processed_transponders = 0;
	failed_transponders = 0;
	found_transponders = 0;
 	found_tv_chans = 0;
 	found_radio_chans = 0;
 	found_data_chans = 0;
	found_channels = 0;
	bool satfeed = false;
	int mode = (intptr_t) scanmode;
	abort_scan = 0;

	curr_sat = 0;
	scantransponders.clear();
	scanedtransponders.clear();
	nittransponders.clear();

	cable = (frontend->getInfo()->type == FE_QAM);
	terrestrial = (frontend->getInfo()->type == FE_OFDM);
	if (terrestrial)
		frontendType = "terrestrial";
	else if (cable)
		frontendType = "cable";
	else
		frontendType = "sat";

	scan_mode = mode & 0xFF;// NIT (0) or fast (1)
	scan_sat_mode = mode & 0xFF00; // single = 0, all = 1

	printf("[zapit] scan mode %s, satellites %s\n", scan_mode ? "fast" : "NIT", scan_sat_mode ? "all" : "single");
	CZapitClient myZapitClient;

	fake_tid = fake_nid = 0;
	/* get first child */
	xmlNodePtr search = xmlDocGetRootElement(scanInputParser)->xmlChildrenNode;

	/* read all sat or cable sections */
	while ((search = xmlGetNextOccurence(search, frontendType)) != NULL) {
		t_satellite_position position = xmlGetSignedNumericAttribute(search, "position", 10);

		if(cable || terrestrial) {
			strcpy(providerName, xmlGetAttribute(search, "name"));
			for (spI = scanProviders.begin(); spI != scanProviders.end(); spI++)
				if (!strcmp(spI->second.c_str(), providerName)) {
					position = spI->first;
					break;
				}

		} else {
			for (spI = scanProviders.begin(); spI != scanProviders.end(); spI++)
				if(spI->first == position)
					break;
		}

		/* provider is not wanted - jump to the next one */
		if (spI != scanProviders.end()) {
			/* get name of current satellite oder cable provider */
			strcpy(providerName, xmlGetAttribute(search, "name"));

			if (cable && xmlGetAttribute(search, "satfeed")) {
				if (!strcmp(xmlGetAttribute(search, "satfeed"), "true"))
					satfeed = true;
			}

			/* increase sat counter */
			curr_sat++;

			scantransponders.clear();
			scanedtransponders.clear();
			nittransponders.clear();

			printf("[scan] scanning %s at %d bouquetMode %d\n", providerName, position, bouquetMode);
			scan_provider(search, position, diseqc_pos, satfeed);
			if(abort_scan) {
				found_channels = 0;
				break;
			}

			if(scanBouquetManager->Bouquets.size() > 0) {
				scanBouquetManager->saveBouquets(bouquetMode, providerName);
			}
			scanBouquetManager->clearAll();
		}
		/* go to next satellite */
		search = search->xmlNextNode;
	}

	/* report status */
	printf("[scan] found %d transponders (%d failed) and %d channels\n", found_transponders, failed_transponders, found_channels);

	if (found_channels) {
		SaveServices(true);
		printf("[scan] save services done\n"); fflush(stdout);
	        g_bouquetManager->saveBouquets();
	        //g_bouquetManager->sortBouquets();
	        //g_bouquetManager->renumServices();
	        g_bouquetManager->clearAll();
		g_bouquetManager->loadBouquets();
		printf("[scan] save bouquets done\n");
		stop_scan(true);
		myZapitClient.reloadCurrentServices();
	} else {
		stop_scan(false);
		frontend->setTsidOnid(0);
		zapit(live_channel_id, 0);
	}

	scantransponders.clear();
	scanedtransponders.clear();
	nittransponders.clear();

	pthread_exit(0);
}

void * scan_transponder(void * arg)
{
	TP_params *TP = (TP_params *) arg;
	char providerName[32] = "";
	t_satellite_position satellitePosition = 0;

	prov_found = 0;
        found_transponders = 0;
        found_channels = 0;
        processed_transponders = 0;
        found_tv_chans = 0;
        found_radio_chans = 0;
        found_data_chans = 0;
	fake_tid = fake_nid = 0;
	scanBouquetManager = new CBouquetManager();
	scantransponders.clear();
	scanedtransponders.clear();
	nittransponders.clear();
	cable = (frontend->getInfo()->type == FE_QAM);
	terrestrial = (frontend->getInfo()->type == FE_OFDM);

	strcpy(providerName, scanProviders.size() > 0 ? scanProviders.begin()->second.c_str() : "unknown provider");

	satellitePosition = scanProviders.begin()->first;
	printf("[scan_transponder] scanning sat %s position %d\n", providerName, satellitePosition);
	eventServer->sendEvent(CZapitClient::EVT_SCAN_SATELLITE, CEventServer::INITID_ZAPIT, providerName, strlen(providerName) + 1);

	scan_mode = TP->scan_mode;
	TP->feparams.inversion = INVERSION_AUTO;

	if (!(cable || terrestrial)) {
		printf("[scan_transponder] freq %d rate %d fec %d pol %d NIT %s\n", TP->feparams.frequency, TP->feparams.u.qpsk.symbol_rate, TP->feparams.u.qpsk.fec_inner, TP->polarization, scan_mode ? "no" : "yes");
	} else
		printf("[scan_transponder] freq %d rate %d fec %d mod %d\n", TP->feparams.frequency, TP->feparams.u.qam.symbol_rate, TP->feparams.u.qam.fec_inner, TP->feparams.u.qam.modulation);
#if 0
        if (cable) {
                /* build special transponder for cable with satfeed */
                build_bf_transponder(&(TP->feparams), satellitePosition);
        } else
#endif
	{
		freq_id_t freq;
		if(cable || terrestrial)
			freq = TP->feparams.frequency/100;
		else
			freq = TP->feparams.frequency/1000;
		/* read network information table */
		fake_tid++; fake_nid++;
		status = add_to_scan(
			CREATE_TRANSPONDER_ID_FROM_SATELLITEPOSITION_ORIGINALNETWORK_TRANSPORTSTREAM_ID(freq /*TP->feparams.frequency/1000*/, satellitePosition, fake_nid, fake_tid),
			&TP->feparams, TP->polarization);
	}
	get_sdts(satellitePosition);

	if(abort_scan)
		found_channels = 0;

	CZapitClient myZapitClient;
	if(found_channels) {
		SaveServices(true);
		scanBouquetManager->saveBouquets(bouquetMode, providerName);
	        g_bouquetManager->saveBouquets();
	        //g_bouquetManager->sortBouquets();
	        //g_bouquetManager->renumServices();
	        g_bouquetManager->clearAll();
		g_bouquetManager->loadBouquets();
		stop_scan(true);
		myZapitClient.reloadCurrentServices();
	} else {
		stop_scan(false);
		frontend->setTsidOnid(0);
		zapit(live_channel_id, 0);
	}
	scantransponders.clear();
	scanedtransponders.clear();
	nittransponders.clear();
	if(terrestrial)
		DBG("[scan_transponder] done scan freq %d\n", TP->feparams.frequency);	
	else if(cable)
		DBG("[scan_transponder] done scan freq %d rate %d fec %d mod %d\n", TP->feparams.frequency, TP->feparams.u.qam.symbol_rate, TP->feparams.u.qam.fec_inner,TP->feparams.u.qam.modulation);
	else
		DBG("[scan_transponder] done scan freq %d rate %d fec %d pol %d\n", TP->feparams.frequency, TP->feparams.u.qpsk.symbol_rate, TP->feparams.u.qpsk.fec_inner, TP->polarization);


	pthread_exit(0);
}
