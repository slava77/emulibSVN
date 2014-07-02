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
public class HVhostcard {

  public HVhostcard() {
    module = new HVmodule[HVData.HOST_MAX_MODULES];
    for (int ch = 0; ch < module.length; ch++) {
      module[ch] = new HVmodule();
    }
  }

  public HVhostcard(ByteBuffer buf) {
    module = new HVmodule[HVData.HOST_MAX_MODULES];
    for (int ch = 0; ch < module.length; ch++) {
      module[ch] = new HVmodule();
    }
    decode(buf);
  }

  Integer pdev = 0;
  Integer dma = 0;
  Integer revision = 0;
  Integer ioaddr = 0;
  Integer hostid = 0;
  Integer bus = 0;
  Integer slot = 0;
  Integer dataslot = 0;
  HVmodule[] module;

  static int getSize() {
    return HVData.HOST_MAX_MODULES * HVmodule.getSize() + 8 * 4;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    String res = "pdev:" + pdev + SEP +
        "dma:" + dma + SEP +
        "revision:" + revision + SEP +
        "ioaddr:" + ioaddr + SEP +
        "hostid:" + hostid + SEP +
        "bus:" + bus + SEP +
        "slot:" + slot + SEP +
        "dataslot:" + dataslot + SEP;
    for (int ch = 0; ch < HVData.HOST_MAX_MODULES; ch++) {
      module[ch].setPrintSeparator(SEP);
      res += "module:" + (ch + 1) + SEP + module[ch].toString() + SEP;
    }

    return res;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.putInt(pdev);
    buf.putInt(dma);
    buf.putInt(revision);
    buf.putInt(ioaddr);
    buf.putInt(hostid);
    buf.putInt(bus);
    buf.putInt(slot);
    buf.putInt(dataslot);
    for (int ch = 0; ch < HVData.HOST_MAX_MODULES; ch++) {
      buf.put(module[ch].encode());
    }
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
    pdev = buf.getInt();
    dma = buf.getInt();
    revision = buf.getInt();
    ioaddr = buf.getInt();
    hostid = buf.getInt();
    bus = buf.getInt();
    slot = buf.getInt();
    dataslot = buf.getInt();
    for (int ch = 0; ch < HVData.HOST_MAX_MODULES; ch++) {
      module[ch].decode(buf);
    }
    return getSize();
  }

}
