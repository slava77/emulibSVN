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
public class HVcsccmd {
  protected static final int NAME_LEN = 12;
  String csc = ""; // char[NAME_LEN]
  Integer chan = 0;
  Integer cmd = 0; // HV_CMD
  Long data = new Long(0);
  Long size = new Long(0);

  int getSize() {return NAME_LEN+2*4+2*8;}

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }


  public String toString() {
    return "csc:"+ csc + SEP +
        "chan:"+ chan + SEP +
        "cmd:"+ HVData.HV_CMD.getValue(cmd) + SEP +
        "data:"+ data + SEP +
        "size:"+ size;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    byte[] bb = new byte[NAME_LEN];
    System.arraycopy(csc.getBytes(),0,bb,0,csc.length());
    if (csc.length() < NAME_LEN) bb[csc.length()]=0;
    buf.put(bb);
    buf.putInt(chan);
    buf.putInt(cmd);
    buf.putLong(data);
    buf.putLong(size);
    buf.rewind();
    return buf;
  }

  int decode(byte[] databuf) {
    return decode(databuf, 0);
  }

  int decode(byte[] databuf, int pos) {
    ByteBuffer buf = ByteBuffer.wrap(databuf, pos, getSize());
    return pos+decode(buf);
  }

  int decode(ByteBuffer buf) {
    buf.order(ByteOrder.LITTLE_ENDIAN);
    // buf.rewind();
    byte[] bb = buf.get(buf.array(),buf.position(),NAME_LEN).array();
    csc = new String(bb,0,NAME_LEN).replaceAll("[^meME0-9/]","");
    chan = buf.getInt();
    cmd = buf.getInt();
    data = buf.getLong();
    size = buf.getLong();
    return getSize();
  }

}
