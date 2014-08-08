#ifndef PVSS_H
#define PVSS_H


//========================================================

typedef struct HVchannel_pack
{

  UINT ramp_up;
  UINT ramp_down;
  UINT imax;
  UINT trip_cnt;
  UINT trip_delay;
  UINT trip_reset_delay;
  UINT trip_reset_delay_cnt;
  UINT trip_max_resets;
  UINT trip_resets_cnt;
  UINT itrip;
  UINT vtrip;
  //        UINT imon;
  UINT vmax;
  //        UINT vmon;
  UINT vset;
  UINT vcur;
  UINT relay;
  UINT fuse;
  //        HV_STATE state;
  //	HV_STATUS status;

} HVchannel_pack;


//=========================================================

typedef struct HVchannel_short
{


  UINT imon;
  UINT vmon;
  HV_STATE state;
  HV_STATUS status;


} HVchannel_short;

//=========================================================



typedef struct pvss_module_summary
{

  int    request_card;
  int    request_module;
  int    request_chan;
  HV_CMD request_cmd;

  ULONG                host_hostid;
  UINT                 host_bus;
  UINT                 host_slot;

  int             module_number;

  HV_TYPE         module_type;
  UINT            module_ID;
  int             module_busaddr;
  int             module_interlock;
  UINT            module_pos5v;
  UINT            module_neg5v;
  int             module_num_chans;
  HV_STATE        module_state;
  HV_STATUS       module_status;

  HVchannel_short  chan[36];  //chan[MODULE_MAX_CHANS];

  int master_id;
  int master_hostid;
  int master_hvcard;
  int master_busaddr;
  int master_chan;
  int master_trip_delay;
  //uncomment_chamber_status
  int chamber_complex_status;
  HVchannel_pack  chan_pack[18];  //chan[MODULE_MAX_CHANS];
  int update_value;
  /////    int status;

} pvss_module_summary;
#endif

