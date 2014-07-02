package hvclient;

import java.nio.*;
import java.util.Map;
import java.util.LinkedHashMap;

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
public class HVchannel {

  private Map<String, Object> m = new LinkedHashMap<String, Object> ();
  private void createParametersMap(Map m) {
    m.put("Vset", vset);
    m.put("Vset_adc", vset_adc);
    m.put("Vmon", vmon);
    m.put("Imon", imon);
    m.put("Vcur", vcur);
    m.put("RampUp", ramp_up);
    m.put("RampDown", ramp_down);
    m.put("Vmax", vmax);
    m.put("Imax", imax);
    m.put("RampImax", imax_ramp);
    m.put("Vov", vov);
    m.put("Itrip", itrip);
    m.put("Vtrip", vtrip);
    m.put("OVTripCnt", vtrip_cnt);
    m.put("OVTripDelay", vtrip_delay);
    m.put("TripCnt", trip_cnt);
    m.put("TripDelay", trip_delay);
    m.put("TripResetDelay", trip_reset_delay);
    m.put("TripResetDelayCnt", trip_reset_delay_cnt);
    m.put("TripMaxResets", trip_max_resets);
    m.put("TripResetsCnt", trip_resets_cnt);
    m.put("DeadMaxCnt", dead_max_cnt);
    m.put("DeadCnt", dead_cnt);
    m.put("RampCounter", ramp_counter);
    m.put("Relay", relay);
    m.put("Fuse", fuse);
    m.put("Flags", flags);
    m.put("State", state);
    m.put("Status", status);

    /*
         m.put("Vset", new Integer(0));
        m.put("Vmon", new Integer(0));
        m.put("Imon", new Integer(0));
        m.put("Vcur", new Integer(0));
        m.put("RampUp", new Integer(0));
        m.put("RampDown", new Integer(0));
        m.put("Vmax", new Integer(0));
        m.put("Imax", new Integer(0));
        m.put("Itrip", new Integer(0));
        m.put("Vtrip", new Integer(0));
        m.put("TripCnt", new Integer(0));
        m.put("TripDelay", new Integer(0));
        m.put("TripResetDelay", new Integer(0));
        m.put("TripResetDelayCnt", new Integer(0));
        m.put("TripMaxResets", new Integer(0));
        m.put("TripResetsCnt", new Integer(0));
        m.put("Relay", new Integer(0));
        m.put("Fuse", new Integer(0));
        m.put("State", new Integer(0));
        m.put("Status", new Integer(0));
     */
  }

 public int getIndex(String param) {
   int index = -1;
   int i=0;
   for (String key: m.keySet()) {
     i++;
     if (key.equals(param)) {
       return i;
     }
   }
   return index;
 }


  public HVchannel(ByteBuffer buf) {
    createParametersMap(m);
    decode(buf);
  }

  public HVchannel() {
    createParametersMap(m);
  }

  Long module = new Long(0);
  Integer vset = 0;
  Integer vset_adc = 0;
  Integer vov = 0;
  Integer vmon = 0;
  Integer vcur = 0;
  Integer vmax = 0;
  Integer ramp_up = 0;
  Integer ramp_down = 0;
  Integer imax = 0;
  Integer imax_ramp = 0;
  Integer imon = 0;
  Integer itrip = 0;
  Integer vtrip = 0;
  Integer trip_cnt = 0;
  Integer trip_delay = 0;
  Integer vtrip_cnt = 0;
  Integer vtrip_delay = 0;
  Integer trip_reset_delay = 0;
  Integer trip_reset_delay_cnt = 0;
  Integer trip_max_resets = 0;
  Integer trip_resets_cnt = 0;
  Integer dead_max_cnt = 0;
  Integer dead_cnt = 0;
  Integer ramp_counter = 0;
  Integer relay = 0;
  Integer fuse = 0;
  Integer flags = 0;
  Integer state = 0;
  Integer status = 0;

  static int getSize() {
    return 8+29 * 4;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    String res = "";
    for (String key : m.keySet()) {
      res += key + ":" + m.get(key) + SEP;
    }

    /*
          return "vset:"+ vset + SEP +
              "vmon:"+ vmon + SEP +
              "vcur:"+ vcur + SEP +
              "vmax:"+ vmax + SEP +
              "ramp_up:"+ ramp_up + SEP +
              "ramp_down:"+ ramp_down + SEP +
              "imax:"+ imax + SEP +
              "imon:"+ imon + SEP +
              "itrip:"+ itrip + SEP +
              "vtrip:"+ vtrip + SEP +
              "trip_cnt:"+ trip_cnt + SEP +
              "trip_delay:"+ trip_delay + SEP +
              "trip_reset_delay:"+ trip_reset_delay + SEP +
              "trip_reset_delay_cnt:"+ trip_reset_delay_cnt + SEP +
              "trip_max_resets:"+ trip_max_resets + SEP +
              "trip_resets_cnt:"+ trip_resets_cnt + SEP +
              "relay:"+ relay + SEP +
              "fuse:"+ fuse + SEP +
              "state:"+ state + SEP +
              "status:" + status;
     */
    return res;
  }

  public Object get(String key) {
    createParametersMap(m);
    if (m.containsKey(key)) {
      return m.get(key);
    }
    return null;
  }

  public Object[] getHeaders() {
    return m.keySet().toArray();
  }

  public Object[] getValues() {
    createParametersMap(m);
    return m.values().toArray();
  }

  public Map getData() {
    createParametersMap(m);
    return m;
  }

  public Object set(String key, Object value) {
    if (m.containsKey(key)) {
      m.put(key, value);
      return m.get(key);
    }
    return null;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    createParametersMap(m);
    buf.putLong(module);

    for (String key : m.keySet()) {
      Utils.writeMapValue(key, m, buf);
    }
    /*
         buf.putInt(module);
         buf.putInt(vset);
         buf.putInt(vmon);
         buf.putInt(imon);
         buf.putInt(vcur);
         buf.putInt(ramp_up);
         buf.putInt(ramp_down);
         buf.putInt(vmax);
         buf.putInt(imax);
         buf.putInt(itrip);
         buf.putInt(vtrip);
         buf.putInt(trip_cnt);
         buf.putInt(trip_delay);
         buf.putInt(trip_reset_delay);
         buf.putInt(trip_reset_delay_cnt);
         buf.putInt(trip_max_resets);
         buf.putInt(trip_resets_cnt);
         buf.putInt(relay);
         buf.putInt(fuse);
         buf.putInt(state);
         buf.putInt(status);
     */
    return buf;
  }

  int decode(byte[] databuf) {
    return decode(databuf, 0);
  }

  int decode(byte[] databuf, int pos) {
    ByteBuffer buf = ByteBuffer.wrap(databuf, pos, getSize());
    return pos + decode(buf);
  }

  int decode(ByteBuffer buf) {
    buf.order(ByteOrder.LITTLE_ENDIAN);
    /*for (String key : m.keySet()) {
      Utils.readMapValue(key, m, buf);
         }
     */
    module = buf.getLong();
    vset = buf.getInt();
    vset_adc = buf.getInt();
    vmon = buf.getInt();
    imon = buf.getInt();
    vcur = buf.getInt();
    ramp_up = buf.getInt();
    ramp_down = buf.getInt();
    vmax = buf.getInt();
    imax = buf.getInt();
    imax_ramp = buf.getInt();
    vov = buf.getInt();
    itrip = buf.getInt();
    vtrip = buf.getInt();
    vtrip_cnt = buf.getInt();
    vtrip_delay = buf.getInt();
    trip_cnt = buf.getInt();
    trip_delay = buf.getInt();
    trip_reset_delay = buf.getInt();
    trip_reset_delay_cnt = buf.getInt();
    trip_max_resets = buf.getInt();
    trip_resets_cnt = buf.getInt();
    dead_max_cnt = buf.getInt();
    dead_cnt = buf.getInt();
    ramp_counter = buf.getInt();
    relay = buf.getInt();
    fuse = buf.getInt();
    flags = buf.getInt();
    state = buf.getInt();
    status = buf.getInt();
    createParametersMap(m);
    return getSize();
  }
}
