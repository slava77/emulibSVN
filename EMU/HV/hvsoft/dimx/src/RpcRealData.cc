#include <iomanip>
#include <sstream>
#include <fstream>
#include "hvserver.h"
#include "hvpvss.h"

#include "pvss.h"

using namespace std;

void RpcRealData::rpcHandler()
{
  HVcmdDIM * cmdDIM = ( HVcmdDIM * ) getData();
  HVcmd * cmd = & ( cmdDIM->data );

  if (fAutoDetect)
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,"RpcRealData: Can not process request from " << HVDimServer::getClientName() << ": Waiting for autodetection to finish");
      setData( cmd, sizeof( HVcmd ) );
      return;
    }

  // ========= mycorr++ PVSS_MODE  =============================================================
  if (cmd->size == -1)
    {
      PVSS_MODE=1;
      cmd->size=0;
    }
  else if (cmd->size == -2)
    {
      PVSS_MODE=2;
      cmd->size=0;
    }
  else PVSS_MODE=0;
  /////// if(PVSS_MODE && cmd->cmd == HV_SET_MOD_STATE)(map_module_statuses[(int)cmd->card])[(int)cmd->module]=(int)cmd->data;
#ifdef PVSSDEBUG_5
  if ((int)cmd->chan >36)
    {
      LOG4CPLUS_DEBUG (hvcmdlog,rpcname << "->card:" << (int)cmd->card
                       << " module:" << (int)cmd->module
                       << " chan:" << (int)cmd->chan << " cmd:" << (int)cmd->cmd << " data:"
                       << (int)cmd->data << " size:" << (int)cmd->size);
    }
#endif
  //========== end mycorr++ PVSS_MODE ==========================================================

  if ( ( ( ( int )cmd->card != ALL ) && ( ( ( int )cmd->card + 1 ) > dataservices.size() ) )
       || ( ( ( int )cmd->module != ALL ) && (( int )cmd->module != RDBALL_TYPE)
            && (( int )cmd->module != RDB30_TYPE) && (( int )cmd->module != RDB36_TYPE)
            && (( int )cmd->module != MASTER_TYPE) && ( ( int )cmd->module > 15 ) ) )
    {
      setData( cmd, sizeof( HVcmd ) );
      return;
    }

  if ( rpcname == string( servername + "/HV_REAL_CMD" ) )
    {
      LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Command request from " << HVDimServer::getClientName()
                       << " card:" << (int)cmd->card << " module:" << (int)cmd->module
                       << " chan:" << (int)cmd->chan << " cmd:" << (int)cmd->cmd << "(" << hvcmd_str[(int)cmd->cmd]
                       << ") data:" << (int)cmd->data << " size:" << (int)cmd->size);
      pthread_mutex_lock(&CSCMonmutex);
//      commands_queue["HVCMD"].push_back(*cmdDIM);
      processRealHVCommand(& (cmdDIM->data));
      pthread_mutex_unlock(&CSCMonmutex);

      // setData((char *) &cmd, sizeof(HVcmd));
    }

  if ( rpcname == string( servername + "/HV_RAW_CMD" ) )
    {
      LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Command request from " << HVDimServer::getClientName()
                       << " card:" << (int)cmd->card << " module:" << (int)cmd->module
                       << " chan:" << (int)cmd->chan << " cmd:" << (int)cmd->cmd << "(" << hvcmd_str[(int)cmd->cmd]
                       << ") data:" << (int)cmd->data << " size:" << (int)cmd->size);
      if ( fd > 0 )
        {

          // == Disable OV for VSET in raw dac counts
          switch ( cmd->cmd )
            {
            case HV_SET_VSET:
              cmd->data += 4000<<16;
              break;
            }

          int res = ioctl( fd, HVCARD_PCIE_IOXCMD, cmd );
        }
    }

  if ( rpcname == string( servername + "/HV_RAW_DATA" ) )
    {
      // cout << "Received request for HV_RAW_DATA" << endl;
      // LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Request for raw data from " << HVDimServer::getClientName());
      if ((cmd->card >= dataservices.size()) || (cmd->module >= HVCARD_MAX_MODULES))
        {
          setData( cmdDIM, sizeof( HVcmdDIM ) );
          return;
        }

      pthread_mutex_lock(&IOmutex);
      HVhostcard * hvcard = dataservices[cmd->card].data;
      HVboardDIM realcardDIM;
      realcardDIM.ID = cmdDIM->ID;
      realcardDIM.data.card.hostid = hvcard->hostid;
      realcardDIM.data.card.bus = hvcard->bus;
      realcardDIM.data.card.slot = hvcard->slot;
      realcardDIM.data.card.dataslot = hvcard->dataslot;
      memcpy( & realcardDIM.data.module, & ( hvcard->module[cmd->module] ), sizeof( struct HVboard ) );
      pthread_mutex_unlock(&IOmutex);
      setData( ( char * ) & realcardDIM, sizeof( realcardDIM ) );
    }

  if ( rpcname == string( servername + "/HV_REAL_DATA" ) )
    {
      // LOG4CPLUS_DEBUG (hvcmdlog,rpcname << " Request for data from " << HVDimServer::getClientName());
      if ((cmd->card >= dataservices.size()) || (cmd->module >= HVCARD_MAX_MODULES))
        {
          setData( cmdDIM, sizeof( HVcmdDIM ) );
          return;
        }
      HVboardDIM boardDIM;
      HVboard board;
      if ( getRealHVData(cmd, board) )
        {
          memcpy( & boardDIM.data, & board, sizeof( board ) );
          boardDIM.ID = cmdDIM->ID;
          if ( !PVSS_MODE )
            {
              setData( ( char * ) & boardDIM, sizeof( boardDIM ) );
            }
          else
            {
	      pthread_mutex_lock(&CSCMonmutex);
              pvssInterface(board, cmd, PVSS_MODE );
	      pthread_mutex_unlock(&CSCMonmutex);
            }
        }
      else
        {
          setData( cmdDIM, sizeof( HVcmdDIM ) );
        };

    };
}

//=================== mycorr++  piece 3
int pvssChannelCopy(int i, pvss_module_summary *pvss_l, HVboard * realcard)
{


  //memcpy( & ( pvss_l.chan[i] ), & ( realcard->module.chan[i + ip * 24] ), sizeof( realcard->module.chan[i + ip * 24] ) );
  if (realcard->module.type != 0)
    {
      pvss_l->chan[i].imon=realcard->module.chan[i].imon;
      pvss_l->chan[i].vmon=realcard->module.chan[i].vmon;
    }
  else
    {
      pvss_l->chan[i].imon=0; // to avoid garbage
      pvss_l->chan[i].vmon=0; // to avoid garbage
    }

  pvss_l->chan[i].state=realcard->module.chan[i].state;
  pvss_l->chan[i].status=realcard->module.chan[i].status;


  if (i%2) return 2;

  for (int j=0; j<2; j++)
    {
      if (realcard->module.chan[i+j].ramp_up> 65000)realcard->module.chan[i+j].ramp_up=65000;
      if (realcard->module.chan[i+j].ramp_down> 65000)realcard->module.chan[i+j].ramp_down=65000;
      if (realcard->module.chan[i+j].imax> 65000)realcard->module.chan[i+j].imax=65000;
      if (realcard->module.chan[i+j].trip_cnt> 65000)realcard->module.chan[i+j].trip_cnt=65000;
      if (realcard->module.chan[i+j].trip_delay> 65000)realcard->module.chan[i+j].trip_delay=65000;
      realcard->module.chan[i+j].trip_reset_delay /= 1000;
      if (realcard->module.chan[i+j].trip_reset_delay> 65000)realcard->module.chan[i+j].trip_reset_delay=65000;
      if (realcard->module.chan[i+j].trip_reset_delay_cnt> 65000)realcard->module.chan[i+j].trip_reset_delay_cnt=65000;
      if (realcard->module.chan[i+j].trip_max_resets> 65000)realcard->module.chan[i+j].trip_max_resets=65000;
      if (realcard->module.chan[i+j].trip_resets_cnt> 65000)realcard->module.chan[i+j].trip_resets_cnt=65000;
      if (realcard->module.chan[i+j].itrip> 65000)realcard->module.chan[i+j].itrip=65000;
      if (realcard->module.chan[i+j].vtrip> 65000)realcard->module.chan[i+j].vtrip=65000;
      if (realcard->module.chan[i+j].vmax> 65000)realcard->module.chan[i+j].vmax=65000;
      if (realcard->module.chan[i+j].vset> 65000)realcard->module.chan[i+j].vset=65000;
      if (realcard->module.chan[i+j].vcur> 65000)realcard->module.chan[i+j].vcur=65000;
      if (realcard->module.chan[i+j].relay> 65000)realcard->module.chan[i+j].relay=65000;
      if (realcard->module.chan[i+j].fuse> 65000)realcard->module.chan[i+j].fuse=65000;

    }


  pvss_l->chan_pack[i/2].ramp_up=realcard->module.chan[i].ramp_up;
  pvss_l->chan_pack[i/2].ramp_up |=  (realcard->module.chan[i+1].ramp_up << 16);

  pvss_l->chan_pack[i/2].ramp_down=realcard->module.chan[i].ramp_down;
  pvss_l->chan_pack[i/2].ramp_down |=  (realcard->module.chan[i+1].ramp_down << 16);

  pvss_l->chan_pack[i/2].imax=realcard->module.chan[i].imax;
  pvss_l->chan_pack[i/2].imax |=  (realcard->module.chan[i+1].imax << 16);

  pvss_l->chan_pack[i/2].trip_cnt=realcard->module.chan[i].trip_cnt;
  pvss_l->chan_pack[i/2].trip_cnt |=  (realcard->module.chan[i+1].trip_cnt << 16);

  pvss_l->chan_pack[i/2].trip_delay=realcard->module.chan[i].trip_delay;
  pvss_l->chan_pack[i/2].trip_delay |=  (realcard->module.chan[i+1].trip_delay << 16);

  pvss_l->chan_pack[i/2].trip_reset_delay=realcard->module.chan[i].trip_reset_delay;
  pvss_l->chan_pack[i/2].trip_reset_delay |=  (realcard->module.chan[i+1].trip_reset_delay << 16);

  pvss_l->chan_pack[i/2].trip_reset_delay_cnt=realcard->module.chan[i].trip_reset_delay_cnt;
  pvss_l->chan_pack[i/2].trip_reset_delay_cnt |=  (realcard->module.chan[i+1].trip_reset_delay_cnt << 16);

  pvss_l->chan_pack[i/2].trip_max_resets=realcard->module.chan[i].trip_max_resets;
  pvss_l->chan_pack[i/2].trip_max_resets |=  (realcard->module.chan[i+1].trip_max_resets << 16);

  pvss_l->chan_pack[i/2].trip_resets_cnt=realcard->module.chan[i].trip_resets_cnt;
  pvss_l->chan_pack[i/2].trip_resets_cnt |=  (realcard->module.chan[i+1].trip_resets_cnt << 16);

  pvss_l->chan_pack[i/2].itrip=realcard->module.chan[i].itrip;
  pvss_l->chan_pack[i/2].itrip |=  (realcard->module.chan[i+1].itrip << 16);

  pvss_l->chan_pack[i/2].vtrip=realcard->module.chan[i].vtrip;
  pvss_l->chan_pack[i/2].vtrip |=  (realcard->module.chan[i+1].vtrip << 16);

  pvss_l->chan_pack[i/2].vmax=realcard->module.chan[i].vmax;
  pvss_l->chan_pack[i/2].vmax |=  (realcard->module.chan[i+1].vmax << 16);

  pvss_l->chan_pack[i/2].vset=realcard->module.chan[i].vset;
  pvss_l->chan_pack[i/2].vset |=  (realcard->module.chan[i+1].vset << 16);

  pvss_l->chan_pack[i/2].vcur=realcard->module.chan[i].vcur;
  pvss_l->chan_pack[i/2].vcur |=  (realcard->module.chan[i+1].vcur << 16);

  pvss_l->chan_pack[i/2].relay=realcard->module.chan[i].relay;
  pvss_l->chan_pack[i/2].relay |=  (realcard->module.chan[i+1].relay << 16);

  pvss_l->chan_pack[i/2].fuse=realcard->module.chan[i].fuse;
  pvss_l->chan_pack[i/2].fuse |=  (realcard->module.chan[i+1].fuse << 16);


  return 1;

  // for(int j=0;j<sizeof(struct HVchannel_pack);j++ ){
  //     if(  *((int *)   &(pvss_l->chan_pack[j]) +j )   > 65000  ){};
  //}


}

//==========================================================================
int RpcRealData::pvssInterface( HVboard& board, HVcmd * cmd, int mode )
{

  /* static int temp=0; temp++; */

  //	PVSS_QUEUE_FILL=1;

  bool UPDATE_FLAG = false;
  // float smooth_voltage_range=30; // 50 Volts
  // float smooth_current_range=2;  // 0.2 mkA

  pvss_module_summary pvss_l;
  int module_complex_state;
  int module_complex_state_1;
  int module_complex_state_2;
  int module_complex_trip_1;
  int module_complex_trip_2;

  int module_complex_status_ch1;
  int module_complex_status_ch2;

  pvss_l.update_value=mode;
  pvss_l.host_hostid = board.card.hostid;
  pvss_l.host_bus = board.card.bus;
  pvss_l.host_slot = board.card.dataslot;

  //	 int		  request_card;
  //	 int		  request_module;
  //	 int		  request_chan;
  //	 HV_CMD	request_cmd;

  pvss_l.request_card = cmd->card;
  pvss_l.request_module = cmd->module;
  pvss_l.request_chan = cmd->chan;
  pvss_l.request_cmd = cmd->cmd;

  int ip = 0;
  int im = board.module.busaddr;
  //					  for(int ip=0;ip<2;ip++){
  if ( mode == 1 && board.module.type != MASTER ) UPDATE_FLAG = true;
  /* if(cmd->module!=im && cmd->module !=255)continue; if(cmd->module == 255){
     char im_text[20], card_text[20];
     sprintf(im_text,"%d",im); sprintf(card_text,"%d",card_database[cmd->card]); int	found=0;
     for(int	i=0;i<module_database.size();i++){ if(host_database+";"+card_text+";"+im_text == module_database[i]){ found=1;
     break; } } if(!found)continue; } */
  pvss_l.module_number = im; //+1000*ip;

  pvss_l.module_type = board.module.type;
  pvss_l.module_ID = board.module.ID;
  pvss_l.module_busaddr = board.module.busaddr;
  pvss_l.module_interlock = board.module.interlock;
  pvss_l.module_pos5v = board.module.pos5v;
  pvss_l.module_neg5v = board.module.neg5v;
  pvss_l.module_num_chans = board.module.num_chans;
  pvss_l.module_state = board.module.state;
  pvss_l.module_status = board.module.status;

  pvss_l.master_id = board.module.master_id;
  pvss_l.master_hostid = board.module.master_hostid;
  pvss_l.master_hvcard = board.module.master_hvcard;
  pvss_l.master_busaddr = board.module.master_busaddr;
  pvss_l.master_chan = board.module.master_chan;
  pvss_l.master_trip_delay = board.module.master_trip_delay;

  /* if(pvss_l.module_status	>=5){ printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);

     } */
  //		printf("4\n");

  ///////	if(im==1)board.module[im].chan[10].status=(HV_STATE)5;
  //-------------------------
  int max_ch_number = board.module.num_chans;
  if ( board.module.type < 1 || board.module.type > 3 )  // case of LV Winener PL512 is off
    {
      max_ch_number = 36;
    }
  for ( int i = 0; i < max_ch_number; i++ )
    {

      if ( board.module.type < 1 || board.module.type > 3 )  // case of LV Winener PL512 is off
        {
          board.module.chan[i + ip * 24].status = (HV_STATE)4; //DEAD
        }

      ////if(board.module[im].type!=MASTER){ ///////
      if ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
           || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN ) UPDATE_FLAG = true;

      //      if(i==0)printf("type=%d \n",board.module.type);


      if ( board.module.chan[i + ip * 24].status != pvss_channel_status_save[cmd->card] [im] [i + ip * 24] )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update onc more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }
      /////////////if(board.module[im].chan[i+ip*24].status >= (HV_STATUS)HV_STATE_ILOCK)	UPDATE_FLAG=true;
      ///}

      if ( (abs( ( int )( board.module.chan[i + ip * 24].vmon
                          - pvss_channel_vmon_save[cmd->card] [im] [i + ip * 24] ) ) > PVSS_VMON_THRESHOLD )
           &&  (board.module.type > 0) )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update one more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }


//corr2
      if ( board.module.chan[i + ip * 24].imax != pvss_channel_imax_save[cmd->card] [im] [i + ip * 24] )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update onc more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }


      /* =VB= Add monitoring of current with threshold to trigger PVSS update */
      if ( (abs( ( int )( board.module.chan[i + ip * 24].imon
                          - pvss_channel_imon_save[cmd->card] [im] [i + ip * 24] ) ) > PVSS_IMON_THRESHOLD )
           &&  (board.module.type > 0) )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to     update one more time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }


      /*						 if(abs((int)(board.module[im].chan[i+ip*24].vmon-board.module[im].chan[i+ip*24].cur)) >	10){
      // board.module[im].chan[i+ip*24].status=101; pvss_l.module_status=board.module[im].status=101;							 } */

      pvss_channel_status_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].status;
      if (mode==2)
        {
          pvss_channel_vmon_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].vmon;
          pvss_channel_imon_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].imon;
        }
      pvss_channel_imax_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].imax;
    }
  ////if(board.module[im].type!=MASTER){ ///////
  if ( board.module.status != pvss_module_status_save[cmd->card] [im] ) UPDATE_FLAG = true;

  /////////////////////if(board.module[im].status	>= (HV_STATUS)HV_STATE_ILOCK )UPDATE_FLAG=true;
  ////}
  /* if(board.module[im].type==RDB36	|| board.module[im].type==RDB30){ if(MASTER_UPDATE_FLAG)UPDATE_FLAG=true; } */

  pvss_module_status_save[cmd->card] [im] = board.module.status;

  //-------------------------

  if ( board.module.type == MASTER )
    {

      /* if(UPDATE_FLAG)MASTER_UPDATE_FLAG=true; else MASTER_UPDATE_FLAG=false; */
      if ( mode == 1 ) UPDATE_FLAG = true;

    }
  //------------------------

  module_complex_state_1 = 0;
  module_complex_state_2 = 0;
  module_complex_trip_1 = 0;
  module_complex_trip_2 = 0;
  module_complex_status_ch1 = 1;
  module_complex_status_ch2 = 1;
  /* if(temp	> 10 &&	im==1){ board.module[1].chan[8]=6; } */


  int number_state_off_channels_ch1=0;
  int number_state_off_channels_ch2=0;
  for ( int i = 0; i < 36; i++ )
    {
      //memcpy( & ( pvss_l.chan[i] ), & ( board.module.chan[i + ip * 24] ), sizeof( board.module.chan[i + ip * 24] ) );
      pvssChannelCopy(i, &pvss_l, &board);
      //printf("imon=%d vmon=%d \n",pvss_l.chan[i].imon,pvss_l.chan[i].vmon);
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //this is test of trip:	if(im==0 &&	i==0) pvss_l.chan[i].status=(HV_STATE)6;
      // corr2: to allow trips if the channel is OFF
      // if ( pvss_l.chan[i].state == HV_STATE_OFF ) pvss_l.chan[i].status = HV_STATE_OFF;
      //========================================================
      if ( board.module.type == RDB36 )
        {
          if ( i < 18 )
            {
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_1 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch1++;
              if (number_state_off_channels_ch1==18)module_complex_status_ch1 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch1 = 0;
            }
          else if ( i >= 18 )
            {
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_2 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_2 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch2++;
              if (number_state_off_channels_ch2==18)module_complex_status_ch2 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch2 = 0;
            }
        }
      else if ( board.module.type == RDB30 )
        {
          if ( i <= 29 )
            {
              module_complex_status_ch2 = 0;
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_1 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch1++;
              if (number_state_off_channels_ch1==30)module_complex_status_ch1 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch1 = 0;
            }
        }
      else if ( board.module.type == MASTER )
        {
          if ( i <= 8 && board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
        }

    } // for ( int i = 0; i < 36; i++ )


  //uncomment_chamber_status
  pvss_l.chamber_complex_status = 1000 * module_complex_status_ch2 + module_complex_status_ch1;

  int is_part_1 = 0;
  int is_part_2 = 0;
  if ( shmem_address->module_statuses_count[pvss_l.request_card] [im] [0]
       /*|| (module_complex_state_1==0	&& module_complex_trip_1==1)*/ ) is_part_1 = 1; //	trip in	the	all	channels !!!
  if ( shmem_address->module_statuses_count[pvss_l.request_card] [im] [1]
       /*|| (module_complex_state_2==0	&& module_complex_trip_2==1)*/ ) is_part_2 = 1; //	trip in	the	all	channels !!!


  int map_module_status = shmem_address->map_module_statuses[pvss_l.request_card] [im];

  map_module_status = ( ( map_module_status / 1000 ) * is_part_2 ) * 1000 + ( ( map_module_status % 1000 ) * is_part_1 );

  if ( map_module_status % 1000 == 0 ) module_complex_state_1 = module_complex_state_1;
  else if ( map_module_status % 1000 == 1 ) module_complex_state_1 = 0;
  else if ( map_module_status % 1000 == 2 ) module_complex_state_1 = 1;

  if ( map_module_status / 1000 == 0 ) module_complex_state_2 = module_complex_state_2;
  else if ( map_module_status / 1000 == 1 ) module_complex_state_2 = 0;
  else if ( map_module_status / 1000 == 2 ) module_complex_state_2 = 1;


  module_complex_state = 1000 * module_complex_state_2 + module_complex_state_1;
  pvss_l.module_state = ( HV_STATE )module_complex_state;

  if ( pvss_l.module_state != pvss_module_state_save[pvss_l.request_card] [im] ) UPDATE_FLAG = true;
  pvss_module_state_save[pvss_l.request_card] [im] = pvss_l.module_state;

  //// if(im==0)pvss_l.module_status=(HV_STATE)5;	// test	of interlock
  //---------------------
  //// pvss_l.module_state=(HV_STATE)1;//board.module[im].chan[0].state;


#ifdef PVSSDEBUG_5
  if ( map_module_status != 0 )
    {
      printf( " ************ module_number=%d save_state=%d module_state=%d %d	%d \n", pvss_l.module_number, map_module_status,
              pvss_l.module_state, shmem_address->module_statuses_count[pvss_l.request_card] [im] [0],
              shmem_address->module_statuses_count[pvss_l.request_card] [im] [1] );
    }
  else
    {

      printf( " ------------ module_number=%d save_state=%d module_state=%d %d	%d	module_status=%d \n", pvss_l.module_number,
              map_module_status, pvss_l.module_state, shmem_address->module_statuses_count[pvss_l.request_card] [im] [0],
              shmem_address->module_statuses_count[pvss_l.request_card] [im] [1], pvss_l.module_status );
    }
#endif

  if (pvss_l.module_status > HV_STATE_ILOCK)pvss_l.module_status=HV_STATE_ON;


  if ( UPDATE_FLAG )
    {
      char tmp[20];
      int db_index = -1;
      sprintf( tmp, ";%d;%d", pvss_l.host_slot, pvss_l.module_number );
      for ( int i10 = 0; i10 < module_database.size(); i10++ )
        {
          if ( module_database[i10] == string( tmp ) )
            {
              db_index = i10;
              break;
            }
        }
      pvss_queue[0].push_back( pvss_l );
      db_index_queue[0].push_back( db_index );

      ////							printf("db_index=%d\n",db_index);

      /* if(pvss_l.host_slot==16) pvss_queue[0].push_back(pvss_l); else if(pvss_l.host_slot==18)
      pvss_queue[1].push_back(pvss_l); else if(pvss_l.host_slot==20) pvss_queue[2].push_back(pvss_l); */
    }


#ifdef PVSSDEBUG_10	// this	printf is needed to	know the host_id, slot number (useful at PC	change)
  printf( " %d	%d %d		%d %d %d   host=%d bus=%d slot=%d\n", cmd->card, cmd->module, cmd->chan, cmd->cmd, cmd->data,
          cmd->size, pvss_l.host_hostid, pvss_l.host_bus, pvss_l.host_slot );
#endif

  // PVSS_QUEUE_FILL=0;

  return 0;

}
//=======================mycorr++ end of piece 3

//==========================================================================
int pvssInterfaceRealHVData(HVcmd * cmd)
{
  // LOG4CPLUS_DEBUG(hvlog, "Update Real HV Data");

  /* static int temp=0; temp++; */

  //	PVSS_QUEUE_FILL=1;

  if (cmd->size == -1)
    {
      PVSS_MODE=1;
      cmd->size=0;
    }
  else if (cmd->size == -2)
    {
      PVSS_MODE=2;
      cmd->size=0;
    }
  else PVSS_MODE=0;

  bool UPDATE_FLAG = false;
  // float smooth_voltage_range=30; // 50 Volts
  // float smooth_current_range=2;  // 0.2 mkA

  pvss_module_summary pvss_l;
  int module_complex_state;
  int module_complex_state_1;
  int module_complex_state_2;
  int module_complex_trip_1;
  int module_complex_trip_2;

  int module_complex_status_ch1;
  int module_complex_status_ch2;

  HVboard board;
  memset(&board, 0, sizeof(board));
  if ( !getRealHVData(cmd, board) ) return 0;

  pvss_l.update_value=PVSS_MODE;
  pvss_l.host_hostid = board.card.hostid;
  pvss_l.host_bus = board.card.bus;
  pvss_l.host_slot = board.card.dataslot;

  //	 int		  request_card;
  //	 int		  request_module;
  //	 int		  request_chan;
  //	 HV_CMD	request_cmd;

  pvss_l.request_card = cmd->card;
  pvss_l.request_module = cmd->module;
  pvss_l.request_chan = cmd->chan;
  pvss_l.request_cmd = cmd->cmd;

  int ip = 0;
  int im = board.module.busaddr;
  //					  for(int ip=0;ip<2;ip++){
  if ( PVSS_MODE == 1 && board.module.type != MASTER ) UPDATE_FLAG = true;
  /* if(cmd->module!=im && cmd->module !=255)continue; if(cmd->module == 255){
     char im_text[20], card_text[20];
     sprintf(im_text,"%d",im); sprintf(card_text,"%d",card_database[cmd->card]); int	found=0;
     for(int	i=0;i<module_database.size();i++){ if(host_database+";"+card_text+";"+im_text == module_database[i]){ found=1;
     break; } } if(!found)continue; } */
  pvss_l.module_number = im; //+1000*ip;

  pvss_l.module_type = board.module.type;
  pvss_l.module_ID = board.module.ID;
  pvss_l.module_busaddr = board.module.busaddr;
  pvss_l.module_interlock = board.module.interlock;
  pvss_l.module_pos5v = board.module.pos5v;
  pvss_l.module_neg5v = board.module.neg5v;
  pvss_l.module_num_chans = board.module.num_chans;
  pvss_l.module_state = board.module.state;
  pvss_l.module_status = board.module.status;

  pvss_l.master_id = board.module.master_id;
  pvss_l.master_hostid = board.module.master_hostid;
  pvss_l.master_hvcard = board.module.master_hvcard;
  pvss_l.master_busaddr = board.module.master_busaddr;
  pvss_l.master_chan = board.module.master_chan;
  pvss_l.master_trip_delay = board.module.master_trip_delay;

  /* if(pvss_l.module_status	>=5){ printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);
     printf("%d +++++++++++++++++++++++++++++++++ \n",pvss_l.module_status);

     } */
  //		printf("4\n");

  ///////	if(im==1)board.module[im].chan[10].status=(HV_STATE)5;
  //-------------------------
  int max_ch_number = board.module.num_chans;
  if ( board.module.type < 1 || board.module.type > 3 )  // case of LV Winener PL512 is off
    {
      max_ch_number = 36;
    }
  for ( int i = 0; i < max_ch_number; i++ )
    {

      if ( board.module.type < 1 || board.module.type > 3 )  // case of LV Winener PL512 is off
        {
          board.module.chan[i + ip * 24].status = (HV_STATE)4; //DEAD
        }

      ////if(board.module[im].type!=MASTER){ ///////
      if ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
           || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN ) UPDATE_FLAG = true;

      //      if(i==0)printf("type=%d \n",board.module.type);


      if ( board.module.chan[i + ip * 24].status != pvss_channel_status_save[cmd->card] [im] [i + ip * 24] )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update onc more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }
      /////////////if(board.module[im].chan[i+ip*24].status >= (HV_STATUS)HV_STATE_ILOCK)	UPDATE_FLAG=true;
      ///}

      if ( (abs( ( int )( board.module.chan[i + ip * 24].vmon
                          - pvss_channel_vmon_save[cmd->card] [im] [i + ip * 24] ) ) > PVSS_VMON_THRESHOLD )
           &&  (board.module.type > 0) )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update one more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }


//corr2
      if ( board.module.chan[i + ip * 24].imax != pvss_channel_imax_save[cmd->card] [im] [i + ip * 24] )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to	update onc more	time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }

      /* =VB= Add monitoring of current with threshold to trigger PVSS update */
      if ( (abs( ( int )( board.module.chan[i + ip * 24].imon
                          - pvss_channel_imon_save[cmd->card] [im] [i + ip * 24] ) ) > PVSS_IMON_THRESHOLD )
           &&  (board.module.type > 0) )
        {
          UPDATE_FLAG = true;
          pvss_module_changestatus_save[cmd->card] [im] = 1;
        }
      else
        {
          if ( pvss_module_changestatus_save[cmd->card] [im] )
            { // to     update one more time
              UPDATE_FLAG = true;
              pvss_module_changestatus_save[cmd->card] [im] = 0;
            }
        }

      /*						 if(abs((int)(board.module[im].chan[i+ip*24].vmon-board.module[im].chan[i+ip*24].cur)) >	10){
      // board.module[im].chan[i+ip*24].status=101; pvss_l.module_status=board.module[im].status=101;							 } */

      pvss_channel_status_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].status;
      if (PVSS_MODE==2)
        {
          pvss_channel_vmon_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].vmon;
          pvss_channel_imon_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].imon;
        }
      pvss_channel_imax_save[cmd->card] [im] [i + ip * 24] = board.module.chan[i + ip * 24].imax;
    }
  ////if(board.module[im].type!=MASTER){ ///////
  if ( board.module.status != pvss_module_status_save[cmd->card] [im] ) UPDATE_FLAG = true;

  /////////////////////if(board.module[im].status	>= (HV_STATUS)HV_STATE_ILOCK )UPDATE_FLAG=true;
  ////}
  /* if(board.module[im].type==RDB36	|| board.module[im].type==RDB30){ if(MASTER_UPDATE_FLAG)UPDATE_FLAG=true; } */

  pvss_module_status_save[cmd->card] [im] = board.module.status;

  //-------------------------

  if ( board.module.type == MASTER )
    {

      /* if(UPDATE_FLAG)MASTER_UPDATE_FLAG=true; else MASTER_UPDATE_FLAG=false; */
      if ( PVSS_MODE == 1 ) UPDATE_FLAG = true;

    }
  //------------------------

  module_complex_state_1 = 0;
  module_complex_state_2 = 0;
  module_complex_trip_1 = 0;
  module_complex_trip_2 = 0;
  module_complex_status_ch1 = 1;
  module_complex_status_ch2 = 1;
  /* if(temp	> 10 &&	im==1){ board.module[1].chan[8]=6; } */


  int number_state_off_channels_ch1=0;
  int number_state_off_channels_ch2=0;
  for ( int i = 0; i < 36; i++ )
    {
      //memcpy( & ( pvss_l.chan[i] ), & ( board.module.chan[i + ip * 24] ), sizeof( board.module.chan[i + ip * 24] ) );
      pvssChannelCopy(i, &pvss_l, &board);
      //printf("imon=%d vmon=%d \n",pvss_l.chan[i].imon,pvss_l.chan[i].vmon);
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //======= THEN (AFTER TEST)	SHOULD BE UNCOMMENTED ========
      //this is test of trip:	if(im==0 &&	i==0) pvss_l.chan[i].status=(HV_STATE)6;
      // corr2: to allow trips if the channel is OFF
      // if ( pvss_l.chan[i].state == HV_STATE_OFF ) pvss_l.chan[i].status = HV_STATE_OFF;
      //========================================================
      if ( board.module.type == RDB36 )
        {
          if ( i < 18 )
            {
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_1 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch1++;
              if (number_state_off_channels_ch1==18)module_complex_status_ch1 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch1 = 0;
            }
          else if ( i >= 18 )
            {
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_2 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_2 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch2++;
              if (number_state_off_channels_ch2==18)module_complex_status_ch2 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch2 = 0;
            }
        }
      else if ( board.module.type == RDB30 )
        {
          if ( i <= 29 )
            {
              module_complex_status_ch2 = 0;
              if ( board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
              if ( board.module.chan[i + ip * 24].status >= ( HV_STATUS )HV_STATE_INT_TRIP ) module_complex_trip_1 = 1;
              if ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_OFF)number_state_off_channels_ch1++;
              if (number_state_off_channels_ch1==30)module_complex_status_ch1 = 0;

              if ( ( board.module.chan[i + ip * 24].state == ( HV_STATUS )HV_STATE_ON
                     && ( board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_UP
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_RAMP_DOWN
                          || board.module.chan[i + ip * 24].status == ( HV_STATUS )HV_STATE_OFF ) ) ) module_complex_status_ch1 = 0;
            }
        }
      else if ( board.module.type == MASTER )
        {
          if ( i <= 8 && board.module.chan[i + ip * 24].state == 1 ) module_complex_state_1 = 1;
        }

    } // for ( int i = 0; i < 36; i++ )


  //uncomment_chamber_status
  pvss_l.chamber_complex_status = 1000 * module_complex_status_ch2 + module_complex_status_ch1;

  int is_part_1 = 0;
  int is_part_2 = 0;
  if ( shmem_address->module_statuses_count[pvss_l.request_card] [im] [0]
       /*|| (module_complex_state_1==0	&& module_complex_trip_1==1)*/ ) is_part_1 = 1; //	trip in	the	all	channels !!!
  if ( shmem_address->module_statuses_count[pvss_l.request_card] [im] [1]
       /*|| (module_complex_state_2==0	&& module_complex_trip_2==1)*/ ) is_part_2 = 1; //	trip in	the	all	channels !!!


  int map_module_status = shmem_address->map_module_statuses[pvss_l.request_card] [im];

  map_module_status = ( ( map_module_status / 1000 ) * is_part_2 ) * 1000 + ( ( map_module_status % 1000 ) * is_part_1 );

  if ( map_module_status % 1000 == 0 ) module_complex_state_1 = module_complex_state_1;
  else if ( map_module_status % 1000 == 1 ) module_complex_state_1 = 0;
  else if ( map_module_status % 1000 == 2 ) module_complex_state_1 = 1;

  if ( map_module_status / 1000 == 0 ) module_complex_state_2 = module_complex_state_2;
  else if ( map_module_status / 1000 == 1 ) module_complex_state_2 = 0;
  else if ( map_module_status / 1000 == 2 ) module_complex_state_2 = 1;


  module_complex_state = 1000 * module_complex_state_2 + module_complex_state_1;
  pvss_l.module_state = ( HV_STATE )module_complex_state;

  if ( pvss_l.module_state != pvss_module_state_save[pvss_l.request_card] [im] ) UPDATE_FLAG = true;
  pvss_module_state_save[pvss_l.request_card] [im] = pvss_l.module_state;

  //// if(im==0)pvss_l.module_status=(HV_STATE)5;	// test	of interlock
  //---------------------
  //// pvss_l.module_state=(HV_STATE)1;//board.module[im].chan[0].state;


#ifdef PVSSDEBUG_5
  if ( map_module_status != 0 )
    {
      printf( " ************ module_number=%d save_state=%d module_state=%d %d	%d \n", pvss_l.module_number, map_module_status,
              pvss_l.module_state, shmem_address->module_statuses_count[pvss_l.request_card] [im] [0],
              shmem_address->module_statuses_count[pvss_l.request_card] [im] [1] );
    }
  else
    {

      printf( " ------------ module_number=%d save_state=%d module_state=%d %d	%d	module_status=%d \n", pvss_l.module_number,
              map_module_status, pvss_l.module_state, shmem_address->module_statuses_count[pvss_l.request_card] [im] [0],
              shmem_address->module_statuses_count[pvss_l.request_card] [im] [1], pvss_l.module_status );
    }
#endif

  if (pvss_l.module_status > HV_STATE_ILOCK)pvss_l.module_status=HV_STATE_ON;


  if ( UPDATE_FLAG )
    {
      char tmp[20];
      int db_index = -1;
      sprintf( tmp, ";%d;%d", pvss_l.host_slot, pvss_l.module_number );
      for ( int i10 = 0; i10 < module_database.size(); i10++ )
        {
          if ( module_database[i10] == string( tmp ) )
            {
              db_index = i10;
              break;
            }
        }
      pvss_queue[0].push_back( pvss_l );
      db_index_queue[0].push_back( db_index );

      ////							printf("db_index=%d\n",db_index);

      /* if(pvss_l.host_slot==16) pvss_queue[0].push_back(pvss_l); else if(pvss_l.host_slot==18)
      pvss_queue[1].push_back(pvss_l); else if(pvss_l.host_slot==20) pvss_queue[2].push_back(pvss_l); */
    }


#ifdef PVSSDEBUG_10	// this	printf is needed to	know the host_id, slot number (useful at PC	change)
  printf( " %d	%d %d		%d %d %d   host=%d bus=%d slot=%d\n", cmd->card, cmd->module, cmd->chan, cmd->cmd, cmd->data,
          cmd->size, pvss_l.host_hostid, pvss_l.host_bus, pvss_l.host_slot );
#endif

  // PVSS_QUEUE_FILL=0;

  return 0;

}


int processRealHVCommand(HVcmd* cmd)
{
//	if (cmd_cntr%100==0) LOG4CPLUS_DEBUG(hvlog, cmd_cntr << " Process Real HV Command");
  cmd_cntr++;
  bool fTypeFilter = ((cmd->module >= RDBALL_TYPE) && (cmd->module <= MASTER_TYPE)) ? true: false;
  HVcmd realcmd = *cmd;
  range itr_card, itr_module, itr_chan;
  itr_card.start = ( cmd->card == ALL ) ? 0 : cmd->card;
  itr_card.end = ( cmd->card == ALL ) ? dataservices.size() : cmd->card + 1;
  itr_card.len = itr_card.end - itr_card.start;
  itr_module.start = ( cmd->module >= HVCARD_MAX_MODULES ) ? ( int )0 : cmd->module;
  itr_module.end = ( cmd->module >= HVCARD_MAX_MODULES) ?
                   ( int )HVCARD_MAX_MODULES : ( cmd->module + 1 );
  itr_module.len = itr_module.end - itr_module.start;
  HVhistmodule trip_data;

  // --> PVSS related part
  if (cmd->chan==253 && cmd->cmd <=1)
    {

      shmem_address->map_module_statuses[cmd->card][cmd->module]=
        (shmem_address->map_module_statuses[cmd->card][cmd->module]/1000)*1000
        +(cmd->cmd+1);
      shmem_address->module_statuses_count[cmd->card][cmd->module][0]=STATUS_DELAY_KEEP; // part 1 of the module

    }
  else if (cmd->chan==254 && cmd->cmd <=1)
    {

      shmem_address->map_module_statuses[cmd->card][cmd->module]=
        (shmem_address->map_module_statuses[cmd->card][cmd->module]%1000)
        +(cmd->cmd+1)*1000;
      shmem_address->module_statuses_count[cmd->card][cmd->module][1]=STATUS_DELAY_KEEP; // part 2 of the module
    }
  else
    {
      if (cmd->chan==255 && cmd->cmd <=1)
        {
          shmem_address->map_module_statuses[cmd->card][cmd->module]=cmd->cmd+1;
          shmem_address->module_statuses_count[cmd->card][cmd->module][0]=STATUS_DELAY_KEEP; // part 1 of the module
        }
    }
  // <--



  if (cmd->cmd == HV_DO_CALIB)
    {
      // LOG4CPLUS_INFO (hvlog, "Run Calibration");
      if (cmd->data > 0)
        {
          fCalibrateOnlyTurnedOn = true;
        }
      else fCalibrateOnlyTurnedOn = false;
      fDoCalibration = true;
    }


  for ( int card = 0; card < itr_card.len; card++ )
    {
      int i = card + itr_card.start;
      int res=0;
      realcmd.card = i;

      if (cmd->cmd == HV_DETECT)
        {
          LOG4CPLUS_INFO (hvlog, "Forced Autodetection for hostcard " << i);
          res = ioctl( fd, HVCARD_PCIE_IOXCMD, & realcmd );
          fAutoDetect = true;
          continue;
          //  break;
        }

      for ( int module = 0; module < itr_module.len; module++ )
        {
          int j = module + itr_module.start;
          HVhistmodule mod_hist;
          memset(&mod_hist,0, sizeof(struct HVhistmodule));

          bool fSkip = false;
          HV_TYPE mod_type = dataservices[i].data->module[j].type;
          if (fTypeFilter)
            {
              switch (cmd->module)
                {
                case RDBALL_TYPE:
                  if (mod_type != RDB30 && mod_type != RDB36) fSkip = true;
                  break;
                case RDB30_TYPE:
                  if (mod_type != RDB30) fSkip = true;
                  break;
                case RDB36_TYPE:
                  if (mod_type != RDB36) fSkip = true;
                  break;
                case MASTER_TYPE:
                  if (mod_type != MASTER) fSkip = true;
                  break;
                }
            }
          if (fSkip) continue;


          if (cmd->cmd == HV_UPDATE_CALIB)
            {
              // LOG4CPLUS_INFO (hvlog, "Update Calibration for " << i << ":" << j);
              dataservices[i].calib[j].fLoaded = 0;
              fForceCalibrationUpdate = 1;
              LoadCalibration(i, j);
              continue;
            }


          switch ( cmd->chan )
            {
            case ALL:
              itr_chan.start = 0;
              itr_chan.end = dataservices[i].data->module[j].num_chans;
              break;
            case CHAMB_1:
              itr_chan.start = 0;
              itr_chan.end = ( dataservices[i].data->module[j].type == RDB36 ) ? 18 : dataservices[i].data->module[j].num_chans;
              break;
            case CHAMB_2:
              itr_chan.start = ( dataservices[i].data->module[j].type == RDB36 ) ? 18 : 0;
              itr_chan.end = dataservices[i].data->module[j].num_chans;
              break;
            default:
              itr_chan.start = cmd->chan;
              itr_chan.end = cmd->chan + 1;
            }
          itr_chan.len = itr_chan.end - itr_chan.start;

          realcmd.data = cmd->data;
          // realcmd.card = i;
          realcmd.module = j;
          realcmd.chan = 0;
          realcmd.size = 0;
          // int res = 0;
          switch ( cmd->cmd )
            {
            case HV_GET_MOD_DATA:
              // Set Policy file write flag  (bit0)
              cmd->cmd = HV_SET_MOD_POLICY;
              cmd->data &= POLICY_WRITE_FILE;
              res = ioctl( fd, HVCARD_PCIE_IOXCMD, cmd );

              break;
            case HV_GET_MOD_HIST_DATA:
              GetHVData(fd,i,j,0, HV_GET_MOD_HIST_DATA, (ULONG*)(&mod_hist),sizeof(struct HVhistmodule));
              /*
               for (int h=0; h<1500; h++) {
              std::cout << h << ": ";
               for (int k=0; k<dataservices[i].data->module[j].num_chans; k++) {
              if (mod_hist.chan[k].pos == h) std::cout << "*";
              std::cout << mod_hist.chan[k].vmon[h] << " ";
              //std::cout << mod_hist.chan[k].pos << " ";
               }
              std::cout << std:: endl;
              }
              */
              break;

            case HV_SET_MOD_STATE:
              res = ioctl( fd, HVCARD_PCIE_IOXCMD, & realcmd );
              break;
            case HV_SET_MOD_TYPE:
              dataservices[i].calib[j].fLoaded = false;
              res = ioctl( fd, HVCARD_PCIE_IOXCMD, & realcmd );
              break;
            }
          if ( LoadCalibration( i, j ) )
            {
              HVcalibdata data = dataservices[i].calib[j];
              HVmodule& setdata = dataservices[i].setdata->module[j];
              for ( int chan = 0; chan < itr_chan.len; chan++ )
                {
                  int k = chan + itr_chan.start;
                  float val;
                  int v;
                  switch ( cmd->cmd )
                    {
                    case HV_SET_RAMP_UP:
                      setdata.chan[k].ramp_up = cmd->data;
                      val = cmd->data * data.dac[k].A;
                      realcmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                      break;
                    case HV_SET_RAMP_DOWN:
                      setdata.chan[k].ramp_down = cmd->data;
                      val = cmd->data * data.dac[k].A;
                      realcmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                      break;
                      // case	HV_SET_VMAX:
                    case HV_SET_VSET:
                      setdata.chan[k].vset = cmd->data;
                      if ( ( dataservices[i].data->module[j].type == MASTER ) && ( cmd->data >= 4000 ) )
                        val = cmd->data;
                      else
                        val = cmd->data * data.dac[k].A + data.dac[k].B;
                      realcmd.data = ( val < 0 ) ? 0 : ( unsigned long ) val;
                      if (cmd->data==0) realcmd.data = 0;
                      if (data.adc[k].A>0)
                        {
                          float fval = ((float)(cmd->data)-data.adc[k].B)/data.adc[k].A;
                          int ival = (int) rint(fval);
                          realcmd.data += (ival < 0) ? 0: (unsigned long) (ival<<16);
                          // cout << k << ": " << cmd->data << ", " << data.adc[k].A << ", " << data.adc[k].B << ", " << fval << ", " << ival << endl;
                        }

                      break;
                    case HV_SET_OV_LIMIT:
                      setdata.chan[k].vov = cmd->data;
                      if (data.adc[k].A>0)
                        {
                          float fval = cmd->data/data.adc[k].A;
                          int ival= (int) rint(fval);
                          realcmd.data = (ival < 0) ? 0: ival;
                        }
                      break;

                    case HV_SET_IMAX:
                      setdata.chan[k].imax = cmd->data;
                      v = 0;
                      if ( data.type == MASTER )
                        {
                          cmd->data = ( ( cmd->data * 25 ) / 10 > data.cur[k] [0] ) ? data.cur[k] [0] : cmd->data;
                          while ( ( v <= 3999 ) && ( data.cur[k] [v + 1] > ( cmd->data * 25 ) / 10 )
                                  || ( data.cur[k] [v] == data.cur[k] [v + 1] ) )v++;
                        }
                      else
                        {
                          cmd->data = ( cmd->data * 10 > data.cur[k] [0] ) ? data.cur[k] [0] : cmd->data;
                          while ( ( v <= 3999 ) && ( data.cur[k] [v + 1] > ( cmd->data * ADC_CONV_COEF ) )
                                  || ( data.cur[k] [v] == data.cur[k] [v + 1] ) )v++;
                        }
                      //		cout <<	i << ":	" << data.cur[k][i]	<< endl;
                      realcmd.data = v;
                      break;

                    case HV_SET_IMAX_RAMP:
                      setdata.chan[k].imax = cmd->data;
                      v = 0;
                      if ( data.type == MASTER )
                        {
                          cmd->data = ( ( cmd->data * 25 ) / 10 > data.cur[k] [0] ) ? data.cur[k] [0] : cmd->data;
                          while ( ( v <= 3999 ) && ( data.cur[k] [v + 1] > ( cmd->data * 25 ) / 10 )
                                  || ( data.cur[k] [v] == data.cur[k] [v + 1] ) )v++;
                        }
                      else
                        {
                          cmd->data = ( cmd->data * 10 > data.cur[k] [0] ) ? data.cur[k] [0] : cmd->data;
                          while ( ( v <= 3999 ) && ( data.cur[k] [v + 1] > ( cmd->data * ADC_CONV_COEF ) )
                                  || ( data.cur[k] [v] == data.cur[k] [v + 1] ) )v++;
                        }
                      //                cout << i << ": " << data.cur[k][i]     << endl;
                      realcmd.data = v;
                      break;
                      /*
                           case HV_RESET_TRIP:
                                          memset(&trip_data, 0, sizeof(struct HVhistmodule));
                                          GetHVData(fd,i,j,0, HV_GET_MOD_HIST_DATA, (ULONG*)(&trip_data),sizeof(struct HVhistmodule));
                      	dataservices[i].databuf[j].trip_data[k].flag = true;
                                          dataservices[i].databuf[j].trip_data[k].tstamp = time(NULL);
                      	if (!SaveTripInfoToDB(i,j,k, trip_data.chan[k], HV_STATE_CHAN_TRIP))
                                                  LOG4CPLUS_WARN(hvlog, "Error saving Trip Info to database");
                            break;
                      */

                    }

                  realcmd.card = i;
                  realcmd.module = j;
                  realcmd.chan = k;
                  int res = ioctl( fd, HVCARD_PCIE_IOXCMD, & realcmd );
                }
            }
          // }
        }
    }
  return 0;
}

int getRealHVData(HVcmd* cmd, HVboard& board)
{
  if ( LoadCalibration( cmd->card, cmd->module ) )
    {
      pthread_mutex_lock(&IOmutex);
      HVhostcard * hvcard = dataservices[cmd->card].data;
      HVcalibdata data = dataservices[cmd->card].calib[cmd->module];
      board.card.hostid = hvcard->hostid;
      board.card.bus = hvcard->bus;
      board.card.slot = hvcard->slot;
      board.card.dataslot = hvcard->dataslot;
      memcpy( & ( board.module ), & ( hvcard->module[cmd->module] ), sizeof( board ) );
      pthread_mutex_unlock(&IOmutex);
      float hvmon = board.module.hvmon * data.vmon.A + data.vmon.B;
      board.module.hvmon = ( hvmon < 0 ) ? 0 : ( UINT )hvmon;
      for ( int i = 0; i < board.module.num_chans; i++ )
        {
          HVchannel ch = board.module.chan[i];
          if ( ch.status == HV_STATE_DEAD )
            {
              ch.vmon = 0;
              ch.imon = 0;

            }
          else
            {

              float val = ch.vmon * data.adc[i].A;
              val += (ch.vmon>0)?data.adc[i].B:0;
              // val = (val>4000)?4000:val;


              // float val = ch.vmon*data.adc[i].A;
              ch.vmon = ( val < 0 ) ? 0 : ( UINT )val;

              val = ch.vset_adc * data.adc[i].A;
              val += (ch.vset_adc>0)?data.adc[i].B:0;
              ch.vset_adc = ( val < 0 ) ? 0 : ( UINT )val;

              val = ch.vov * data.adc[i].A;
              ch.vov = ( val < 0 ) ? 0 : ( UINT )val;

              val = ( ch.vtrip > 0 ) ? ch.vtrip * data.adc[i].A + data.adc[i].B : ch.vtrip;
              // val = (ch.vtrip>0)?ch.vtrip*data.adc[i].A:ch.vtrip;
              ch.vtrip = ( val < 0 ) ? 0 : ( UINT )val;

              // val = (ch.vmax -	data.dac[i].B)/data.dac[i].A;
              // ch.vmax = (val <0)?0:(UINT)val;

              if (data.dac[i].A > 0)
                {
                  val = ch.ramp_up / data.dac[i].A;
                  ch.ramp_up = ( val < 0 ) ? 0 : ( UINT )val;

                  val = ch.ramp_down / data.dac[i].A;
                  ch.ramp_down = ( val < 0 ) ? 0 : ( UINT )val;

                  val = ( ch.vset - data.dac[i].B ) / data.dac[i].A;
                  ch.vset = ( val < 0 ) ? 0 : ((val>4000)?4000:( UINT )val);


                  val = ( ch.vcur - data.dac[i].B ) / data.dac[i].A;
                  ch.vcur = ( val < 0 ) ? 0 : ((val>4000)?4000:( UINT )val);
                }

              if ( data.type == MASTER )
                {
                  ch.imon = ( ( data.cur[i] [ch.imon] - 1 ) * 100 ) / 25;
                  ch.itrip = ( ch.itrip > 0 ) ? ( ( data.cur[i] [ch.itrip] - 1 ) * 10 ) / 25 : ch.itrip;
                  ch.imax = ( data.cur[i] [ch.imax] * 10 ) / 25;
                  ch.imax_ramp = ( data.cur[i] [ch.imax_ramp] * 10 ) / 25;
                }
              else
                {
                  ch.imon = data.cur[i] [ch.imon] - 1;
                  ch.itrip = ( ch.itrip > 0 ) ? ( data.cur[i] [ch.itrip] - 1 ) / ADC_CONV_COEF : ch.itrip;
                  ch.imax = data.cur[i] [ch.imax] / ADC_CONV_COEF;
                  ch.imax_ramp = data.cur[i] [ch.imax_ramp] / ADC_CONV_COEF;
                }
            }
          board.module.chan[i] = ch;
        }
      return 1;
    }
  else return 0;
}
