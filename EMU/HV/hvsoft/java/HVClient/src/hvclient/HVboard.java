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
public class HVboard {

  public HVboard(ByteBuffer buf) {
    card = new HVcard(buf);
    module = new HVmodule(buf);
  }

  public HVboard() {
      card = new HVcard();
      module = new HVmodule();
  }

  HVcard card;
  HVmodule module;

  int getSize() {
    return card.getSize() + module.getSize();
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    card.setPrintSeparator(SEP);
    module.setPrintSeparator(SEP);
    return card.toString() + SEP + module.toString();
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.put(card.encode());
    buf.put(module.encode());
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
    card.decode(buf);
    module.decode(buf);
    return getSize();
  }

}
