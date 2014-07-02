package hvclient;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import javax.swing.table.*;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.Timer;
import java.awt.Dimension;
import javax.swing.border.TitledBorder;
import dim.*;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import javax.swing.table.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import javax.swing.event.TableModelEvent;

/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2004</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class HVCSCFrame
    extends JInternalFrame {
//  BorderLayout borderLayout1 = new BorderLayout();
  protected String[] set_params_list = {
      "State",
      "Vset (V)", "Ramp Up (V)", "Ramp Down (V)", "Imax (mkA)", "RampImax (mkA)",
      "OV Limit(V)", "OV Trip Delay (ms)",
      "Trip Delay (ms)",
      "Trip Reset Delay (ms)", "Trip Max Resets", "Reset Trips", "Max Dead Counter"};
  JPopupMenu menu = new JPopupMenu();
  JPopupMenu hmenu = new JPopupMenu();
  protected boolean isEditingValue = false;

  static int openFrameCount = 0;
  static final int offset = 30;
  static final int DEF_HOSTID = 100;
  static final int DEF_HVCARD = 1;
  static final int DEF_BUSADDR = 0;
  String CSC = "ME+/1/1/1";
  int HostID = DEF_HOSTID;
  int HVCard = DEF_HVCARD;
  int BusAddr = DEF_BUSADDR;
  JSplitPane jSplitPane1 = new JSplitPane();
  JPanel jPanel1 = new JPanel();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  JLabel lblHVCard = new JLabel();
  JLabel lblHostId = new JLabel();
  JLabel lblBusAddr = new JLabel();
  JLabel lblTimerDelay = new JLabel("Update time (sec)");

  SpinnerNumberModel modelHostID;
  JSpinner spHostID;
  SpinnerNumberModel modelHVCard;
  JSpinner spHVCard;
  SpinnerNumberModel modelBusAddr;
  JSpinner spBusAddr;
  SpinnerNumberModel modelTimerDelay;
  JSpinner spTimerDelay; // = new JSpinner();
  BorderLayout borderLayout1 = new BorderLayout();
  TitledBorder titledBorder1 = new TitledBorder("");
  JPanel jPanel2 = new JPanel();
  JPanel jPanel3 = new JPanel();
  // AbstractTableModel HVTableModel;
  HVDataTableModelEx hvTableModel;
  // JPanel InfoPanel = new JPanel();
  // JPanel PlotsPanel = new JPanel();
  JTabbedPane PlotsPanel = new JTabbedPane();
  HVTimeChart VmonPlotsPanel = new HVTimeChart(0, "Ch#", "Voltage, V", 300);
  HVTimeChart ImonPlotsPanel = new HVTimeChart(0, "Ch#", "Current, mkA", 300);
  BorderLayout borderLayout3 = new BorderLayout();
  // JSplitPane LogPanel = new JSplitPane();
  JPanel jPanel4 = new JPanel();
  JScrollPane jScrollPane1 = new JScrollPane();
  HVcscdata cscdata;
  int delay = 2000;
  Timer timer;
  byte[] hvbuf;

  DimRpcInfo hvRpcData;
  DimRpcInfo hvRpcCmd;
  JTextArea Log = new JTextArea();
  GridLayout gridLayout1 = new GridLayout();

  JLabel infoCSCID = new JLabel();
  JLabel infoEndCap = new JLabel();
  JLabel infoStation = new JLabel();
  JLabel infoRing = new JLabel();
  JLabel infoPosition = new JLabel();

  JLabel infoHostID = new JLabel();
  JLabel infoPCIBus = new JLabel();
  JLabel infoPCISlot = new JLabel();
  JLabel infoDataslot = new JLabel();
  JLabel infoNumChans = new JLabel();
  JLabel infoBusAddr = new JLabel();
  JLabel infoID = new JLabel();
  JLabel infoType = new JLabel();
  JLabel infoHVMon = new JLabel();
  JLabel infoNeg5V = new JLabel();
  JLabel infoPos5V = new JLabel();
  JLabel infoInterlock = new JLabel();
  JLabel infoILockDelay = new JLabel();
  JLabel infoILockDelayCnt = new JLabel();
  JLabel infoMasterID = new JLabel();
  JLabel infoMasterHostID = new JLabel();
  JLabel infoMasterHVCard = new JLabel();
  JLabel infoMasterBusAddr = new JLabel();
  JLabel infoMasterChan = new JLabel();
  JLabel infoMasterTripDelay = new JLabel();
  JLabel infoState = new JLabel();
  JLabel infoStatus = new JLabel();
  JLabel infoPolicy = new JLabel();
  JLabel infoPrimaryStatus = new JLabel();
  JPanel jPanel5 = new JPanel();
  BorderLayout borderLayout2 = new BorderLayout();
  JLabel statusBar = new JLabel();
  Calendar cal;
  java.text.SimpleDateFormat sdf;
  JCheckBox cbTimerCtrl = new JCheckBox();
  JTable DataTable; // = new JTable();
  JScrollPane DataPanel = new JScrollPane();

  public HVCSCFrame(int hostid, String csc) {
    super("", true, true, true, true);
    HostID = hostid;
    CSC = csc;
    create();

  };

  private void create() {

    openFrameCount++;
    try {
      hvRpcData = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID + "/HV_CSC_DATA");
      hvRpcCmd = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID + "/HV_CSC_CMD");
      cscdata = new HVcscdata();
      jbInit();

      String DATE_FORMAT = "HH:mm:ss";
      sdf = new java.text.SimpleDateFormat(DATE_FORMAT);

      ActionListener taskPerformer = new ActionListener() {
        public void actionPerformed(ActionEvent evt) {
          cal = Calendar.getInstance(TimeZone.getDefault());
          UpdateRealHVData();
          /*
                     if (UpdateRealHVData() > 0) {
            UpdateHVDataWindow();
                     }
           */
        }
      };
      timer = new Timer(delay, taskPerformer);
      timer.start();
      setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
    } ;

    public HVCSCFrame() {
      super("", true, true, true, true);
      create();
    }

    int UpdateRealHVData() {
      HVcsccmd cmd = new HVcsccmd();
      cmd.csc = CSC;
      cmd.chan = 0;
      cmd.data = new Long(0);
      cmd.size = new Long(0);
      cmd.encode();
      int res = 0;
      if ( (res = hvRpcData.setData(cmd.encode())) > 0 &&
          (hvRpcData.getSize() == cscdata.getSize())) {
        // System.out.println(hvRpcData.getSize()+ " " + cscdata.getSize());
        cscdata.decode(hvRpcData.getData());
        UpdateHVDataWindow();
      }
      else {
        res = 0;
        statusBar.setText("Status: Data Error");
        System.out.println(hvRpcData.getSize()+ " " + cscdata.getSize());
        // System.out.println(hvRpcData.getServiceName() + ": Data Error");
      }
      return res;
    }



    void UpdateHVDataWindow() {

      if (DataTable.isEditing() || (DataTable.getSelectedRowCount() > 1) ||
          isEditingValue || menu.isVisible()) {
        statusBar.setText(
            " Status: Not Updated. Cell Editing or Selection Detected");
        return;
      }

      Object[][] chdata = {};

      if (cscdata.getSize() > 0) {

        String name = cscdata.csc.name;
        infoCSCID.setText("CSC ID: " + name);
        infoEndCap.setText("EndCap: " + Integer.toString(cscdata.csc.endcap));
        infoStation.setText("Station: " + Integer.toString(cscdata.csc.station));
        infoRing.setText("Ring: " + Integer.toString(cscdata.csc.ring));
        infoPosition.setText("Position: " +
                             Integer.toString(cscdata.csc.position));

        infoHostID.setText("HostID: " +
                           Integer.toString(cscdata.hvcard.hostid));
        infoPCIBus.setText("PCI Bus: " + Integer.toString(cscdata.hvcard.bus));
        infoPCISlot.setText("PCI Slot: " +
                            Integer.toString(cscdata.hvcard.slot));
        infoDataslot.setText("Dataslot: " +
                             Integer.toString(cscdata.hvcard.dataslot));
        infoType.setText("Type: " +
                         HVData.HV_TYPE.getValue(cscdata.rdb.type).toString());
        infoID.setText("ID: " + Integer.toString(cscdata.rdb.ID));
        infoBusAddr.setText("Bus Addr: " +
                            Integer.toString(cscdata.rdb.busaddr));
        infoNumChans.setText("# Channels: " +
                             Integer.toString(cscdata.rdb.num_chans));
        infoHVMon.setText("HVMon: " +
                          Integer.toString(cscdata.rdb.hvmon) + "V");
        infoNeg5V.setText("-5V: " + Integer.toString(cscdata.rdb.pos5v));
        infoPos5V.setText("+5V: " + Integer.toString(cscdata.rdb.neg5v));
        infoInterlock.setText("Interlock: " +
                              Integer.toString(cscdata.rdb.interlock));
        infoILockDelay.setText("ILock delay: " +
                               Integer.toString(cscdata.rdb.ilock_delay) +
                               "ms");
        infoILockDelayCnt.setText("ILock delay cnt: " +
                                  Integer.toString(cscdata.rdb.
            ilock_delay_cnt) + "ms");
        infoMasterID.setText("Master ID: " +
                             Integer.toString(cscdata.rdb.master_id));
        infoMasterHostID.setText("Master HostID: " +
                                 Integer.toString(cscdata.rdb.
                                                  master_hostid));
        infoMasterHVCard.setText("Master HVCard: " +
                                 Integer.toString(cscdata.rdb.
                                                  master_hvcard));
        infoMasterBusAddr.setText("Master BusAddr: " +
                                  Integer.toString(cscdata.rdb.
            master_busaddr));
        infoMasterChan.setText("Master Channel: " +
                               Integer.toString(cscdata.rdb.master_chan +
                                                1));
        infoMasterTripDelay.setText("Master TripDelay: " +
                                    Integer.toString(cscdata.rdb.
            master_trip_delay) + "ms");
        infoState.setText("State: " +
                          HVData.HV_STATE.getValue(cscdata.rdb.state).desc());
        infoStatus.setText("Status: " +
                           HVData.HV_STATE.getValue(cscdata.rdb.status).desc());
        infoPolicy.setText("Policy: 0x" +
                           Integer.toHexString(cscdata.rdb.policy));
        infoPrimaryStatus.setText("Primary Status: " +
                   HVData.HV_STATE.getValue(cscdata.rdb.pr_status).desc());

        // chdata = cscdata.rdb.getRealChanValues();
        chdata = cscdata.getCSCChanValues();

        int vseries = VmonPlotsPanel.getNumSeries();
        if (vseries != chdata.length - 1) {
          VmonPlotsPanel.setNumSeries(chdata.length - 1);
          vseries = chdata.length - 1;
        }
        HVchannel ch = new HVchannel();
        for (int i = 0; i < vseries; i++) {
          Object value = chdata[i][ch.getIndex("Vmon")];
          VmonPlotsPanel.addValue(i, ( (Integer) value));
        }

        int iseries = ImonPlotsPanel.getNumSeries();
        if (iseries != chdata.length - 1) {
          ImonPlotsPanel.setNumSeries(chdata.length - 1);
          iseries = chdata.length - 1;
        }
        for (int i = 0; i < iseries; i++) {

          Object value = chdata[i][ch.getIndex("Imon")];
          if (value.getClass() == Float.class) {
            ImonPlotsPanel.addValue(i, ( (Float) value));
          }
        }

      }
      else {
        // Log.append("Empty buffer received\n");
        SetInitialInfoText();
      }
      if (!DataTable.isEditing()) {
        hvTableModel.setData(chdata);
      }

      statusBar.setText(" Status: Last Updated at " + sdf.format(cal.getTime()));

    }

    void SetInitialInfoText() {

      infoCSCID.setText("CSC ID: ---");
      infoEndCap.setText("EndCap: ---");
      infoStation.setText("Station: ---");
      infoRing.setText("Ring: ---");
      infoPosition.setText("Position: ---");

      infoHostID.setText("HostID: ---");
      infoPCIBus.setText("PCI Bus: ---");
      infoPCISlot.setText("PCI Slot: ---");
      infoDataslot.setText("Dataslot: ---");
      infoType.setText("Type: ---");
      infoID.setText("ID: ---");
      infoBusAddr.setText("Bus Addr: ---");
      infoNumChans.setText("Channels: ---");
      infoHVMon.setText("HVMon: ---V");
      infoNeg5V.setText("-5V: ---");
      infoPos5V.setText("+5V: ---");
      infoInterlock.setText("Interlock: ---");
      infoILockDelay.setText("ILock delay: ---ms");
      infoILockDelayCnt.setText("ILock delay cnt: ---ms");
      infoMasterID.setText("Master ID: ---");
      infoMasterHostID.setText("Master HostID: ---");
      infoMasterHVCard.setText("Master HVCard: ---");
      infoMasterBusAddr.setText("Master BusAddr: ---");
      infoMasterChan.setText("Master Channel: ---");
      infoMasterTripDelay.setText("Master TripDelay: ---ms");
      infoState.setText("State: ---");
      infoStatus.setText("Status: ---");
      infoPolicy.setText("Policy: 0x---");
      infoPrimaryStatus.setText("Primary Status: ---");
    }

    public void Place() {
      int left = offset * openFrameCount;
      int top = offset * openFrameCount;
      JDesktopPane d = this.getDesktopPane();
      if (d != null) {
        Dimension dsize = d.getSize();

        if ((dsize.width - 800)>0) {
          if (left + 800 > dsize.width)
            left = offset * (openFrameCount % ( ( (dsize.width - 800) / offset) + 1));
        } else left = 0;

        if ((dsize.height - 800)>0) {
          if (top + 800 > dsize.height)
            top = offset * (openFrameCount % ( ( (dsize.height - 800) / offset) + 1));
        }  else top = 0;

       // System.out.println(openFrameCount + " " + left + " " + top + " " + dsize.width + " " + dsize.height);
        setLocation(left, top);
      }

    }


    private void jbInit() throws Exception {
      jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);

      setContentPane(jSplitPane1);
      this.addInternalFrameListener(new HVCSCFrame_this_internalFrameAdapter(this));
      pack();
      setSize(800, 800);

      jSplitPane1.setDividerLocation(100);
      jPanel1.setLayout(borderLayout1);

      modelHostID = new SpinnerNumberModel(HostID, 0, 1000, 1);
      spHostID = new JSpinner(modelHostID);
      modelHVCard = new SpinnerNumberModel(HVCard, 0, 16, 1);
      spHVCard = new JSpinner(modelHVCard);
      modelBusAddr = new SpinnerNumberModel(BusAddr, 0, 15, 1);
      spBusAddr = new JSpinner(modelBusAddr);
      modelTimerDelay = new SpinnerNumberModel(delay / 1000, 1, 100000, 1);
      spTimerDelay = new JSpinner(modelTimerDelay);

      lblHVCard.setMaximumSize(new Dimension(55, 16));
      lblHVCard.setMinimumSize(new Dimension(55, 16));
      lblHVCard.setPreferredSize(new Dimension(55, 16));

      lblHVCard.setText("HV Card:");
      lblBusAddr.setMaximumSize(new Dimension(80, 16));
      lblBusAddr.setMinimumSize(new Dimension(80, 16));
      lblBusAddr.setPreferredSize(new Dimension(80, 16));
      lblBusAddr.setText("Bus Address:");
      lblHostId.setMaximumSize(new Dimension(50, 16));
      lblHostId.setMinimumSize(new Dimension(50, 16));
      lblHostId.setPreferredSize(new Dimension(50, 16));
      lblHostId.setText("HostID:");
      spHostID.addChangeListener(new HVCSCFrame_spinners_changeAdapter(this));
      spHVCard.addChangeListener(new HVCSCFrame_spinners_changeAdapter(this));
      spBusAddr.addChangeListener(new HVCSCFrame_spinners_changeAdapter(this));
      spHostID.setMinimumSize(new Dimension(65, 20));
      spHostID.setPreferredSize(new Dimension(55, 20));
      // InfoPanel.setLayout(borderLayout3);
      // LogPanel.setOrientation(JSplitPane.VERTICAL_SPLIT);
      jPanel3.setLayout(gridLayout1);
      gridLayout1.setColumns(4);
      gridLayout1.setHgap(10);
      gridLayout1.setRows(5);

      SetInitialInfoText();
      jPanel5.setLayout(borderLayout2);
      statusBar.setText("Status: ");
      spTimerDelay.setMinimumSize(new Dimension(60, 18));
      spTimerDelay.setPreferredSize(new Dimension(60, 18));
      spTimerDelay.setToolTipText("Set updates interval in sec");
      spTimerDelay.addChangeListener(new
                                     HVCSCFrame_spTimerDelay_changeAdapter(this));
      lblTimerDelay.setText("Update (sec):");
      cbTimerCtrl.setSelected(true);
      cbTimerCtrl.setText("Update On/Off");
      cbTimerCtrl.addActionListener(new HVCSCFrame_cbTimerCtrl_actionAdapter(this));
      /*
             jPanel2.add(lblHostId);
             jPanel2.add(spHostID);
             jPanel2.add(lblHVCard);
             jPanel2.add(spHVCard);
             jPanel2.add(lblBusAddr);
             jPanel2.add(spBusAddr);
       */
      jPanel2.add(infoCSCID);
      jPanel2.add(infoEndCap);
      jPanel2.add(infoStation);
      jPanel2.add(infoRing);
      jPanel2.add(infoPosition);

      jPanel2.add(lblTimerDelay);
      jPanel2.add(spTimerDelay);
      jPanel2.add(cbTimerCtrl);

      jPanel3.add(infoHostID);
      jPanel3.add(infoPCIBus);
      jPanel3.add(infoPCISlot);
      jPanel3.add(infoDataslot);
      jPanel3.add(infoType);
      jPanel3.add(infoID);
      jPanel3.add(infoBusAddr);
      jPanel3.add(infoNumChans);
      jPanel3.add(infoInterlock);
      jPanel3.add(infoILockDelay);
      jPanel3.add(infoILockDelayCnt);
      jPanel3.add(infoPos5V);
      jPanel3.add(infoNeg5V);
      jPanel3.add(infoHVMon);
      jPanel3.add(infoMasterID);
      jPanel3.add(infoMasterHostID);
      jPanel3.add(infoMasterHVCard);
      jPanel3.add(infoMasterBusAddr);
      jPanel3.add(infoMasterChan);
      jPanel3.add(infoMasterTripDelay);
      jPanel3.add(infoState);
      jPanel3.add(infoStatus);
      jPanel3.add(infoPolicy);
      jPanel3.add(infoPrimaryStatus);

      jPanel1.add(jPanel2, java.awt.BorderLayout.NORTH);
      jPanel1.add(jPanel3, java.awt.BorderLayout.CENTER);

      DataTable = new JTable() {

        public void tableStructureChanged() {
          TableColumnModel tcm = this.getColumnModel();
          TableModel tm = this.getModel();
          if (tm instanceof HVDataTableModelEx) {
            int pos = ( (HVDataTableModelEx) tm).getVisibleColumnIndex("Status");
            if (pos >= 0) {
              TableColumn tc = tcm.getColumn(pos);
              tc.setCellRenderer(new HVTableRenderer());
            }
            pos = ( (HVDataTableModelEx) tm).getVisibleColumnIndex("State");
            if (pos >= 0) {
              TableColumn tc = tcm.getColumn(pos);
              tc.setCellRenderer(new HVTableRenderer());
              tc.setCellEditor(new HVTableEditor());
              // tc.setCellEditor(new StateComboCellEditor(new String[] {"OFF","ON"}));
              // tc.setCellEditor(new StateComboCellEditor(new String[] {"OFF","ON"}));
            }

          }
        }

        public void tableChanged(TableModelEvent e) {
          // System.out.println("Table changed event: "+ e.getColumn());
          // assignCellRenderers();
          super.tableChanged(e);
          if (e.getType() == TableModelEvent.UPDATE) {
            tableStructureChanged();
          }

        } //tableChanged

        protected JTableHeader createDefaultTableHeader() {
          return new JTableHeader(columnModel) {
            public String getToolTipText(MouseEvent e) {
              String tip = null;
              java.awt.Point p = e.getPoint();
              int index = columnModel.getColumnIndexAtX(p.x);
              int realIndex =
                  columnModel.getColumn(index).getModelIndex();
              return getModel().getColumnName(index);

            }
          };
        }

        public void editingStopped(ChangeEvent e) {
          // timer.stop();
          super.editingStopped(e);
          try {

            int currentRow = DataTable.getSelectedRow();
            int currentCol = DataTable.getSelectedColumn();

            Object value = DataTable.getValueAt(currentRow, currentCol);
            HVcsccmd cmd = new HVcsccmd();
            // cmd.card = HVCard;
            // cmd.module = BusAddr;
            cmd.csc = CSC;
            cmd.cmd = HVData.HV_CMD.HV_GET_MOD_DATA.ordinal();
            cmd.chan = (currentRow == 0) ? HVData.ALL : currentRow - 1;
            cmd.data = new Long(0);
            cmd.size = new Long(0);

            if (DataTable.getColumnName(currentCol).startsWith("State")) {
              cmd.data = new Long(("ON".equals(value)) ? 1 : 0);
              if (cmd.chan == HVData.ALL) {
                cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
                cmd.chan = 0;
                hvRpcCmd.setData(cmd.encode());
              }
              cmd.chan = (currentRow == 0) ? HVData.ALL : currentRow - 1;
              cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
            }

            if (DataTable.getColumnName(currentCol).startsWith("Vset")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (DataTable.getColumnName(currentCol).startsWith("RampUp")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_UP.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (DataTable.getColumnName(currentCol).startsWith("RampDown")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_DOWN.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (DataTable.getColumnName(currentCol).startsWith("Vmax")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_VMAX.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (DataTable.getColumnName(currentCol).startsWith("Imax")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_IMAX.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (DataTable.getColumnName(currentCol).startsWith("RampImax")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_IMAX_RAMP.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if ( (DataTable.getColumnName(currentCol).startsWith("Vtrip")) ||
                (DataTable.getColumnName(currentCol).startsWith("Itrip"))) {
              cmd.cmd = HVData.HV_CMD.HV_RESET_TRIP.ordinal();
              cmd.data = new Long(0);
            }

            if (DataTable.getColumnName(currentCol).startsWith("Vov")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_OV_LIMIT.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if (DataTable.getColumnName(currentCol).startsWith("OVTripDelay")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_OV_TRIP_DELAY.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if (DataTable.getColumnName(currentCol).startsWith("TripDelay")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_DELAY.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if (DataTable.getColumnName(currentCol) == "TripResetDelay") {
              cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_RESET_DELAY.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if (DataTable.getColumnName(currentCol).startsWith("TripMaxResets")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_MAX_RESETS.ordinal();
              cmd.data = new Long(( (Integer) value));
            }

            if (DataTable.getColumnName(currentCol).startsWith("DeadMaxCnt")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_DEAD_MAX_CNT.ordinal();
              cmd.data = new Long(( (Integer) value));
            }


            hvRpcCmd.setData(cmd.encode());
            Log.append(sdf.format(cal.getTime()) + "> Module_" +
                       Integer.toString(HostID) + "." + Integer.toString(HVCard) +
                       "." + Integer.toString(BusAddr) +
                       "." + Integer.toString(cmd.chan) + " " +
                       HVData.HV_CMD.getValue(cmd.cmd).toString() +
                       " " + Long.toString(cmd.data) + "\n");

          }
          catch (Exception exception) {
            exception.printStackTrace();
          }
        } //editingStopped

      };

      // LogPanel.add(jPanel4, JSplitPane.TOP);
      // LogPanel.add(jScrollPane1, JSplitPane.BOTTOM);
      DataPanel.getViewport().add(DataTable);
      jScrollPane1.getViewport().add(Log);
      jSplitPane1.add(jPanel5, JSplitPane.BOTTOM);
      jSplitPane1.add(jPanel1, JSplitPane.TOP);
      jPanel5.add(jTabbedPane1, java.awt.BorderLayout.CENTER);
      jPanel5.add(statusBar, java.awt.BorderLayout.SOUTH);
      jTabbedPane1.add(DataPanel, "Data");
      // jTabbedPane1.add(InfoPanel, "Info");
      // jTabbedPane1.add(LogPanel, "Log");
      jTabbedPane1.add(PlotsPanel, "Plots");
      PlotsPanel.add(VmonPlotsPanel, "Voltage");
      PlotsPanel.add(ImonPlotsPanel, "Current");

      hvTableModel = new HVDataTableModelEx(new HVmodule().getChanHeaders());
      hvTableModel.setEditableColumn("Ch#", false);
      hvTableModel.setEditableColumn("Vcur", false);
      hvTableModel.setEditableColumn("Vmon", false);
      hvTableModel.setEditableColumn("Vset_adc", false);
      hvTableModel.setEditableColumn("Imon", false);
      hvTableModel.setEditableColumn("TripCnt", false);
      hvTableModel.setEditableColumn("OVTripCnt", false);
      hvTableModel.setEditableColumn("TripResetDelayCnt", false);
      hvTableModel.setEditableColumn("TripResetsCnt", false);
      hvTableModel.setEditableColumn("DeadCnt", false);
      hvTableModel.setEditableColumn("Relay", false);
      hvTableModel.setEditableColumn("Fuse", false);
      hvTableModel.setEditableColumn("Status", false);
      hvTableModel.setEditableColumn("RampCounter", false);

      hvTableModel.setVisibleColumn("Vset_adc", false);
      hvTableModel.setVisibleColumn("TripCnt", false);
      hvTableModel.setVisibleColumn("OVTripCnt", false);
      hvTableModel.setVisibleColumn("TripResetDelayCnt", false);
      hvTableModel.setVisibleColumn("TripResetsCnt", false);
      hvTableModel.setVisibleColumn("TripMaxResets", false);
      hvTableModel.setVisibleColumn("Flags", false);
      hvTableModel.setVisibleColumn("Relay", false);
      hvTableModel.setVisibleColumn("Fuse", false);
      hvTableModel.setVisibleColumn("RampCounter", false);

      KeyStroke inc_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_PERIOD, java.awt.event.InputEvent.CTRL_MASK);
      KeyStroke dec_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_COMMA, java.awt.event.InputEvent.CTRL_MASK);
      String inc_font_str = "inc_font";
      String dec_font_str = "dec_font";

      DataTable.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          inc_font_size, inc_font_str);
      DataTable.getActionMap().put(inc_font_str,
                                   new TableFontResizeAction(inc_font_str));
      DataTable.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          dec_font_size, dec_font_str);
      DataTable.getActionMap().put(dec_font_str,
                                   new TableFontResizeAction(dec_font_str));

      DataTable.setSelectionForeground(Color.BLUE);
      DataTable.setRowSelectionAllowed(true);
      // DataTable.setColumnSelectionAllowed(true);
      DataTable.setModel(hvTableModel);
      DataTable.setDefaultRenderer(Integer.class, new HVTableObjectRenderer());
      DataTable.setDefaultRenderer(Float.class, new HVTableFloatRenderer());
      TableColumnModel tcm = DataTable.getColumnModel();
      /*
       TableColumn tc = tcm.getColumn(hvTableModel.getColumnPos("State"));
             tc.setCellRenderer(new HVTableRenderer());
             tc.setCellEditor(new HVTableEditor());
             tc = tcm.getColumn(hvTableModel.getColumnPos("Status"));
             tc.setCellRenderer(new HVTableRenderer());
       */
      TableColumn tc = tcm.getColumn(hvTableModel.getColumnPos("Vtrip"));
      tc.setCellEditor(new HVTableResetTripEditor());
      tc = tcm.getColumn(hvTableModel.getColumnPos("Itrip"));
      tc.setCellEditor(new HVTableResetTripEditor());
      tc = tcm.getColumn(hvTableModel.getColumnPos("Vmon"));
      tc.setCellRenderer(new HVVoltageCheckCellRenderer(hvTableModel.
          getColumnPos("Vset"),
          HVClient.config.csc_mon_thresholds.vmon_max,
          hvTableModel.getColumnPos("State")));
      tc = tcm.getColumn(hvTableModel.getColumnPos("Imon"));
      tc.setCellRenderer(new HVCurrentCheckCellRenderer( -1,
          HVClient.config.csc_mon_thresholds.imon_max));

      menu = CreatePopupMenu();
      DataTable.addMouseListener(new MouseAdapter() {
        public void mousePressed(MouseEvent evt) {
          if (evt.isPopupTrigger()) {
            menu.show(evt.getComponent(), evt.getX(), evt.getY());
          }
        }

        public void mouseReleased(MouseEvent evt) {
          if (evt.isPopupTrigger()) {
            menu.show(evt.getComponent(), evt.getX(), evt.getY());
          }
        }
      });

      hmenu = CreateHeaderPopupMenu(hvTableModel);

      DataTable.getTableHeader().addMouseListener(new MouseAdapter() {
        public void mousePressed(MouseEvent evt) {
          if (evt.isPopupTrigger()) {
            hmenu.show(evt.getComponent(), evt.getX(), evt.getY());
          }
        }

        public void mouseReleased(MouseEvent evt) {
          if (evt.isPopupTrigger()) {
            hmenu.show(evt.getComponent(), evt.getX(), evt.getY());
          }
        }
      });

      /*
            final JPopupMenu menu = new JPopupMenu();

            for (int i = 0; i < hvTableModel.getColumnCount(); i++) {
       menu.add(new JCheckBoxMenuItem(hvTableModel.getColumnName(i), true));
            }
            // Set the component to show the popup menu
            DataTable.getTableHeader().addMouseListener(new MouseAdapter() {
              public void mousePressed(MouseEvent evt) {
                if (evt.isPopupTrigger()) {
                  menu.show(evt.getComponent(), evt.getX(), evt.getY());
                }
              }

              public void mouseReleased(MouseEvent evt) {
                if (evt.isPopupTrigger()) {
                  menu.show(evt.getComponent(), evt.getX(), evt.getY());
                }
              }
            });
       */
      hvTableModel.fireTableStructureChanged();
      updateTitle();
      // LogPanel.setDividerLocation(100);
    }

    private JPopupMenu CreateHeaderPopupMenu(HVDataTableModelEx model) {
      JPopupMenu menu = new JPopupMenu();
      for (Object hdr : model.getAllHeaders()) {
        JCheckBoxMenuItem item = new JCheckBoxMenuItem(hdr.toString(),
            model.isVisible(hdr));
        item.addActionListener(new ActionListener() {

          public void actionPerformed(ActionEvent e) {
            if (e.getSource() instanceof JMenuItem) {
              Object col = ( (JCheckBoxMenuItem) e.getSource()).getText();
              boolean state = ( (JCheckBoxMenuItem) e.getSource()).getState();
              setColumnVisible(col, state);
            }
          }
        });

        menu.add(item);
      }
      menu.addSeparator();
      return menu;
    }

    private void setColumnVisible(Object col, boolean flag) {
      hvTableModel.setVisibleColumn(col, flag);
    }

    private JPopupMenu CreatePopupMenu() {
      JPopupMenu menu = new JPopupMenu();
      JMenu set_parameters_menu = new JMenu(
          "Set Parameters for selected channels");
      menu.add(set_parameters_menu);
      for (int i = 0; i < set_params_list.length; i++) {
        JMenuItem param = new JMenuItem(set_params_list[i]);
        param.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            if (e.getSource() instanceof JMenuItem) {
              SetParameter( ( (JMenuItem) (e.getSource())).getText());
            }
          }
        });
        set_parameters_menu.add(param);
      }

      return menu;
    }

    private void SetParameter(String param) {

      isEditingValue = true;
      JTable table = DataTable;
      HVcsccmd cmd = new HVcsccmd();
      cmd.csc = CSC;
      cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
      cmd.chan = 0;
      cmd.data = new Long(0);
      cmd.size = new Long(0);

      if (param.startsWith("State")) {
        String[] states = {
            "OFF", "ON"};

        String value = (String) JOptionPane.showInputDialog(
            this,
            "Set State:",
            "Set State",
            JOptionPane.QUESTION_MESSAGE,
            null,
            states,
            states[0]);
        if (value == null) {
          return;
        }
        cmd.data = new Long(("ON".equals(value)) ? 1 : 0);
        cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
        // rpcCSC.setData(cmd.encode());

      }

      if (param.startsWith("Reset Trips")) {
        cmd.cmd = HVData.HV_CMD.HV_RESET_TRIP.ordinal();
      }

      try {
        if (param.startsWith("Vset")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 3600);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 4000) {
              cmd.data = new Long(4000);
            }
          }
        }

        if (param.startsWith("Ramp Up")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 15);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_UP.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 4000) {
              cmd.data = new Long(4000);
            }
          }
        }

        if (param.startsWith("Ramp Down")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 100);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_DOWN.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 4000) {
              cmd.data = new Long(4000);
            }
          }
        }

        if (param.startsWith("Imax")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 10);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_IMAX.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 200) {
              cmd.data = new Long(200);
            }
          }
        }

        if (param.startsWith("RampImax")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 10);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_IMAX_RAMP.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 200) {
              cmd.data = new Long(200);
            }
          }
        }

        if (param.startsWith("OV Limit")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 20);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_OV_LIMIT.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data < 0) {
              cmd.data = new Long(0);
            }
          }
        }

        if (param.startsWith("OV Trip Delay")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 200);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_OV_TRIP_DELAY.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data < 20) {
              cmd.data = new Long(20);
            }
          }
        }

        if (param.startsWith("Trip Delay")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 100);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_DELAY.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data < 20) {
              cmd.data = new Long(20);
            }
          }
        }

        if (param.startsWith("Trip Reset Delay")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 30000);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_RESET_DELAY.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data < 2000) {
              cmd.data = new Long(2000);
            }
          }
        }

        if (param.startsWith("Trip Max Resets")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 1);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_MAX_RESETS.ordinal();
            cmd.data = new Long(Integer.parseInt(value.toString()));
            if (cmd.data > 20) {
              cmd.data = new Long(20);
            }
          }
        }

        if (param.startsWith("Max Dead Counter")) {
         String value = JOptionPane.showInputDialog("Enter " + param +
             " value:", 20);
         if (value == null) {
           return;
         }
         if (value != null && value.length() > 0) {
           cmd.cmd = HVData.HV_CMD.HV_SET_DEAD_MAX_CNT.ordinal();
           cmd.data = new Long(Integer.parseInt(value.toString()));
           if (cmd.data < 0 ) {
             cmd.data = new Long(0);
           }
         }
       }


        if (table.getSelectedRowCount() > 0) {
          int rowIndexStart = table.getSelectedRow();
          int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();
          for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
            if (table.isCellSelected(r, 0)) {
              // Get CSC Name
              // String CSC = (String) table.getModel().getValueAt(r, 0);
              String Channel = table.getModel().getValueAt(r, 0).toString();

              // Skip Master Channel selection
              if (Channel.startsWith("MASTER")) {
                continue;
              }

              if (Channel.startsWith("ALL")) {
                cmd.chan = HVData.ALL;
              }
              else {
                cmd.chan = Integer.parseInt(Channel) - 1;
              }
              hvRpcCmd.setData(cmd.encode());
              // System.out.println(cmd.toString());
              // cell is selected
            }
          }
          table.setRowSelectionInterval(rowIndexStart, rowIndexStart);
        }

      }
      catch (NumberFormatException nfe) {
        JOptionPane.showMessageDialog(this,
                                      "Exception",
                                      nfe.toString(),
                                      JOptionPane.ERROR_MESSAGE);
        cmd.data = new Long(0);
        cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
      }

      isEditingValue = false;

    }

    public void updateTitle() {
      setTitle(HostID + ":" + CSC);
    }

    public void spinners_stateChanged(ChangeEvent e) {

      timer.stop();
      HostID = modelHostID.getNumber().intValue();
      HVCard = modelHVCard.getNumber().intValue();
      BusAddr = modelBusAddr.getNumber().intValue();
      updateTitle();
      UpdateRealHVData();
      UpdateHVDataWindow();
      if (cbTimerCtrl.isSelected()) {
        timer.start();
      }
      else {
        timer.stop();
      }
      ;
    }

    public void this_internalFrameClosing(InternalFrameEvent e) {
      timer.stop();
      hvRpcData.finalize();
      openFrameCount--;
      // System.gc();
      // System.runFinalization();
    }

    public void this_internalFrameClosed(InternalFrameEvent e) {
      // timer.stop();
    }

    public void spTimerDelay_stateChanged(ChangeEvent e) {
      delay = 1000 * modelTimerDelay.getNumber().intValue();
      timer.setDelay(delay);
    }

    public void cbTimerCtrl_actionPerformed(ActionEvent e) {
      if (cbTimerCtrl.isSelected()) {
        timer.start();
      }
      else {
        timer.stop();
      }

    }
  }

  class HVCSCFrame_cbTimerCtrl_actionAdapter
      implements ActionListener {
    private HVCSCFrame adaptee;
    HVCSCFrame_cbTimerCtrl_actionAdapter(HVCSCFrame adaptee) {
      this.adaptee = adaptee;
    }

    public void actionPerformed(ActionEvent e) {
      adaptee.cbTimerCtrl_actionPerformed(e);
    }
  }

  class HVCSCFrame_spTimerDelay_changeAdapter
      implements ChangeListener {
    private HVCSCFrame adaptee;
    HVCSCFrame_spTimerDelay_changeAdapter(HVCSCFrame adaptee) {
      this.adaptee = adaptee;
    }

    public void stateChanged(ChangeEvent e) {
      adaptee.spTimerDelay_stateChanged(e);
    }
  }

  class HVCSCFrame_this_internalFrameAdapter
      extends InternalFrameAdapter {
    private HVCSCFrame adaptee;
    HVCSCFrame_this_internalFrameAdapter(HVCSCFrame adaptee) {
      this.adaptee = adaptee;
    }

    public void internalFrameClosing(InternalFrameEvent e) {
      adaptee.this_internalFrameClosing(e);
    }
  }

  class HVCSCFrame_spinners_changeAdapter
      implements ChangeListener {
    private HVCSCFrame adaptee;
    HVCSCFrame_spinners_changeAdapter(HVCSCFrame adaptee) {
      this.adaptee = adaptee;
    }

    public void stateChanged(ChangeEvent e) {
      adaptee.spinners_stateChanged(e);
    }
  }
