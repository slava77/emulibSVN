package hvclient;

/**
 * <p>Title: HVClient</p>
 *
 * <p>Description: HVClient Java GUI Application</p>
 *
 * <p>Copyright: Copyright (c) 2005</p>
 *
 * <p>Company: University of Florida</p>
 *
 * @author Victor Barashko <barvic@ufl.edu>
 * @version 1.0
 */
public class HVData {

  public static final int MODULE_MAX_CHANS = 48;
  public static final int HOST_MAX_MODULES = 16;
  public static int RDBALL_TYPE = 249;
  public static int RDB30_TYPE = 250;
  public static int RDB36_TYPE = 251;
  public static int MASTER_TYPE = 252;
  public static int CHAMB_1 = 253; // First 18 channels for RDB36
  public static int CHAMB_2 = 254; // Last 18 channels for RDB36
  public static int ALL = 255;
  public static int NA = -1;

  /**
   * HV commands enum
   */
  public enum HV_CMD {
   HV_SET_OFF, // + Turn Off module or channel
   HV_SET_ON, // + Turn On module or channel
   HV_SET_LOCK, // - Lock module or channel (obsolete)
   HV_SET_RAMP_UP, // + Set Ramp Up rate
   HV_SET_RAMP_DOWN, // + Set Ramp Down rate
   HV_SET_VMAX, // + Set maximum voltage output value
   HV_SET_IMAX, // + Set maximum current limit (trip level)
   HV_SET_VSET, // + Set output voltage
   HV_SET_STATE, // + Set channel state
   HV_GET_VMON, // - Read channel monitored voltage value
   HV_GET_IMON, // - Read channel monitored current value
   HV_GET_RAMP_UP, // - Read channel Ramp Up rate
   HV_GET_RAMP_DOWN, // - Read channel Ramp Down rate
   HV_GET_VMAX, // - Read channel maximum voltage output value
   HV_GET_IMAX, // - Read channel maximum current limit (trip level)
   HV_GET_VCUR, // - Read internal vcur voltage value
   HV_GET_VSET, // - Read set output voltage value
   HV_GET_STATE, // - Read channel state
   HV_GET_STATUS, // - Read channel status
   HV_GET_ITRIP, // - Read trip current value
   HV_GET_VTRIP, // - Read trip voltage value
   HV_GET_MOD_ADDR, // - Read module's bus address
   HV_GET_MOD_ID, // - Read module's ID
   HV_GET_MOD_ILOCK, // - Read module's interlock state
   HV_GET_MOD_POS5V, // - Read module's +5V status
   HV_GET_MOD_NEG5V, // - Read module's -5V status
   HV_GET_MOD_TYPE, // - Read module's type
   HV_SET_MOD_TYPE, // + Set module's type
   HV_GET_MOD_NUM_CHANS, // - Read module's number of HV channels
   HV_GET_MOD_RAWDATA, // -
   HV_SET_MOD_STATE, // + Set module state
   HV_GET_CARD_DATA, // + Read hostcard's data block from driver
   HV_GET_MOD_DATA, // + Read module's  data from driver (controls data file writing in Dim Server)
   HV_GET_CHAN_DATA, // - Read channel's data from driver
   HV_SET_RELAY, // + Set relay state
   HV_GET_RELAY, // - Read relay state
   HV_GET_FUSE_STATUS, // - Read fuse status
   HV_RESET_TRIP, // + Resest trip state
   HV_SET_TRIP_DELAY, // + Set trip delay in ms
   HV_GET_TRIP_DELAY, // - Read set trip delay in ms
   HV_GET_TRIP_CNT, // - Read trip delay counter in ms
   HV_GET_MOD_HVMON, // - Read voltage from monitoring channel of RDB
   HV_SET_MASTER, // + Set linked Master information
   HV_GET_MASTER, // - Read linked Master information
   HV_SET_MASTER_TRIP_DELAY, // + Set linked master trip delay in ms
   HV_GET_MASTER_TRIP_DELAY, // - Read linked master trip delay in ms
   HV_SET_TRIP_RESET_DELAY, // + Set trip auto reset delay in ms
   HV_GET_TRIP_RESET_DELAY, // - Read set trip auto reset delay in ms
   HV_GET_TRIP_RESET_DELAY_CNT, // - Read trip auto reset delay counter in ms
   HV_SET_TRIP_MAX_RESETS, // + Set max number of auto trip resets
   HV_GET_TRIP_MAX_RESETS, // - Read set max number of auto trip resets
   HV_GET_TRIP_RESETS_CNT, // - Read auto trip resets counter
   HV_SET_ILOCK_DELAY, // + Set interlock and interface noise errors ignore delay in ms
   HV_GET_ILOCK_DELAY, // - Read set interlock and interface noise errors ignore delay in ms
   HV_GET_ILOCK_DELAY_CNT, // - Read interlock and interface noise errors ignore counter in ms
   HV_DETECT, // Force Modules detection
   HV_SET_MOD_POLICY,
   HV_GET_MOD_POLICY,
   HV_UPDATE_CALIB, // Update calibration data in hvServer
   HV_SET_OV_LIMIT,
   HV_SET_OV_TRIP_DELAY,
   HV_DO_CALIB,
   HV_SET_RAMPDOWN_SET_DELAY, // Set ramp down delay in seconds
   HV_SET_IMAX_RAMP,          // Set trip level for ramp state
   HV_GET_IMAX_RAMP,          // Get trip level for ramp state
   HV_SET_MOD_PRIMARY_STATUS,
   HV_SET_DEAD_MAX_CNT,
   HV_GET_MOD_HIST_DATA,
   LAST_CMD;

   public static HV_CMD getValue(int val) {
    HV_CMD res= LAST_CMD;
    for (HV_CMD t : HV_CMD.values())
      if (t.ordinal() == val) {res = t; break;}
    return res;
   }

 };


 /**
  * HV module types enum
  */
 public enum HV_TYPE {
   NONE   (0),
   MASTER (8),  // Master card
   RDB30  (30), // Remote distribution board 30-channels
   RDB36  (36), // Remote distribution board 36-channels
   MON30  (30), // Monitoring board for 30-channels
   MON36  (36); // Monitoring board for 36-channels
   private final int num_chans;
   HV_TYPE(int num_channels) {
     this.num_chans = num_channels;
   }
   public static HV_TYPE getValue(int val) {
    HV_TYPE res= NONE;
    for (HV_TYPE t : HV_TYPE.values())
      if (t.ordinal() == val) {res = t; break;}
    return res;
   }
   public int num_chans() {return num_chans;}
 };

 /**
  * HV states and statuses enum
  */
 public enum HV_STATE {
   HV_STATE_OFF            ("OFF"),        // Channel or module is turned Off
   HV_STATE_ON             ("ON"),         // Channel or module is turned On
   HV_STATE_RAMP_UP        ("RAMP UP"),    // Channel is ramping Up
   HV_STATE_RAMP_DOWN      ("RAMP DOWN"),  // Channel is ramping Down
   HV_STATE_DEAD           ("DEAD"),       // Channel is Dead
   HV_STATE_ILOCK          ("INTERLOCK"),  // Module's Interlock
   HV_STATE_INT_TRIP       ("INT TRIP"),   // Module detects Trip condition in one of channels
   HV_STATE_CHAN_TRIP      ("CUR TRIP"),// Current Trip condition in channel
   HV_STATE_OV_TRIP        ("OV TRIP"), // Over Voltage Trip condition in channel
   HV_STATE_SET            ("SET");        // Unused
   private final String name;
   HV_STATE(String name) {
     this.name = name;
   }
   public static HV_STATE getValue(int val) {
    HV_STATE res= HV_STATE_OFF;
    for (HV_STATE t : HV_STATE.values())
      if (t.ordinal() == val) {res = t; break;}
    return res;
   }
   public String desc() {return name;}
 }

 final public String[] hvstate_str = {
      "OFF", "ON", "RAMP UP", "RAMP DOWN",
      "DEAD", "ILOCK", "INT TRIP", "CH INT TRIP"
  };


}
