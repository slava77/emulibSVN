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
public class HVcsc {
  public HVcsc(ByteBuffer buf) {
    decode(buf);
  }

  public HVcsc() {
  }

  protected static final int NAME_LEN = 12;
  String name = ""; // char[NAME_LEN]
  Integer endcap = 0;
  Integer station = 0;
  Integer ring = 0;
  Integer position = 0;

  int getSize() {
    return NAME_LEN + 4 * 4;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    return "name:" + name + SEP +
        "endcap:" + endcap + SEP +
        "station:" + station + SEP +
        "ring:" + ring + SEP +
        "position:" + position;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    byte[] bb = new byte[NAME_LEN];
    System.arraycopy(name.getBytes(), 0, bb, 0, name.length());
    if (name.length() < NAME_LEN) {
      bb[name.length()] = 0;
    }
    buf.put(bb);

    buf.putInt(endcap);
    buf.putInt(station);
    buf.putInt(ring);
    buf.putInt(position);

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
    byte[] bb = buf.get(buf.array(), buf.position(), NAME_LEN).array();
    name = new String(bb, 0, NAME_LEN).replaceAll("[^meME0-9/+-]", "");
    endcap = buf.getInt();
    station = buf.getInt();
    ring = buf.getInt();
    position = buf.getInt();

    return getSize();
  }

}
