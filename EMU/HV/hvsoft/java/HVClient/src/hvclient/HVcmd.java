package hvclient;

import java.lang.*;
import hvclient.*;
import java.nio.*;


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
public class HVcmd {
  Integer card = 0;
  Integer module = 0;
  Integer chan = 0;
  Integer cmd = 0;
  Long data = new Long(0);
  Long size = new Long(0);

  int getSize() {return 4*4+8*2;}

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }


  public String toString() {
    return "card:"+ card + SEP +
        "module:"+ module + SEP +
        "chan:"+ chan + SEP +
        "cmd:"+ HVData.HV_CMD.getValue(cmd) + SEP +
        "data:"+ data + SEP +
        "size:"+ size;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.putInt(card);
    buf.putInt(module);
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
    card = buf.getInt();
    module = buf.getInt();
    chan = buf.getInt();
    cmd = buf.getInt();
    data = buf.getLong();
    size = buf.getLong();
    return getSize();
  }
}
