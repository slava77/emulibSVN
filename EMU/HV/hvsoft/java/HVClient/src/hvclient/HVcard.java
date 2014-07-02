package hvclient;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;

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
public class HVcard {

  public HVcard(ByteBuffer buf) {
    decode(buf);
  }

  public HVcard() {

  }

  Integer hostid = 0;
  Integer bus = 0;
  Integer slot = 0;
  Integer dataslot = 0;

  int getSize() {
    return 4 * 4;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    return "hostid:" + hostid + SEP +
        "bus:" + bus + SEP +
        "slot:" + slot + SEP +
        "dataslot:" + dataslot;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.putInt(hostid);
    buf.putInt(bus);
    buf.putInt(slot);
    buf.putInt(dataslot);
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
    // buf.rewind();
    hostid = buf.getInt();
    bus = buf.getInt();
    slot = buf.getInt();
    dataslot = buf.getInt();
    return getSize();
  }

}
