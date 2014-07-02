package hvclient;

import java.nio.ByteOrder;
import java.nio.ByteBuffer;
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
public class HVcscdata {
  public HVcscdata() {
    csc = new HVcsc();
    primary = new HVprimary();
    hvcard = new HVcard();
    master = new HVchannel();
    rdb = new HVmodule();
    rdb_part = 1;
  }

  public HVcscdata(ByteBuffer buf) {
    csc = new HVcsc(buf);
    primary = new HVprimary(buf);
    hvcard = new HVcard(buf);
    master = new HVchannel(buf);
    rdb = new HVmodule(buf);
    rdb_part = buf.getInt();
  }


  HVcsc           csc;            // CSC information
  HVprimary       primary;        // Primary PS data
  HVcard          hvcard;         // Host Card information
  HVchannel       master;         // Linked Master Board channel data
  HVmodule        rdb;            // RDB module data
  Integer          rdb_part;       // RDB part which control CSC (1/2 in case of RDB 36 modules)

  int getSize() {
    return csc.getSize() + primary.getSize() + hvcard.getSize() + master.getSize() + rdb.getSize() + 4;
  }

  protected String SEP = " ";
  public void setPrintSeparator(String sep) {
    SEP = sep;
  }

  public String toString() {
    csc.setPrintSeparator(SEP);
    primary.setPrintSeparator(SEP);
    hvcard.setPrintSeparator(SEP);
    master.setPrintSeparator(SEP);
    rdb.setPrintSeparator(SEP);
    return "CSC->" + SEP + csc.toString() + SEP +
        "Primary->" + SEP + primary.toString() + SEP +
        "Hostcard->" + SEP + hvcard.toString() + SEP +
        "Master->" + SEP + master.toString() + SEP +
        "RDB->" + SEP + rdb.toString() + SEP +
        "rdb_part:" + rdb_part;
  }

  public Object[][] getCSCChanValues() {

    int num_chans = (rdb.type == HVData.HV_TYPE.RDB30.ordinal())?30:18;

     Object[][] valarray = new Object[num_chans + 2][rdb.chan[0].getHeaders().length +
         1];
     Vector<Object>
         allvalue = new Vector<Object> (rdb.chan[0].getHeaders().length + 1);
     Vector<Object>
         mastervalue = new Vector<Object> (master.getHeaders().length + 1);

     HVchannel allchan = new HVchannel();

     int start = (rdb_part==2 && num_chans == 18)?18:0;
     for (int i = start; i < num_chans+start; i++) {

       HVchannel ch = rdb.chan[i];
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
           value = new Vector<Object> (rdb.chan[i].getHeaders().length + 1);
       value.add(0, i + 1-start);
       value.addAll(1, rdb.chan[i].getData().values());
       valarray[i + 1 - start] = value.toArray();
       valarray[i + 1 - start][ch.getIndex("Imon")] = new Float(new Integer(ch.imon).floatValue()/11);
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
     allchan.state = rdb.state;
     allchan.status = rdb.status;

     allvalue.add(0, "ALL");
     allvalue.addAll(1, allchan.getData().values());
     valarray[0] = allvalue.toArray();
     valarray[0][allchan.getIndex("Imon")] = new Float(new Integer(allchan.imon).floatValue()/11);

     mastervalue.add(0, "MASTER");
     mastervalue.addAll(1, master.getData().values());
     valarray[num_chans+1] = mastervalue.toArray();
     valarray[num_chans+1][master.getIndex("Imon")] = new Float(new Integer(master.imon).floatValue()/11);

     return valarray;
  }

  ByteBuffer encode() {
    ByteBuffer buf = ByteBuffer.allocate(getSize());
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.put(csc.encode());
    buf.put(primary.encode());
    buf.put(hvcard.encode());
    buf.put(master.encode());
    buf.put(rdb.encode());
    buf.putInt(rdb_part);
    buf.rewind();
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
    csc.decode(buf);
    primary.decode(buf);
    hvcard.decode(buf);
    master.decode(buf);
    rdb.decode(buf);
    rdb_part = buf.getInt();
    return getSize();
  }


}
