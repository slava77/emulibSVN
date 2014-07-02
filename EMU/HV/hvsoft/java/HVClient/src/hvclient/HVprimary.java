package hvclient;

import java.nio.*;
import java.util.*;

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
public class HVprimary {

  private Map<String, Object> m = new LinkedHashMap<String, Object> ();
  private void createParametersMap(Map m) {
    m.put("HostID", new Integer(0));
    m.put("Port", new Integer(0));
    m.put("Addr", new Integer(0));
    m.put("Polarity", new Integer(0));
    m.put("Outset", new Integer(0));
    m.put("Output", new Integer(0));
    m.put("Control", new Integer(0));
    m.put("Imax", new Float(0));
    m.put("Vset", new Integer(0));
    m.put("Imon", new Float(0));
    m.put("Vmon", new Float(0));
    m.put("Vcur", new Integer(0));
    m.put("RampUp", new Integer(0));
    m.put("RampDown", new Integer(0));
    m.put("State", new Integer(0));
    m.put("Status", new Integer(0));
  }

  public HVprimary() {
    createParametersMap(m);
  }

  public HVprimary(ByteBuffer buf) {
    createParametersMap(m);
    decode(buf);
  }

  Integer hostid = 0; // Computer Host ID
  Integer port = 0; // COM port number (COM1 - 0)
  Integer addr = 0; // GPIB address
  Integer polarity = 0; // Output polarity
  Integer outset = 0; // Set value for Output mode
  Integer output = 0; // Read value of actual Output mode
  Integer control = 0; // Control status
  Float imax = new Float(0); // Maximum current (trip level)
  Integer vset = 0; // Set value for output voltage
  Float imon = new Float(0); // Monitored value of current in mkA
  Float vmon = new Float(0); // Monitored value of voltage in V
  Integer vcur = 0; // Internal value of voltage for ramp ups
  Integer rampup = 0; // Ramp up rate V/s
  Integer rampdown = 0; // Ramp down rate V/s
  Integer state = 0; // state
  Integer status = 0; // status

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    String res = "";
    for (String key : m.keySet()) {
      res += key + ":" + m.get(key) + SEP;
    }
    /*return "hostid:" + hostid + SEP +
        "port:" + port + SEP +
        "addr:" + addr + SEP +
        "polarity:" + polarity + SEP +
        "outset:" + outset + SEP +
        "output:" + output + SEP +
        "control:" + control + SEP +
        "imax:" + imax + SEP +
        "vset:" + vset + SEP +
        "imon:" + imon + SEP +
        "vmon:" + vmon + SEP +
        "vcur:" + vcur + SEP +
        "rampup:" + rampup + SEP +
        "rampdown:" + rampdown + SEP +
        "state:" + state + SEP +
        "status:" + status;
     */
    return res;
  }

  static int getSize() {
    return 16 * 4;
  }

  public Object get(String key) {
    if (m.containsKey(key)) {
      return m.get(key);
    }
    return null;
  }

  public Object[] getHeaders() {
    return m.keySet().toArray();
  }

  public Object[] getValues() {
    return m.values().toArray();
  }

  public Map getData() {
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
    for (String key : m.keySet()) {
      Utils.writeMapValue(key, m, buf);
    }
    /*
        buf.putInt(hostid);
        buf.putInt(port);
        buf.putInt(addr);
        buf.putInt(polarity);
        buf.putInt(outset);
        buf.putInt(output);
        buf.putInt(control);
        buf.putFloat(imax);
        buf.putInt(vset);
        buf.putFloat(imon);
        buf.putFloat(vmon);
        buf.putInt(vcur);
        buf.putInt(rampup);
        buf.putInt(rampdown);
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
    for (String key : m.keySet()) {
      Utils.readMapValue(key, m, buf);
    }

    /*
         hostid = buf.getInt();
         port = buf.getInt();
         addr = buf.getInt();
         polarity = buf.getInt();
         outset = buf.getInt();
         output = buf.getInt();
         control = buf.getInt();
         imax = buf.getFloat();
         vset = buf.getInt();
         imon = buf.getFloat();
         vmon = buf.getFloat();
         vcur = buf.getInt();
         rampup = buf.getInt();
         rampdown = buf.getInt();
         state = buf.getInt();
         status = buf.getInt();
     */
    return getSize();
  }

}
