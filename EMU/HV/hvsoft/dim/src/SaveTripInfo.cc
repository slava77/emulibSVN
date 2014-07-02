#include "hvserver.h"
#include "hvdb.h"

bool SaveTripInfoToDB(int card, int addr, int chan, HVhistchannel& trip_data, int trip_type)
{

  HVhostcard * curdata = dataservices[card].data;
  if ( (curdata != NULL)
       	&& (curdata->module[addr].type != NONE) 
	&& (curdata->module[addr].type != MASTER) )
    {
      int ID = curdata->module[addr].ID;
      time_t tstamp = dataservices[card].databuf[addr].trip_data[chan].tstamp;
      int vtrip = curdata->module[addr].chan[chan].vtrip;
      float itrip = curdata->module[addr].chan[chan].itrip;
      if (LoadCalibration(card, addr)) {
      HVcalibdata& data = dataservices[card].calib[addr];
      vtrip = ( vtrip > 0 ) ? (int)(vtrip * data.adc[chan].A + data.adc[chan].B) : vtrip;
      itrip = ( itrip > 0 ) ? ( data.cur[chan] [curdata->module[addr].chan[chan].itrip] - 1 ) / ADC_CONV_COEF : itrip;
      }
      try
        {
          std::string trip_data_str((char *) &trip_data, sizeof(struct HVhistchannel));
          mysqlpp::Connection con(HV_DB_NAME, HV_DB_HOST, HV_DB_ADMIN, HV_DB_ADMIN_PASS, mysqlpp::use_exceptions);
          mysqlpp::Query query = con.query();
          query.clear();
          query << "insert into TRIP_INFO VALUES (" << ID << ","
          << chan << ","  << trip_type
          << "," << mysqlpp::quote_only << now(tstamp)
          << "," << vtrip
          << "," << itrip
          << "," << curdata->module[addr].chan[chan].vtrip
          << "," << curdata->module[addr].chan[chan].itrip
	  << "," << curdata->module[addr].chan[chan].trip_delay
          << "," << curdata->module[addr].chan[chan].vset
          << "," << curdata->module[addr].chan[chan].imax
          << "," << trip_data.trip_marker
          << ",\""
          << mysqlpp::escape << trip_data_str << "\")" << ends;
          query.execute();

        }
      catch (mysqlpp::BadQuery& er)
        {
          cerr << "Error: " << er.what() << endl;
          return false;
        }
      catch (exception & er)
        {
          cerr << "Error: " << er.what() << endl;
          return false;
        }

    }


  return true;
}
