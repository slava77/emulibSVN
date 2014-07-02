package hvclient;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.Vector;

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
public class HVmodule {

  public HVmodule(ByteBuffer buf) {
    chan = new HVchannel[HVData.MODULE_MAX_CHANS];
    for (int ch = 0; ch < chan.length; ch++) {
      chan[ch] = new HVchannel();
    }
    decode(buf);
  }

  public HVmodule() {
    chan = new HVchannel[HVData.MODULE_MAX_CHANS];
    for (int ch = 0; ch < chan.length; ch++) {
      chan[ch] = new HVchannel();
    }
  }

  Long hostcard = new Long(0);
  Integer type = 0;
  Long ioaddr = new Long(0);
  Integer ID = 0;
  Integer busaddr = 0; //UCHAR
  Integer interlock = 0; //UCHAR
  Integer pos5v = 0;
  Integer neg5v = 0;
  Integer hvmon = 0;
  Long map = new Long(0);
  Integer num_chans = 0;
  Integer master_id = 0;
  Integer master_hostid = 0;
  Integer master_hvcard = 0;
  Integer master_busaddr = 0;
  Integer master_chan = 0;
  Integer master_trip_delay = 0;
  Integer ilock_delay = 0;
  Integer ilock_delay_cnt = 0;
  Integer state = 0;
  Integer status = 0;
  Integer pr_status = 0;
  Integer policy = 0;
  HVchannel[] chan; // = new HVchannel[HVData.MODULE_MAX_CHANS];

  static int getSize() {
    return HVData.MODULE_MAX_CHANS * HVchannel.getSize() + 20 * 4 +3 * 8;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    String res = "type:" + type + SEP +
        "ioaddr:" + ioaddr + SEP +
        "ID:" + ID + SEP +
        "busaddr:" + busaddr + SEP +
        "interlock:" + interlock + SEP +
        "pos5v:" + pos5v + SEP +
        "neg5v:" + neg5v + SEP +
        "hvmon:" + hvmon + SEP +
        "num_chans:" + num_chans + SEP +
        "master_id:" + master_id + SEP +
        "master_hostid:" + master_hostid + SEP +
        "master_hvcard:" + master_hvcard + SEP +
        "master_busaddr:" + master_busaddr + SEP +
        "master_chan:" + master_id + SEP +
        "master_trip_delay:" + master_id + SEP +
        "ilock_delay:" + ilock_delay + SEP +
        "ilock_delay_cnt:" + ilock_delay_cnt + SEP +
        "state:" + state + SEP +
        "status:" + status + SEP +
        "pr_status:" + pr_status + SEP +
        "policy:" + policy +SEP;

    for (int ch = 0; ch < HVData.MODULE_MAX_CHANS; ch++) {
//      chan[ch].setPrintSeparator(SEP);
      res += "chan:" + (ch + 1) + SEP + chan[ch].toString() + SEP;
    }

    return res;
  }

  public Object[] getChanHeaders() {
    Vector<Object>
        headers = new Vector<Object> (chan[0].getHeaders().length + 1);
    headers.add(0, "Ch#");
    headers.addAll(1, chan[0].getData().keySet());
    return headers.toArray();
  }

  public Object[][] getRealChanValues() {
    Object[][] valarray = new Object[num_chans + 1][chan[0].getHeaders().length +
        1];
    Vector<Object>
        allvalue = new Vector<Object> (chan[0].getHeaders().length + 1);

    HVchannel allchan = new HVchannel();

    for (int i = 0; i < num_chans; i++) {

      HVchannel ch = chan[i];
      allchan.vmon += ch.vmon;
      allchan.imon += ch.imon;
      allchan.vmax += ch.vmax;
      allchan.imax += ch.imax;
      allchan.imax_ramp += ch.imax_ramp;
      allchan.vov += ch.vov;
      allchan.vset += ch.vset;
      allchan.vset_adc += ch.vset_adc;
      allchan.vcur += ch.vcur;
      allchan.ramp_up += ch.ramp_up;
      allchan.ramp_down += ch.ramp_down;
      allchan.vtrip_delay += ch.vtrip_delay;
      allchan.trip_delay += ch.trip_delay;
      allchan.trip_reset_delay += ch.trip_reset_delay;
      allchan.trip_max_resets += ch.trip_max_resets;
      allchan.dead_max_cnt += ch.dead_max_cnt;

      Vector<Object>
          value = new Vector<Object> (chan[i].getHeaders().length + 1);
      value.add(0, i + 1);
      value.addAll(1, chan[i].getData().values());
      valarray[i + 1] = value.toArray();
      valarray[i + 1][ch.getIndex("Imon")] = new Float(new Integer(ch.imon).floatValue()/11);
      value.clear();
    }

    allchan.vmon /= num_chans;
    allchan.imon /= num_chans;
    allchan.vmax /= num_chans;
    allchan.imax /= num_chans;
    allchan.imax_ramp /= num_chans;
    allchan.vov /= num_chans;
    allchan.vset /= num_chans;
    allchan.vset_adc /= num_chans;
    allchan.vcur /= num_chans;
    allchan.ramp_up /= num_chans;
    allchan.ramp_down /= num_chans;
    allchan.trip_delay /= num_chans;
    allchan.vtrip_delay /= num_chans;
    allchan.trip_reset_delay /= num_chans;
    allchan.trip_max_resets /= num_chans;
    allchan.dead_max_cnt /= num_chans;
    allchan.ramp_counter = 0;
    allchan.state = state;
    allchan.status = status;

    allvalue.add(0, "ALL");
    allvalue.addAll(1, allchan.getData().values());

    valarray[0] = allvalue.toArray();
    valarray[0][allchan.getIndex("Imon")] = new Float(new Integer(allchan.imon).floatValue()/11);
    return valarray;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.putLong(hostcard);
    buf.putInt(type);
    buf.putLong(ioaddr);
    buf.putInt(ID);
    buf.putInt(busaddr);
    buf.putInt(interlock);
    buf.putInt(pos5v);
    buf.putInt(neg5v);
    buf.putInt(hvmon);
    buf.putLong(map);
    buf.putInt(num_chans);
    buf.putInt(master_id);
    buf.putInt(master_hostid);
    buf.putInt(master_hvcard);
    buf.putInt(master_busaddr);
    buf.putInt(master_chan);
    buf.putInt(master_trip_delay);
    buf.putInt(ilock_delay);
    buf.putInt(ilock_delay_cnt);
    buf.putInt(state);
    buf.putInt(status);
    buf.putInt(pr_status);
    buf.putInt(policy);
    for (int ch = 0; ch < HVData.MODULE_MAX_CHANS; ch++) {
      buf.put(chan[ch].encode());
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
    // buf.rewind();
    hostcard = buf.getLong();
    type = buf.getInt();
    ioaddr = buf.getLong();
    ID = buf.getInt();
    busaddr = buf.getInt();
    interlock = buf.getInt();
    pos5v = buf.getInt();
    neg5v = buf.getInt();
    hvmon = buf.getInt();
    map = buf.getLong();
    num_chans = buf.getInt();
    master_id = buf.getInt();
    master_hostid = buf.getInt();
    master_hvcard = buf.getInt();
    master_busaddr = buf.getInt();
    master_chan = buf.getInt();
    master_trip_delay = buf.getInt();
    ilock_delay = buf.getInt();
    ilock_delay_cnt = buf.getInt();
    state = buf.getInt();
    status = buf.getInt();
    pr_status = buf.getInt();
    policy = buf.getInt();
    for (int ch = 0; ch < HVData.MODULE_MAX_CHANS; ch++) {
      chan[ch].decode(buf);
    }
    return getSize();
  }

}
