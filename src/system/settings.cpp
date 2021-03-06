/*

        $Id: settings.cpp,v 1.38 2005/03/03 19:59:41 diemade Exp $

	Neutrino-GUI  -   DBoxII-Project

	License: GPL

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA
*/

#include <cstring>
#include <system/settings.h>

#include <zapit/settings.h>
#include <zapit/satconfig.h>

const int default_timing[TIMING_SETTING_COUNT] =
{
	0,
	60,
	240,
	6,
	60,
	3
};

const neutrino_locale_t timing_setting_name[TIMING_SETTING_COUNT] =
{
	LOCALE_TIMING_MENU,
	LOCALE_TIMING_CHANLIST,
	LOCALE_TIMING_EPG,
	LOCALE_TIMING_INFOBAR,
	LOCALE_TIMING_FILEBROWSER,
	LOCALE_TIMING_NUMERICZAP
};


CScanSettings::CScanSettings(void)
	: configfile('\t')
{
	delivery_system = DVB_S;
	satNameNoDiseqc[0] = 0;
}

void CScanSettings::toSatList( CZapitClient::ScanSatelliteList& /*satList*/) const
{
#if 0
	satList.clear();
	CZapitClient::commandSetScanSatelliteList sat;

	sat.diseqc = 0;
	strncpy(sat.satName, satNameNoDiseqc, 30);

	if(diseqcMode == DISEQC_1_2)
		sat.diseqc = -1;

	if((scan_mode == 2) || (diseqcMode == DISEQC_1_2)) {
		for (int i = 0; i < MAX_SATELLITES; i++) {
			if (!strcmp(satName[i], satNameNoDiseqc)) {
				if (satDiseqc[i] != -1)
					sat.diseqc = satDiseqc[i];
				break;
			}
		}
	}
	satList.push_back(sat);
#endif
#if 0
	if  (scan_mode == 2) {
		strncpy(sat.satName, satNameNoDiseqc, 30);
		sat.diseqc = 0;
		for (int i = 0; i < MAX_SATELLITES; i++) {
			if (!strcmp(satName[i], satNameNoDiseqc)) {
				if (satDiseqc[i] != -1)
					sat.diseqc = satDiseqc[i];
				break;
			}
		}
		satList.push_back(sat);
	}
	else if  (diseqcMode == NO_DISEQC) {
		strncpy(sat.satName, satNameNoDiseqc, 30);
		sat.diseqc = 0;
		satList.push_back(sat);
	}
	else if  (diseqcMode == DISEQC_1_2) {
		strncpy(sat.satName, satNameNoDiseqc, 30);
		sat.diseqc = -1;
		for (int i = 0; i < MAX_SATELLITES; i++) {
			if (!strcmp(satName[i], satNameNoDiseqc)) {
				if (satDiseqc[i] != -1)
					sat.diseqc = satDiseqc[i];
				break;
			}
		}
		satList.push_back(sat);
	} else { // scan all sats with configured diseqc
		for( int i = 0; i < MAX_SATELLITES; i++) {
			if (satDiseqc[i] != -1) {
				strncpy(sat.satName, satName[i], 30);
				sat.diseqc = satDiseqc[i];
				satList.push_back(sat);
			}
		}
	}
#endif
}

void CScanSettings::useDefaults(const delivery_system_t _delivery_system)
{
	delivery_system = _delivery_system;
	bouquetMode     = CZapitClient::BM_UPDATEBOUQUETS;
	scanType = CZapitClient::ST_ALL;
	diseqcMode      = NO_DISEQC;
	diseqcRepeat    = 0;
	TP_mod = 3;
	TP_fec = 3;

	switch (delivery_system)
	{
	case DVB_C:
		strcpy(satNameNoDiseqc, "none");
		break;
	case DVB_S:
		strcpy(satNameNoDiseqc, "none");
		break;
	case DVB_T:
		strcpy(satNameNoDiseqc, "none");
		break;
	}
}

bool CScanSettings::loadSettings(const char * const fileName, const delivery_system_t _delivery_system)
{
	useDefaults(_delivery_system);
	if(!configfile.loadConfig(fileName))
		return false;

	if (configfile.getInt32("delivery_system", -1) != delivery_system)
	{
		// configfile is not for this delivery system
		configfile.clear();
		return false;
	}

	diseqcMode = configfile.getInt32("diseqcMode"  , diseqcMode);
	diseqcRepeat = configfile.getInt32("diseqcRepeat", diseqcRepeat);
	bouquetMode = (CZapitClient::bouquetMode) configfile.getInt32("bouquetMode" , bouquetMode);
	scanType=(CZapitClient::scanType) configfile.getInt32("scanType", scanType);
	strcpy(satNameNoDiseqc, configfile.getString("satNameNoDiseqc", satNameNoDiseqc).c_str());

    scan_fta_flag = configfile.getInt32("scan_fta_flag", 0);
	scan_mode = configfile.getInt32("scan_mode", 1); // NIT (0) or fast (1)
	TP_fec = configfile.getInt32("TP_fec", 1);
	TP_pol = configfile.getInt32("TP_pol", 0);
	TP_mod = configfile.getInt32("TP_mod", 3);
	strcpy(TP_freq, configfile.getString("TP_freq", "10100000").c_str());
	strcpy(TP_rate, configfile.getString("TP_rate", "27500000").c_str());
#if HAVE_DVB_API_VERSION >= 3
	if(TP_fec == 4) TP_fec = 5;
#endif
	scanSectionsd = configfile.getInt32("scanSectionsd", 0);

	if(delivery_system == DVB_T)
	{
		TP_bandwidth = configfile.getInt32("bandwidth", BANDWIDTH_AUTO);
		TP_code_rate_lp = configfile.getInt32("TP_code_rate_lp", FEC_AUTO);
		TP_code_rate_hp = configfile.getInt32("TP_code_rate_hp", FEC_AUTO);
		TP_guard_interval = configfile.getInt32("TP_guard_interval", GUARD_INTERVAL_AUTO);
		TP_transmission_mode = configfile.getInt32("TP_transmission_mode", TRANSMISSION_MODE_AUTO);
		TP_hierarchy_information = configfile.getInt32("TP_hierarchy_information", HIERARCHY_AUTO);
		TP_inversion = configfile.getInt32("TP_inversion", INVERSION_AUTO);
	}
	return true;
}

bool CScanSettings::saveSettings(const char * const fileName)
{
	configfile.setInt32("delivery_system", delivery_system);
	configfile.setInt32( "diseqcMode", diseqcMode );
	configfile.setInt32( "diseqcRepeat", diseqcRepeat );
	configfile.setInt32( "bouquetMode", bouquetMode );
	configfile.setInt32( "scanType", scanType );
	configfile.setString( "satNameNoDiseqc", satNameNoDiseqc );

    configfile.setInt32("scan_fta_flag", scan_fta_flag);
	configfile.setInt32("scan_mode", scan_mode);
	configfile.setInt32("TP_fec", TP_fec);
	configfile.setInt32("TP_pol", TP_pol);
	configfile.setInt32("TP_mod", TP_mod);
	configfile.setString("TP_freq", TP_freq);
	configfile.setString("TP_rate", TP_rate);
	configfile.setInt32("scanSectionsd", scanSectionsd );

	if(delivery_system == DVB_T)
	{
		configfile.setInt32("TP_bandwidth", TP_bandwidth);
		configfile.setInt32("TP_code_rate_lp", TP_code_rate_lp);
		configfile.setInt32("TP_code_rate_hp", TP_code_rate_hp);
		configfile.setInt32("TP_guard_interval", TP_guard_interval);
		configfile.setInt32("TP_transmission_mode", TP_transmission_mode);
		configfile.setInt32("TP_hierarchy_information", TP_hierarchy_information);
		configfile.setInt32("TP_inversion", TP_inversion);
	}

	if(configfile.getModifiedFlag())
		configfile.saveConfig(fileName);

	return true;
}
