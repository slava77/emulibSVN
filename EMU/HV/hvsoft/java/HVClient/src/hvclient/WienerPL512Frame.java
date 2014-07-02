package hvclient;

import dim.*;
import javax.swing.*;
import java.util.TimeZone;
import java.util.Calendar;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.table.TableColumn;
import java.awt.BorderLayout;
import java.awt.event.MouseAdapter;
import javax.swing.table.JTableHeader;
import javax.swing.event.ChangeEvent;
import java.awt.Color;
import java.awt.event.MouseEvent;
import javax.swing.table.TableColumnModel;
import java.awt.Point;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import java.util.Vector;
import java.awt.GridLayout;
import java.awt.Font;
import java.util.Arrays;

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
public class WienerPL512Frame
    extends JInternalFrame {

  protected String[] set_params_list = {
      "State",
      "V output", "I limit",
      "V min sense", "V max sense", "Group"};

  protected String[] headers = {
      "Chan", "V sense mon, V", "I mon, A", "V terminal mon, V",
      "V output", "V adjust",
      "I limit", "V min sense", "V max sense",
      "V max terminal", "Imax", "Cfg V max sense",
      "Cfg V max terminal", "Cfg I max", "Max Power",
      "Group", "State", "Status"
  };

  protected boolean isEditingValue = false;

  String[] cmd_list;
  Calendar cal;
  java.text.SimpleDateFormat sdf;
  HVDataTableModel dtm;
  JPanel jPanel1 = new JPanel();
  JPanel DataPanel = new JPanel();
  JTable jt; //  = new JTable(dtm);
  JScrollPane jsp = new JScrollPane();
  JPopupMenu menu = new JPopupMenu();
  JTextArea Log = new JTextArea();
  JSplitPane LogPanel = new JSplitPane();
  JPanel jPanel4 = new JPanel();
  JScrollPane jScrollPane1 = new JScrollPane();
  JPanel jPanel2 = new JPanel();

  JTabbedPane jTabbedPane1 = new JTabbedPane();
  JTabbedPane PlotsPanel = new JTabbedPane();
  BorderLayout borderLayout1 = new BorderLayout();
  GridLayout gridLayout1 = new GridLayout();
  JLabel statusBar = new JLabel();
  HVTimeChart VmonPlotsPanel = new HVTimeChart(0, "Ch#", "Voltage, V", 600);
  HVTimeChart ImonPlotsPanel = new HVTimeChart(0, "Ch#", "Current, A", 600);

  JLabel lbl_nodeID = new JLabel("Node ID: ---");
  JLabel lbl_psOperatingTime = new JLabel("PS Operating Time: ---");
  JLabel lbl_psSerialNumber = new JLabel("PS Serial Number: ---");
  JLabel lbl_sysDescr = new JLabel("System Description: ---");
  JLabel lbl_sysMainSwitch = new JLabel("Main Switch State: ---");
  JLabel lbl_sysStatus = new JLabel("System Status: ---");
  JLabel lbl_sysUptime = new JLabel("System Uptime: ---");
  JButton btn_sysSwitch = new JButton("Switch System");

  int channels = 0;

  private Vector<DimInfo> dim_svcs;
  private Object[][] data; // = new Object[rows.length][headers.length];

  public WienerPL512Frame() {
    super("", true, true, true, true);
    create();
  }

  public WienerPL512Frame(int hostid) {
    super("", true, true, true, true);
    // HostID = hostid;
    create();
  }

  private void UpdateData(DimInfo svc) {

    String svc_name = svc.getName();

    if (svc_name.endsWith("nodeID")) {
      lbl_nodeID.setText("Node ID: " + svc.getString());
    }
    else if (svc_name.endsWith("sysDescr")) {
      lbl_sysDescr.setText("System Description: " + svc.getString());
    }
    else if (svc_name.endsWith("sysStatus")) {
      lbl_sysStatus.setText("System Status: 0x" +
                            Integer.toHexString(svc.getInt()));
    }
    else if (svc_name.endsWith("psSerialNumber")) {
      lbl_psSerialNumber.setText("PS Serial Number: " + svc.getString());
    }
    else if (svc_name.endsWith("psOperatingTime")) {
      lbl_psOperatingTime.setText("PS Operating Time: " +
                                  Integer.toString(svc.getInt()));
    }
    if (svc_name.endsWith("sysUpTime")) {
      lbl_sysUptime.setText("System Uptime: " + Float.toString(svc.getFloat()));
    }
    if (svc_name.endsWith("sysMainSwitch")) {
      int val = svc.getInt();
      lbl_sysMainSwitch.setText("Main Switch State: " +
                                ( (val == 0) ? "OFF" : "ON") + " (" +
                                Integer.toString(val) + ")");
    }

    if (svc_name.endsWith("/SYSDATA")) {
      Vector<Object> sys_data = new Vector<Object> ();
      String[] fmt = svc.getFormat().split(";");

      for (String entry : fmt) {
        String[] par = entry.split(":");
        if (par.length == 2) {
          int n = new Integer(par[1]);
          if (par[0].compareTo("C") == 0) {
            // Log.append(svc.getString(n) + "\n");
            sys_data.add(svc.getString(n));
          }
          if (par[0].compareTo("I") == 0) {
            for (int i = 0; i < n; i++) {
              Integer val = svc.getInt();
              sys_data.add(val);
            }
          }
          if (par[0].compareTo("L") == 0) {
            for (int i = 0; i < n; i++) {
              Long val = svc.getLong();
              sys_data.add(val);
            }
          }

          if (par[0].compareTo("F") == 0) {
            for (int i = 0; i < n; i++) {
              Float val = svc.getFloat();
              sys_data.add(val);
            }
          }
        }
      }

      // Asssume that data structure is dimWienerPS_system and has 8 elements
      if (sys_data.size() == 8+8) {
        if (sys_data.elementAt(0).getClass() == Integer.class) {
          lbl_sysStatus.setText("System Status: 0x" +
                                Integer.toHexString( (Integer) sys_data.elementAt(0)));
        }
        if (sys_data.elementAt(1).getClass() == Integer.class) {
          int val = (Integer) sys_data.elementAt(1);
          lbl_sysMainSwitch.setText("Main Switch State: " +
                                    ( (val == 0) ? "OFF" : "ON") + " (" +
                                    Integer.toString(val) + ")");
        }
        if (sys_data.elementAt(2).getClass() == Integer.class) {
          lbl_psOperatingTime.setText("PS Operating Time: " +
                                      Integer.toString( (Integer) sys_data.
              elementAt(2)));
        }
        if (sys_data.elementAt(3).getClass() == Float.class) {
          lbl_sysUptime.setText("System Uptime: " +
                                Float.toString( (Float) sys_data.elementAt(3)));
        }
        lbl_nodeID.setText("Node ID: " + sys_data.elementAt(4));
        lbl_sysDescr.setText("System Description: " + sys_data.elementAt(5));
        lbl_psSerialNumber.setText("PS Serial Number: " + sys_data.elementAt(6));
      }
    }

    if (svc_name.endsWith("/DATA")) {

      // Log.append(svc.getName() + "|" + svc.getFormat() + "\n");
//      svc_name = svc_name.replaceAll("/DATA","");
      int index = new Integer(svc_name.substring(svc_name.lastIndexOf("/CHAN") +
                                                 5,
                                                 svc_name.lastIndexOf("/DATA")));
      // System.out.println(svc_name);
      Vector<Object> ch_data = new Vector<Object> ();
      String[] fmt = svc.getFormat().split(";");
      // Log.append(new Integer(fmt.length).toString() + "\n");
      for (String entry : fmt) {
        String[] par = entry.split(":");
        // Log.append(new Integer(par.length).toString() + "\n");
        /*
                 if (par.length == 1) {
          if (par[0].compareTo("C") == 0) {
            Log.append(svc.getString() + "\n");
          }
          if (par[0].compareTo("I") == 0) {
            Integer val = svc.getInt();
            if (val < 0) {
              Log.append(Integer.toHexString(val) + "\n");
            }
            else {
              Log.append(new Integer(val).toString() + "\n");
            }
          }
          if (par[0].compareTo("L") == 0) {

            Long val = svc.getLong();
            if (val < 0) {
              Log.append(Long.toHexString(val) + "\n");
            }
            else {
              Log.append(new Long(val).toString() + "\n");

            }
          }

          if (par[0].compareTo("F") == 0) {
            Log.append(new Float(svc.getFloat()).toString() + "\n");
          }

                 }
         */
        if (par.length == 2) {
          int n = new Integer(par[1]);
          if (par[0].compareTo("C") == 0) {
            // Log.append(svc.getString(n) + "\n");
            ch_data.add(svc.getString(n));
          }
          if (par[0].compareTo("I") == 0) {
            for (int i = 0; i < n; i++) {
              Integer val = svc.getInt();
              ch_data.add(val);
              /*
                             if (val < 0) {
                Log.append(Integer.toHexString(val) + "\n");
                             }
                             else {
                Log.append(new Integer(val).toString() + "\n");
                             }
               */
            }
          }
          if (par[0].compareTo("L") == 0) {
            for (int i = 0; i < n; i++) {
              Long val = svc.getLong();
              ch_data.add(val);
              /*
                             if (val < 0) {
                Log.append(Long.toHexString(val) + "\n");
                             }
                             else {
                Log.append(new Long(val).toString() + "\n");
                             }
               */
            }
          }

          if (par[0].compareTo("F") == 0) {
            for (int i = 0; i < n; i++) {
              Float val = svc.getFloat();
              ch_data.add(val);
              //Log.append(new Float(svc.getFloat()).toString() + "\n");
            }
          }

        }
        if (ch_data.size() == headers.length+9) {

          if (ch_data.elementAt(1).getClass() == Float.class) {
            VmonPlotsPanel.addValue(index, ( (Float) ch_data.elementAt(1)));
          }

          if (ch_data.elementAt(2).getClass() == Float.class) {
            ImonPlotsPanel.addValue(index, ( (Float) ch_data.elementAt(2)));
          }

          // ch_data.remove(ch_data.lastElement());
          data[index] = ch_data.toArray();
          boolean fUpdateTable = true;
          for (int i = 0; i < channels; i++) {
            if (data[i][0].getClass() != String.class) {
              fUpdateTable = false;
            }
          }
          if (jt.isEditing() || (jt.getSelectedRowCount() > 1)
              || isEditingValue || menu.isVisible()) {

            statusBar.setText(
                " Status: Not Updated. Cell Editing or Selection Detected");
            return;
          }
          if (fUpdateTable) {
            cal = Calendar.getInstance(TimeZone.getDefault());
            dtm.setData(data);
            statusBar.setText(" Status: Last Updated at " +
                              sdf.format(cal.getTime()));
          }
        }
      }
    }
  }

  private void create() {

//    openFrameCount++;
    try {
      jbInit();
      String DATE_FORMAT = "HH:mm:ss";
      sdf = new java.text.SimpleDateFormat(DATE_FORMAT);

      String[] service_list = DimBrowserNew.getServices("*WIENER_PL512*");
      cmd_list = DimBrowserNew.getServices("*WIENER_PL512*CHAN*CMD");
      java.util.Arrays.sort(cmd_list);
      channels = DimBrowserNew.getServices("*WIENER_PL512*/DATA").length;
      data = new Object[channels][headers.length];
      VmonPlotsPanel.setNumSeries(channels);
      ImonPlotsPanel.setNumSeries(channels);
      for (int i = 0; i < channels; i++) {
        VmonPlotsPanel.setSerieTitle(i, "Ch" + Integer.toString(i));
        ImonPlotsPanel.setSerieTitle(i, "Ch" + Integer.toString(i));
      }

      dim_svcs = new Vector<DimInfo> ();
      for (String entry : service_list) {
//        String fmt = DimBrowserNew.getFormat(entry);
//        System.out.println(entry);
        if (!DimBrowserNew.isCommand(entry)) {
          DimInfo srv = new DimInfo(entry, -1) {
            public void infoHandler() {
              UpdateData(this);
            }
          };
          dim_svcs.add(srv);
        }
      }
      setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
    } ;

    private void jbInit() throws Exception {
      setContentPane(jPanel1);
      // dtm.setColumnCount(headers.length);

      this.addInternalFrameListener(new
                                    WienerPL512Frame_this_internalFrameAdapter(this));
      pack();
      setSize(800, 400);
      KeyStroke inc_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_PERIOD, java.awt.event.InputEvent.CTRL_MASK);
      KeyStroke dec_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_COMMA, java.awt.event.InputEvent.CTRL_MASK);
      String inc_font_str = "inc_font";
      String dec_font_str = "dec_font";

      jPanel1.setLayout(borderLayout1);
      lbl_sysMainSwitch.addMouseListener(new
                                         WienerPL512Frame_lbl_sysMainSwitch_mouseAdapter(this));
      Font fnt = lbl_sysMainSwitch.getFont();
      lbl_sysMainSwitch.setFont(new java.awt.Font(fnt.getName(), Font.BOLD,
                                                  fnt.getSize()));
      // lbl_sysMainSwitch.setFont(new java.awt.Font("Tahoma", Font.BOLD, 11));
      lbl_sysMainSwitch.setToolTipText(
          "Double-click to change Main Switch state");

      jPanel1.add(jTabbedPane1, java.awt.BorderLayout.CENTER);
      jPanel1.add(statusBar, java.awt.BorderLayout.SOUTH);
      statusBar.setText("Status: ");

      DataPanel.setLayout(new BorderLayout());
      jPanel2.setLayout(gridLayout1);
      gridLayout1.setColumns(2);
      gridLayout1.setHgap(10);
      gridLayout1.setRows(4);

      jPanel2.add(lbl_nodeID);
      jPanel2.add(lbl_sysDescr);
      jPanel2.add(lbl_psSerialNumber);
      jPanel2.add(lbl_psOperatingTime);
      jPanel2.add(lbl_sysUptime);
      jPanel2.add(lbl_sysStatus);
      // JPanel jPanel5 = new JPanel();
      jPanel2.add(lbl_sysMainSwitch);
      // jPanel5.add(btn_sysSwitch);

      // jPanel2.add(jPanel5);
      DataPanel.add(jPanel2, java.awt.BorderLayout.NORTH);
//      DataPanel.add(jPanel2);

      jTabbedPane1.add(DataPanel, "Data");

      LogPanel.setOrientation(JSplitPane.VERTICAL_SPLIT);
      LogPanel.add(jPanel4, JSplitPane.TOP);
      LogPanel.add(jScrollPane1, JSplitPane.BOTTOM);
      jScrollPane1.getViewport().add(Log);

      // jPanel2.setLayout(borderLayout1);


      jt = new JTable() {
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

            int currentRow = jt.getSelectedRow();
            int currentCol = jt.getSelectedColumn();

            Object value = jt.getModel().getValueAt(currentRow, currentCol);
            // cmd_list = DimBrowserNew.getServices(String.format("*WIENER_PL512*CHAN%02d*CMD", currentRow));
            String dim_cmd = "";
            if (cmd_list.length > 0) {
              dim_cmd = cmd_list[currentRow];
            }
            System.out.println("Row" + currentRow + " Column" + currentCol +
                               " " + dim_cmd);

            if (jt.getColumnName(currentCol) == "State") {
              if ("ON".equals(value)) {
                DimClient.sendCommandNB(dim_cmd, "SET_CHAN_ON");
              }
              if ("OFF".equals(value)) {
                DimClient.sendCommandNB(dim_cmd, "SET_CHAN_OFF");
              }
            }

            if (jt.getColumnName(currentCol) == "V output") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_OUTPUT_VOLT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "I limit") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_CURRENT_LIMIT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "V min sense") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_MIN_SENSE_VOLT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "V max sense") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_MAX_SENSE_VOLT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "V max terminal") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_MAX_TERMINAL_VOLT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "Imax") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_MAX_CURRENT=" +
                                      Float.toString( (Float) value));
            }
            if (jt.getColumnName(currentCol) == "Max Power") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_MAX_POWER=" +
                                      Float.toString( (Float) value/100));
            }

            if (jt.getColumnName(currentCol) == "Group") {
              DimClient.sendCommandNB(dim_cmd,
                                      "SET_CHAN_GROUP=" +
                                      Integer.toString( (Integer) value));
            }

          }
          catch (Exception exception) {
            exception.printStackTrace();
          }
        } //editingStopped

      };
      dtm = new HVDataTableModel(headers, false);
      // dtm.setEditableColumn("State",true);
      dtm.setEditableColumn("V output", true);
      dtm.setEditableColumn("I limit", true);
      dtm.setEditableColumn("V min sense", true);
      dtm.setEditableColumn("V max sense", true);
      dtm.setEditableColumn("V max terminal", true);
      dtm.setEditableColumn("Imax", true);
      dtm.setEditableColumn("Max Power", true);
      dtm.setEditableColumn("Group", true);
      dtm.setEditableColumn("State", true);

      jt.setSelectionForeground(Color.BLUE);
      jt.setRowSelectionAllowed(true);
      // jt.setColumnSelectionAllowed(true);
      jt.setModel(dtm);
      jt.setDefaultRenderer(Integer.class, new HVTableObjectRenderer());
      jt.setDefaultRenderer(Float.class, new HVTableFloatRenderer(3));
      TableColumnModel tcm = jt.getColumnModel();
      TableColumn tc = tcm.getColumn(dtm.getColumnPos("State"));
      tc.setCellRenderer(new HVTableRenderer());
      tc.setCellEditor(new HVTableEditor());
      tc = tcm.getColumn(dtm.getColumnPos("Status"));
      tc.setCellRenderer(new HVTableHexRenderer());

      // jsp.add(jt);
      jsp.getViewport().add(jt);
      DataPanel.add(jsp, java.awt.BorderLayout.CENTER);
      // jTabbedPane1.add(LogPanel, "Log");

      menu = CreatePopupMenu();
      jt.addMouseListener(new MouseAdapter() {
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

      PlotsPanel.add(VmonPlotsPanel, "Voltage");
      PlotsPanel.add(ImonPlotsPanel, "Current");
      jTabbedPane1.add(PlotsPanel, "Plots");

      setTitle("WienerPL512");
    }

    private JPopupMenu CreatePopupMenu() {
      JPopupMenu menu = new JPopupMenu();

      JMenuItem mparams = new JMenuItem(
          "Turn All Channels On/Off");
      mparams.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (e.getSource() instanceof JMenuItem) {
            SetAllChannelsState();
          }
        }
      });

      menu.add(mparams);

      JMenuItem mgroup = new JMenuItem(
          "Switch Channels Group On/Off");
      mgroup.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (e.getSource() instanceof JMenuItem) {
            SwitchChannelsGroup();
          }
        }
      });

      menu.add(mgroup);
      menu.addSeparator();

      JMenu set_parameters_menu = new JMenu(
          "Set single parameter for selected channels");
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

    private void SwitchChannelsGroup() {
      isEditingValue = true;
      int GAP = 10;
      Vector<HVParameter> params = new Vector<HVParameter> ();
      Integer[] rVset = {
          0, 128};
      params.add(new HVParameter("Group", "Group:", JTextField.class, 1,
                                 new Vector<Object> (Arrays.asList(rVset))));

      String[] rState = {
          "OFF", "ON"};
      params.add(new HVParameter("State", "State:", JComboBox.class, rState[0],
                                 new Vector<Object> (Arrays.asList(rState))));

      JPanel panel = new JPanel(new SpringLayout());
      JLabel[] labels = new JLabel[params.size()];
      //JComponent[] fields = new JComponent[params.size()];
      int fieldNum = 0;

      for (HVParameter par : params) {
        if (par.getComponent() != null) {
          labels[fieldNum] = new JLabel(par.label,
                                        JLabel.TRAILING);
          labels[fieldNum].setLabelFor(par.getComponent());
          panel.add(labels[fieldNum]);
          // panel.add(fields[fieldNum]);
          panel.add(par.getComponent());
          fieldNum++;
        }
      }

      SpringUtilities.makeCompactGrid(panel,
                                      params.size(), 2,
                                      GAP, GAP, //init x,y
                                      GAP, GAP / 2); //xpad, ypad

      Object[] options = {
          "OK", "CANCEL"};

      int result = JOptionPane.showOptionDialog(this,
                                                panel,
                                                "Switch Channels Group",
                                                JOptionPane.DEFAULT_OPTION, // option type
                                                JOptionPane.INFORMATION_MESSAGE, // message type
                                                null, // optional icon, use null to use the default icon
                                                options,
                                                options[0]
          );
      if (result == 0) {
        String cmdname = "";
        String cmdval = "";
        for (HVParameter par : params) {

          // System.out.println(par.getName() + ": " + par.getValue());
          String param = par.getName();
          String value = par.getValue().toString();

          try {
            if (param.startsWith("Group")) {
              Integer ival = Integer.parseInt(par.getValue().toString());
              cmdval = ival.toString();
            }
          }
          catch (NumberFormatException nfe) {
            JOptionPane.showMessageDialog(this,
                                          "Invalid Number Exception",
                                          nfe.toString(),
                                          JOptionPane.ERROR_MESSAGE);
            cmdval = "";

          }

          if (param.startsWith("State")) {
            cmdname = ("ON".equals(par.getValue().toString())) ?
                "SET_GROUP_SWITCH_ON=" : "SET_GROUP_SWITCH_OFF=";
          }
        }
        String[] cmds_list = DimBrowserNew.getServices("*WIENER_PL512*SYSCMD");
        String dim_cmd = "";
        if (cmds_list.length > 0) {
          dim_cmd = cmds_list[0];
        }
        if (cmdname.length()> 0 && cmdval.length() > 0) {
          System.out.println("Send " + cmdname + cmdval);
          DimClient.sendCommandNB(dim_cmd, cmdname + cmdval);
        }
      }
      isEditingValue = false;

    }

    private void SetAllChannelsState() {
      String[] cmds_list = DimBrowserNew.getServices("*WIENER_PL512*SYSCMD");
      String dim_cmd = "";
      if (cmds_list.length > 0) {
        dim_cmd = cmds_list[0];
      }

      String[] states = {
          "OFF", "ON"};

      String value = (String) JOptionPane.showInputDialog(
          this,
          "Set All Channels Switch State:",
          "Set All Channels Switch State",
          JOptionPane.QUESTION_MESSAGE,
          null,
          states,
          states[0]);
      if (value == null) {
        return;
      }
      if ("ON".equals(value)) {
        DimClient.sendCommandNB(dim_cmd, "SET_ALL_CHANS_ON");
      }
      if ("OFF".equals(value)) {
        DimClient.sendCommandNB(dim_cmd, "SET_ALL_CHANS_OFF");
      }

    }

    private void SetParameter(String param) {

      isEditingValue = true;
      JTable table = jt;

      String cmdstr = "";

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
        if ("ON".equals(value)) {
          cmdstr = "SET_CHAN_ON";
        }
        if ("OFF".equals(value)) {
          cmdstr = "SET_CHAN_OFF";
        }
      }

      try {
        if (param.startsWith("V output")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 8.0);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Float fval = Float.parseFloat(value.toString());
            if (fval > 32) {
              fval = new Float(32.0);
            }
            cmdstr = "SET_CHAN_OUTPUT_VOLT=" + fval.toString();
          }
        }

        if (param.startsWith("I limit")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 20.0);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Float fval = Float.parseFloat(value.toString());
            if (fval > 32) {
              fval = new Float(32.0);
            }
            cmdstr = "SET_CHAN_CURRENT_LIMIT=" + fval.toString();
          }
        }

        if (param.startsWith("V min sense")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 8.0);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Float fval = Float.parseFloat(value.toString());
            if (fval > 32) {
              fval = new Float(32.0);
            }
            cmdstr = "SET_CHAN_MIN_SENSE_VOLT=" + fval.toString();
          }
        }

        if (param.startsWith("V max sense")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 8.0);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Float fval = Float.parseFloat(value.toString());
            if (fval > 32) {
              fval = new Float(32.0);
            }
            cmdstr = "SET_CHAN_MAX_SENSE_VOLT=" + fval.toString();
          }
        }

        if (param.startsWith("V max terminal")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 8.0);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Float fval = Float.parseFloat(value.toString());
            if (fval > 32) {
              fval = new Float(32.0);
            }
            cmdstr = "SET_CHAN_MAX_TERMINAL_VOLT=" + fval.toString();
          }
        }

        if (param.startsWith("Imax")) {
         String value = JOptionPane.showInputDialog("Enter " + param +
             " value:", 20.0);
         if (value == null) {
           return;
         }
         if (value != null && value.length() > 0) {
           Float fval = Float.parseFloat(value.toString());
           if (fval > 32) {
             fval = new Float(32.0);
           }
           cmdstr = "SET_CHAN_MAX_CURRENT=" + fval.toString();
         }
       }

       if (param.startsWith("Max Power")) {
         String value = JOptionPane.showInputDialog("Enter " + param +
             " value:", 300.0);
         if (value == null) {
           return;
         }
         if (value != null && value.length() > 0) {
           Float fval = Float.parseFloat(value.toString());
           if (fval > 300) {
             fval = new Float(300.0);
           }
           fval /= 100;
           cmdstr = "SET_CHAN_MAX_POWER=" + fval.toString();
         }
       }


        if (param.startsWith("Group")) {
          String value = JOptionPane.showInputDialog("Enter " + param +
              " value:", 1);
          if (value == null) {
            return;
          }
          if (value != null && value.length() > 0) {
            Integer ival = Integer.parseInt(value.toString());
            if (ival <= 0) {
              ival = 1;
            }
            cmdstr = "SET_CHAN_GROUP=" + ival.toString();
          }
        }

        if (table.getSelectedRowCount() > 0) {
          int rowIndexStart = table.getSelectedRow();
          int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();
          for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
            if (table.isCellSelected(r, 0)) {

              String dim_cmd = "";
              if (cmd_list.length > 0) {
                dim_cmd = cmd_list[r];
              }
              // System.out.println(dim_cmd);
              DimClient.sendCommandNB(dim_cmd, cmdstr);
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

      }

      isEditingValue = false;

    }

    public void this_internalFrameClosing(InternalFrameEvent e) {
      for (DimInfo entry : dim_svcs) {
        entry.releaseService();
      }
      dim_svcs.clear();
    }
    public void lbl_sysMainSwitch_mouseClicked(MouseEvent e) {
      if (e.getClickCount() == 2) {
        String[] cmds_list = DimBrowserNew.getServices("*WIENER_PL512*SYSCMD");
        String dim_cmd = "";
        if (cmds_list.length > 0) {
          dim_cmd = cmds_list[0];
        }

        String[] states = {
            "OFF", "ON"};

        String value = (String) JOptionPane.showInputDialog(
            this,
            "Set Main Switch State:",
            "Set Main Switch State",
            JOptionPane.QUESTION_MESSAGE,
            null,
            states,
            states[0]);
        if (value == null) {
          return;
        }
        if ("ON".equals(value)) {
          DimClient.sendCommandNB(dim_cmd, "SET_MAIN_ON");
        }
        if ("OFF".equals(value)) {
          DimClient.sendCommandNB(dim_cmd, "SET_MAIN_OFF");
        }

      }
    }

    class WienerPL512Frame_this_internalFrameAdapter
        extends InternalFrameAdapter {
      private WienerPL512Frame adaptee;
      WienerPL512Frame_this_internalFrameAdapter(WienerPL512Frame adaptee) {
        this.adaptee = adaptee;
      }

      public void internalFrameClosing(InternalFrameEvent e) {
        adaptee.this_internalFrameClosing(e);
      }
    }

  }

  class WienerPL512Frame_lbl_sysMainSwitch_mouseAdapter
      extends MouseAdapter {
    private WienerPL512Frame adaptee;
    WienerPL512Frame_lbl_sysMainSwitch_mouseAdapter(WienerPL512Frame adaptee) {
      this.adaptee = adaptee;
    }

    public void mouseClicked(MouseEvent e) {
      adaptee.lbl_sysMainSwitch_mouseClicked(e);
    }
  }
