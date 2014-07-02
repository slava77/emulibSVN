package hvclient;

import java.util.Calendar;
import java.util.TimeZone;
import javax.swing.*;
import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;
import java.awt.Point;
import javax.swing.table.TableColumnModel;
import javax.swing.table.TableColumn;
import java.awt.Color;
import javax.swing.table.JTableHeader;
import javax.swing.event.ChangeEvent;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.Vector;
import java.util.Arrays;
import java.util.Map;
import java.util.HashMap;

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
public class HVCSCStatusFrame
    extends JInternalFrame {

  final static int GAP = 10;
  static final Integer MODULELAYER = new Integer(5);
  protected String[] set_params_list = {
      "State",
      "Vset (V)", "Ramp Up (V)", "Ramp Down (V)", "Imax (mkA)", "Ramp Imax (mkA)",
      "OV Limit(V)", "OV Trip Delay (ms)",
      "Trip Delay (ms)",
      "Trip Reset Delay (ms)", "Trip Max Resets", "Max Dead Counter", "Reset Trips"};

  protected String[] thresholds_list = {
      "Vmon min (V)", "Vmon max (V)", "Imon min (mkA)", "Imon max (mkA)",
      "Imon total (mkA)"};

  protected String[] headers = {
      "CSC Name", "RDB", "RDB Part",
      "#Chans", "#Chans On",
      "Vset", "Vmon min", "Vmon max",
      "Imon total", "Imon min", "Imon max", "State", "Status"};
  protected int HostID = 500;

  protected DimRpcInfo rpcStatus = null;
  protected DimRpcInfo rpcCSC = null;

  private Timer timer;
  private int delay = 5000;

  protected boolean isEditingValue = false;

  HVDataTableModel dtm = new HVDataTableModel(headers, false);

  JPanel jPanel1 = new JPanel();
  JPanel DataPanel = new JPanel();
  JTable jt; //  = new JTable(dtm);
  JScrollPane jsp = new JScrollPane();
  JPopupMenu menu = new JPopupMenu();

  JTabbedPane jTabbedPane1 = new JTabbedPane();
  /*
  JTabbedPane PlotsPanel = new JTabbedPane();
  HVTimeChart VmonPlotsPanel = new HVTimeChart(0, "", "Voltage max, V", 300);
  HVTimeChart ImonPlotsPanel = new HVTimeChart(0, "", "Current total, mkA", 300);
*/
  JLabel statusBar = new JLabel();
  BorderLayout borderLayout1 = new BorderLayout();
  Calendar cal;
  java.text.SimpleDateFormat sdf;
  Vector<HVParameter> HVParams;

  public HVCSCStatusFrame() {
    super("", true, true, true, true);
    create();
  }

  public HVCSCStatusFrame(int hostid) {
    super("", true, true, true, true);
    HostID = hostid;
    create();
  }

  private void create() {

//    openFrameCount++;
    try {

      rpcCSC = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID +
                              "/HV_CSC_CMD");
      rpcStatus = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID +
                                 "/HV_CSC_STATUS");
//      rpcDB.setDebug(true);
      rpcStatus.setServiceIDMode(false);
      // HVParams = createParametersList();
      jbInit();
      String DATE_FORMAT = "HH:mm:ss";
      sdf = new java.text.SimpleDateFormat(DATE_FORMAT);

      ActionListener taskPerformer = new ActionListener() {
        public void actionPerformed(ActionEvent evt) {
          cal = Calendar.getInstance(TimeZone.getDefault());
          UpdateData();
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

    void UpdateData() {

      rpcStatus.setData("STATUS");
      String res = rpcStatus.getString();
      // System.out.println(res);
      int rows_count = 0;

      // Check recieved data length
      if (res.length() == 0) {
        statusBar.setText(
            sdf.format(cal.getTime()) + "> Status: Not Updated. Zero-length data received");
        jt.setVisible(false);
        return;
      } else jt.setVisible(true);

      // Parse recieved data for rows
      String[] rows = res.split(";\n");
      if (rows.length == 0) {
        statusBar.setText(
            sdf.format(cal.getTime())+ "> Status: Not Updated. Unable to parse data for rows");
        jt.setVisible(false);
        return;
      } else jt.setVisible(true);

      // Parse data received data for columns
      Object[][] data = new Object[rows.length][headers.length];
      for (int i = 0; i < rows.length; i++) {
        String[] cols = rows[i].split(",");
        if (cols.length == headers.length) {
          data[i] = cols;
          rows_count++;
        }
      }

      // Check number of valid data rows
      if (rows_count == 0) {
        statusBar.setText(
            sdf.format(cal.getTime())+ "> Status: Not Updated. Unable to parse data for columns");
        jt.setVisible(false);
        return;
      }
      else {
        jt.setVisible(true);
      }

      // Check if table data is not edited or multiple rows selected
      if (jt.isEditing() || (jt.getSelectedRowCount() > 1) ||
          isEditingValue || menu.isVisible()) {
        statusBar.setText(
            sdf.format(cal.getTime())+ "> Status: Not Updated. Cell Editing or Selection Detected");
        return;
      }

      // Update Table data
      dtm.setData(data);

      /*
      // Plots Vmon values
      int vseries = VmonPlotsPanel.getNumSeries();
      if (vseries != data.length) {
        VmonPlotsPanel.setNumSeries(data.length);
        vseries = data.length;
        for (int i = 0; i < vseries; i++) {
          VmonPlotsPanel.setSerieTitle(i, data[i][0].toString());
        }

      }
      for (int i = 0; i < vseries; i++) {
        Object value = data[i][dtm.getColumnPos("Vmon max")];
        try {
          Integer val = Integer.parseInt(value.toString());
          // Float val = Float.parseFloat(value.toString());
          VmonPlotsPanel.addValue(i, val);

        }
        catch (NumberFormatException nfe) {
          System.out.println("Unable to parse Vmon value. ");
        }
      }

      // Plots Imon Values
      int iseries = ImonPlotsPanel.getNumSeries();
      if (iseries != data.length) {
        ImonPlotsPanel.setNumSeries(data.length);
        iseries = data.length;
        for (int i = 0; i < vseries; i++) {
          ImonPlotsPanel.setSerieTitle(i, data[i][0].toString());
        }

      }
      for (int i = 0; i < iseries; i++) {
        Object value = data[i][dtm.getColumnPos("Imon total")];
        try {
          Float val = Float.parseFloat(value.toString());
          ImonPlotsPanel.addValue(i, val);
        }
        catch (NumberFormatException nfe) {
          System.out.println("Unable to parse Imon value. ");
        }
      }
*/
      statusBar.setText(sdf.format(cal.getTime())+"> Status: Data Updated");
      // jt.grabFocus();
    }

    private void jbInit() throws Exception {
      setContentPane(jPanel1);

      // dtm.setColumnCount(headers.length);
      dtm.setColumnIdentifiers(headers);
      this.addInternalFrameListener(new
                                    HVCSCStatusFrame_this_internalFrameAdapter(this));
      pack();
      setSize(800, 800);

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

            int currentRow = getSelectedRow();
            int currentCol = getSelectedColumn();

            Object value = getValueAt(currentRow, currentCol);
            HVcsccmd cmd = new HVcsccmd();
            cmd.csc = getModel().getValueAt(currentRow, 0).toString();
            cmd.cmd = HVData.HV_CMD.HV_GET_MOD_DATA.ordinal();
            cmd.chan = HVData.ALL;
            cmd.data = new Long(0);
            cmd.size = new Long(0);

            if (getColumnName(currentCol).startsWith("State")) {
              cmd.data = new Long(("ON".equals(value)) ? 1 : 0);
              cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
              rpcCSC.setData(cmd.encode());
              cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
            }

            if (getColumnName(currentCol).startsWith("Vset")) {
              cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
              try {
                cmd.data = new Long(Integer.parseInt(value.toString()));
              }
              catch (NumberFormatException nfe) {
                JOptionPane.showMessageDialog(this,
                                              "Exception",
                                              nfe.toString(),
                                              JOptionPane.ERROR_MESSAGE);
                cmd.data = new Long(0);
                cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
              }

            }

            rpcCSC.setData(cmd.encode());
          }
          catch (Exception exception) {
            exception.printStackTrace();
          }
        } //editingStopped

      };

      dtm.setEditableColumn("Vset", true);
      dtm.setEditableColumn("State", true);

      jsp.getViewport().add(jt);
      jt.setSelectionForeground(Color.RED);
      jt.setRowSelectionAllowed(true);
      // jt.setColumnSelectionAllowed(true);
      jt.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
      jt.setSelectionForeground(Color.BLUE);
      jt.setModel(dtm);
      jt.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
      // jt.setDefaultRenderer(String.class, new HVTableObjectRenderer());
      jt.setDefaultRenderer(String.class, new HVTableObjectRenderer());
      jt.setDefaultRenderer(Integer.class, new HVTableObjectRenderer());
      jt.setDefaultRenderer(Float.class, new HVTableFloatRenderer());

      KeyStroke inc_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_PERIOD, java.awt.event.InputEvent.CTRL_MASK);
      KeyStroke dec_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_COMMA, java.awt.event.InputEvent.CTRL_MASK);
      String inc_font_str = "inc_font";
      String dec_font_str = "dec_font";

      jt.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          inc_font_size, inc_font_str);
      jt.getActionMap().put(inc_font_str,
                            new TableFontResizeAction(inc_font_str));
      jt.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          dec_font_size, dec_font_str);
      jt.getActionMap().put(dec_font_str,
                            new TableFontResizeAction(dec_font_str));

      TableColumnModel tcm = jt.getColumnModel();
      TableColumn tc = tcm.getColumn(dtm.getColumnPos("State"));
      tc.setCellRenderer(new HVTableRenderer());
      tc.setCellEditor(new HVTableEditor());
      tc = tcm.getColumn(dtm.getColumnPos("Status"));
      tc.setCellRenderer(new HVTableRenderer());
      tc = tcm.getColumn(dtm.getColumnPos("Vmon min"));
      tc.setCellRenderer(new HVVoltageCheckCellRenderer(dtm.getColumnPos("Vset"),
          HVClient.config.csc_mon_thresholds.vmon_min, dtm.getColumnPos("State")));
      tc = tcm.getColumn(dtm.getColumnPos("Vmon max"));
      tc.setCellRenderer(new HVVoltageCheckCellRenderer(dtm.getColumnPos("Vset"),
          HVClient.config.csc_mon_thresholds.vmon_max, dtm.getColumnPos("State")));
      tc = tcm.getColumn(dtm.getColumnPos("#Chans On"));
      tc.setCellRenderer(new HVVoltageCheckCellRenderer(dtm.getColumnPos(
          "#Chans"), 0, dtm.getColumnPos("State")));
      tc = tcm.getColumn(dtm.getColumnPos("Imon min"));
      tc.setCellRenderer(new HVCurrentCheckCellRenderer( -1,
          HVClient.config.csc_mon_thresholds.imon_min));
      tc = tcm.getColumn(dtm.getColumnPos("Imon max"));
      tc.setCellRenderer(new HVCurrentCheckCellRenderer( -1,
          HVClient.config.csc_mon_thresholds.imon_max));
      tc = tcm.getColumn(dtm.getColumnPos("Imon total"));
      tc.setCellRenderer(new HVCurrentCheckCellRenderer( -1,
          HVClient.config.csc_mon_thresholds.imon_total));

      // Set the component to show the popup menu
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

      jt.addMouseListener(new MouseAdapter() {
        public void mouseClicked(MouseEvent e) {
          if (e.getClickCount() == 2) {
            Point p = e.getPoint();
            int row = jt.rowAtPoint(p);
            int column = jt.columnAtPoint(p); // This is the view column!
            // System.out.println("Clicked at " + row + ":" + column);

            if (jt.getColumnName(column) == "CSC Name") {
              OpenCSCFrame(HostID, jt.getValueAt(row, column).toString());
            }
            // ...
          }
        }
      });

      jPanel1.setLayout(borderLayout1);

      jPanel1.add(jTabbedPane1, java.awt.BorderLayout.CENTER);
      jPanel1.add(statusBar, java.awt.BorderLayout.SOUTH);

      jTabbedPane1.add(DataPanel, "Data");

      DataPanel.setLayout(new BorderLayout());
      DataPanel.add(jsp, java.awt.BorderLayout.CENTER);

      /*
      jTabbedPane1.add(PlotsPanel, "Plots");
      PlotsPanel.add(VmonPlotsPanel, "Voltage");
      PlotsPanel.add(ImonPlotsPanel, "Current");
*/
      // jTabbedPane1.setSelectedComponent(DataPanel);
      jt.grabFocus();
      setTitle("CSC Status for HID" + HostID);
    }

    private JPopupMenu CreatePopupMenu() {
      JPopupMenu menu = new JPopupMenu();
      JMenu set_parameters_menu = new JMenu(
          "Set single parameter for selected CSCs");
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
      menu.addSeparator();

      JMenuItem mparams = new JMenuItem(
          "Set multiple parameters for selected CSCs");
      mparams.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (e.getSource() instanceof JMenuItem) {
            SetMultipleParameters();
            // SetParameter( ( (JMenuItem) (e.getSource())).getText());
          }
        }
      });

      menu.add(mparams);
      menu.addSeparator();
      JMenu set_thresholds_menu = new JMenu("Set Visual Alarm Thresholds");
      menu.add(set_thresholds_menu);
      for (int i = 0; i < thresholds_list.length; i++) {
        JMenuItem param = new JMenuItem(thresholds_list[i]);
        param.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            if (e.getSource() instanceof JMenuItem) {
              SetThreshold( ( (JMenuItem) (e.getSource())).getText());
            }
          }
        });
        set_thresholds_menu.add(param);
      }
      return menu;
    }

    private Vector<HVParameter> createParametersList() {
      Vector<HVParameter> params = new Vector<HVParameter> ();

      String[] rState = {
          "", "OFF", "ON"};
      params.add(new HVParameter("State", "State:", JComboBox.class, rState[0],
                                 new Vector<Object> (Arrays.asList(rState))));
      Integer[] rVset = {
          0, 4000};
      params.add(new HVParameter("Vset", "Vset (V):", JTextField.class, -1,
                                 new Vector<Object> (Arrays.asList(rVset))));
      Integer[] rRampUp = {
          5, 100};
      params.add(new HVParameter("Ramp Up", "Ramp Up (V):", JTextField.class,
                                 0, new Vector<Object> (Arrays.asList(rRampUp))));
      Integer[] rRampDown = {
          5, 4000};
      params.add(new HVParameter("Ramp Down", "Ramp Down (V):", JTextField.class,
                                 0,
                                 new Vector<Object> (Arrays.asList(rRampDown))));
      Integer[] rImax = {
          1, 1500};
      params.add(new HVParameter("Imax", "Imax (mkA)", JTextField.class, 0,
                                 new Vector<Object> (Arrays.asList(rImax))));

      Integer[] rRampImax = {
          1, 1500};
      params.add(new HVParameter("Ramp Imax", "Ramp Imax (mkA)", JTextField.class, 0,
                                 new Vector<Object> (Arrays.asList(rRampImax))));


      Integer[] ovLimit = {
          0, 1000};
      params.add(new HVParameter("OV Limit", "OV Limit (V):", JTextField.class,
                                 0,
                                 new Vector<Object> (Arrays.asList(ovLimit))));


      Integer[] ovTripDelay = {
           20, 10000};
       params.add(new HVParameter("OV Trip Delay", "OV Trip Delay (ms):", JTextField.class,
                                  0,
                                  new Vector<Object> (Arrays.asList(ovTripDelay))));


      Integer[] rTripDelay = {
          20, 10000};
      params.add(new HVParameter("Trip Delay", "Trip Delay (ms):", JTextField.class,
                                 0,
                                 new Vector<Object> (Arrays.asList(rTripDelay))));
      Integer[] rTripResetDelay = {
          10000, 600000};
      params.add(new HVParameter("Trip Reset Delay", "Trip Reset Delay (ms):",
                                 JTextField.class, 0,
                                 new Vector<Object> (
                                     Arrays.asList(rTripResetDelay))));
      Integer[] rTripMaxResets = {
          0, 20};
      params.add(new HVParameter("Trip Max Resets", "Trip Max Resets:",
                                 JTextField.class, -1,
                                 new Vector<Object> (
                                     Arrays.asList(rTripMaxResets))));

      Integer[] rDeadMaxCnt = {
          0, 10000};
      params.add(new HVParameter("Max Dead Counter", "Max Dead Counter:",
                                 JTextField.class, 0,
                                 new Vector<Object> (
                                     Arrays.asList(rDeadMaxCnt))));



      String[] resState = {
          "", "RESET"};
      params.add(new HVParameter("Reset Trips", "Reset Trip:", JComboBox.class, resState[0],
                                 new Vector<Object> (Arrays.asList(resState))));

      /*params.add(new HVParameter("Reset Trips", "Resets Trip:", JCheckBox.class,
                                 new Boolean(false)));
       */
      return params;
    }

    private void SetMultipleParameters() {
      isEditingValue = true;
      JTable table = jt;
      HVcsccmd cmd = new HVcsccmd();
      // cmd.csc = jt.getModel().getValueAt(currentRow, 0).toString();
      cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
      cmd.chan = HVData.ALL;
      cmd.data = new Long(0);
      cmd.size = new Long(0);

      Object[] options = {
          "OK", "CANCEL"};

      HVParams = createParametersList();
      JComponent paramsPanel = createEntryFields(HVParams);
      int result = JOptionPane.showOptionDialog(this,
                                                paramsPanel,
                                                "Set Parameters",
                                                JOptionPane.DEFAULT_OPTION, // option type
                                                JOptionPane.INFORMATION_MESSAGE, // message type
                                                null, // optional icon, use null to use the default icon
                                                options,
                                                options[0]
          );
      if (result == 0) {
        if (table.getSelectedRowCount() > 0) {
          int rowIndexStart = table.getSelectedRow();
          int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();

          // System.out.println("OK pressed");
          for (HVParameter par : HVParams) {
           if (par.isModified()) {
              System.out.println(par.getName() + ": " + par.getValue());
              // par.updateValue();
              String param = par.getName();
              String value = par.getValue().toString();

              if (param.startsWith("State")) {
                cmd.data = new Long(("ON".equals(par.getValue().toString())) ? 1 : 0);
                cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
              }

              if (param.startsWith("Reset Trips") && "RESET".equals(par.getValue().toString())) {
                cmd.cmd = HVData.HV_CMD.HV_RESET_TRIP.ordinal();
              }

              try {
                if (param.startsWith("Vset")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 4000) {
                      cmd.data = new Long(4000);
                    }
                  }
                }

                if (param.startsWith("Ramp Up")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_UP.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 4000) {
                      cmd.data = new Long(4000);
                    }
                  }
                }

                if (param.startsWith("Ramp Down")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_DOWN.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 4000) {
                      cmd.data = new Long(4000);
                    }
                  }
                }

                if (param.startsWith("Imax")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_IMAX.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 200) {
                      cmd.data = new Long(200);
                    }
                  }
                }

                if (param.startsWith("Ramp Imax")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_IMAX_RAMP.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 200) {
                      cmd.data = new Long(200);
                    }
                  }
                }

                if (param.startsWith("OV Limit")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_OV_LIMIT.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data < 0) {
                      cmd.data = new Long(0);
                    }
                  }
                }

                if (param.startsWith("OV Trip Delay")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_OV_TRIP_DELAY.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data < 20) {
                      cmd.data = new Long(20);
                    }
                  }
                }


                if (param.startsWith("Trip Delay")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_DELAY.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data < 20) {
                      cmd.data = new Long(20);
                    }
                  }
                }

                if (param.startsWith("Trip Reset Delay")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_RESET_DELAY.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data < 2000) {
                      cmd.data = new Long(2000);
                    }
                  }
                }

                if (param.startsWith("Trip Max Resets")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_TRIP_MAX_RESETS.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data > 20) {
                      cmd.data = new Long(20);
                    }
                  }
                }

                if (param.startsWith("Max Dead Counter")) {
                  if (value != null && value.length() > 0) {
                    cmd.cmd = HVData.HV_CMD.HV_SET_DEAD_MAX_CNT.ordinal();
                    cmd.data = new Long(Integer.parseInt(value.toString()));
                    if (cmd.data < 0) {
                      cmd.data = new Long(0);
                    }
                  }
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
              String csclist ="";
              for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
                if (table.isCellSelected(r, 0)) {
                  // Get CSC Name
                  String CSC = (String) table.getModel().getValueAt(r, 0);
                  if (CSC.startsWith("ME")) {
                    csclist+= "CSC:"+CSC+";";
                  }

                }
              }
              String cmdstr = csclist
                  +"CHAN:"+ Integer.toString(cmd.chan)+";"
                  +"CMD:"+Integer.toString(cmd.cmd)+";"
                  +"DATA:"+Long.toString(cmd.data)+";"
                  +"SIZE:"+Long.toString(cmd.size)+";";
              rpcCSC.setData(cmdstr);
              System.out.println(cmdstr);
              if (param.startsWith("State")) {
                cmdstr = csclist
                  +"CHAN:"+ Integer.toString(cmd.chan) + ";"
                  +"CMD:"+Integer.toString(HVData.HV_CMD.HV_SET_STATE.ordinal()) +";"
                  +"DATA:"+Long.toString(cmd.data) +";"
                  +"SIZE:"+Long.toString(cmd.size)+";";
                rpcCSC.setData(cmdstr);
                System.out.println(cmdstr);
              }

              try {
                Thread.sleep(500);
              }
              catch
                  (InterruptedException e) {}

              /*
              for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
                if (table.isCellSelected(r, 0)) {
                  // Get CSC Name
                  String CSC = (String) table.getModel().getValueAt(r, 0);
                  if (CSC.startsWith("ME")) {
                    cmd.csc = CSC;
                    rpcCSC.setData(cmd.encode());
                    System.out.println(cmd.toString());
                    if (par.getName().startsWith("State")) {
                      HVcsccmd cmd2 = new HVcsccmd();
                      cmd2.csc = cmd.csc;
                      cmd2.chan = cmd.chan;
                      cmd2.size = cmd.size;
                      cmd2.data = cmd.data;
                      cmd2.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
                      rpcCSC.setData(cmd2.encode());
                      System.out.println(cmd2.toString());
                    }

                  }
                  System.out.println(cmd.toString());
                  // cell is selected
                }
              }
              */
            }
          }
          table.setRowSelectionInterval(rowIndexStart, rowIndexStart);
          isEditingValue = false;
        }

      }

    }
    protected JComponent createEntryFields(Vector<HVParameter> params) {
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

      return panel;
    }

    private void SetThreshold(String param) {
      isEditingValue = true;
      JTable table = jt;

      try {
        if (param.startsWith("Vmon min")) {
          String value = JOptionPane.showInputDialog(
              "Enter threshold value for "
              + param, HVClient.config.csc_mon_thresholds.vmon_min);
          if (value != null && value.length() > 0) {
            Integer thresh = Integer.parseInt(value.toString());
            if (table.getColumnModel().getColumn(dtm.getColumnPos("Vmon min")).
                getCellRenderer() instanceof HVVoltageCheckCellRenderer) {
              ( (HVVoltageCheckCellRenderer) (table.getColumnModel().getColumn(
                  dtm.getColumnPos("Vmon min")).
                                              getCellRenderer())).setThreshold(
                                                  thresh);
              HVClient.config.csc_mon_thresholds.vmon_min = thresh;
            }
          }
        }
        if (param.startsWith("Vmon max")) {
          String value = JOptionPane.showInputDialog(
              "Enter threshold value for "
              + param, HVClient.config.csc_mon_thresholds.vmon_max);
          if (value != null && value.length() > 0) {
            Integer thresh = Integer.parseInt(value.toString());
            if (table.getColumnModel().getColumn(dtm.getColumnPos("Vmon max")).
                getCellRenderer() instanceof HVVoltageCheckCellRenderer) {
              ( (HVVoltageCheckCellRenderer) (table.getColumnModel().getColumn(
                  dtm.getColumnPos("Vmon max")).
                                              getCellRenderer())).setThreshold(
                                                  thresh);
              HVClient.config.csc_mon_thresholds.vmon_max = thresh;
            }
          }
        }

        if (param.startsWith("Imon min")) {
          String value = JOptionPane.showInputDialog(
              "Enter threshold value for "
              + param, HVClient.config.csc_mon_thresholds.imon_min);
          if (value != null && value.length() > 0) {
            Double thresh = Double.parseDouble(value.toString());
            if (table.getColumnModel().getColumn(dtm.getColumnPos("Imon min")).
                getCellRenderer() instanceof HVCurrentCheckCellRenderer) {
              ( (HVCurrentCheckCellRenderer) (table.getColumnModel().getColumn(
                  dtm.getColumnPos("Imon min")).
                                              getCellRenderer())).setThreshold(
                                                  thresh);
              HVClient.config.csc_mon_thresholds.imon_min = thresh;
            }
          }
        }

        if (param.startsWith("Imon max")) {
          String value = JOptionPane.showInputDialog(
              "Enter threshold value for "
              + param, HVClient.config.csc_mon_thresholds.imon_max);
          if (value != null && value.length() > 0) {
            Double thresh = Double.parseDouble(value.toString());
            if (table.getColumnModel().getColumn(dtm.getColumnPos("Imon max")).
                getCellRenderer() instanceof HVCurrentCheckCellRenderer) {
              ( (HVCurrentCheckCellRenderer) (table.getColumnModel().getColumn(
                  dtm.getColumnPos("Imon max")).
                                              getCellRenderer())).setThreshold(
                                                  thresh);
              HVClient.config.csc_mon_thresholds.imon_max = thresh;
            }
          }
        }

        if (param.startsWith("Imon total")) {
          String value = JOptionPane.showInputDialog(
              "Enter threshold value for "
              + param, HVClient.config.csc_mon_thresholds.imon_total);
          if (value != null && value.length() > 0) {
            Double thresh = Double.parseDouble(value.toString());
            if (table.getColumnModel().getColumn(dtm.getColumnPos("Imon total")).
                getCellRenderer() instanceof HVCurrentCheckCellRenderer) {
              ( (HVCurrentCheckCellRenderer) (table.getColumnModel().getColumn(
                  dtm.getColumnPos("Imon total")).
                                              getCellRenderer())).setThreshold(
                                                  thresh);
              HVClient.config.csc_mon_thresholds.imon_total = thresh;
            }
          }
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
    ;

    private void SetParameter(String param) {

      isEditingValue = true;
      JTable table = jt;
      HVcsccmd cmd = new HVcsccmd();
      // cmd.csc = jt.getModel().getValueAt(currentRow, 0).toString();
      cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
      cmd.chan = HVData.ALL;
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
        cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
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

        if (param.startsWith("Ramp Imax")) {
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


      }
      catch (NumberFormatException nfe) {
        JOptionPane.showMessageDialog(this,
                                      "Exception",
                                      nfe.toString(),
                                      JOptionPane.ERROR_MESSAGE);
        cmd.data = new Long(0);
        cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();
      }
/*
      String csclist ="";
      if (table.getSelectedRowCount() > 0) {
        int rowIndexStart = table.getSelectedRow();
        int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();

        for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
          if (table.isCellSelected(r, 0)) {
            // Get CSC Name
            String CSC = (String) table.getModel().getValueAt(r, 0);
            if (CSC.startsWith("ME")) {
              csclist += "CSC:" + CSC + ";";
            }

          }
        }
        String cmdstr = csclist
            + "CHAN:" + Integer.toString(cmd.chan) + ";"
            + "CMD:" + Integer.toString(cmd.cmd) + ";"
            + "DATA:" + Integer.toString(cmd.data) + ";"
            + "SIZE:" + Integer.toString(cmd.size) + ";";
        rpcCSC.setData(cmdstr);
        System.out.println(cmdstr);
        if (param.startsWith("State")) {
          cmdstr = csclist
              + "CHAN:" + Integer.toString(cmd.chan) + ";"
              + "CMD:" + Integer.toString(HVData.HV_CMD.HV_SET_STATE.ordinal()) +
              ";"
              + "DATA:" + Integer.toString(cmd.data) + ";"
              + "SIZE:" + Integer.toString(cmd.size) + ";";
          rpcCSC.setData(cmdstr);
          System.out.println(cmdstr);
        }
        table.setRowSelectionInterval(rowIndexStart, rowIndexStart);
      }
*/

      if (table.getSelectedRowCount() > 0) {
        int rowIndexStart = table.getSelectedRow();
        int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();
        for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
          if (table.isCellSelected(r, 0)) {
            // Get CSC Name
            String CSC = (String) table.getModel().getValueAt(r, 0);
            if (CSC.startsWith("ME")) {
              cmd.csc = CSC;
              rpcCSC.setData(cmd.encode());
              System.out.println(cmd.toString());
              if (param.startsWith("State")) {
                HVcsccmd cmd2 = new HVcsccmd();
                cmd2.csc = cmd.csc;
                cmd2.chan = cmd.chan;
                cmd2.size = cmd.size;
                cmd2.data = cmd.data;
                cmd2.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
                rpcCSC.setData(cmd2.encode());
                System.out.println(cmd2.toString());
              }

            }
            // System.out.println(cmd.toString());
            // cell is selected
          }
        }
        table.setRowSelectionInterval(rowIndexStart, rowIndexStart);
      }

      isEditingValue = false;

    }

    private void OpenCSCFrame(int hostid, String csc) {
      String title = new Integer(hostid).toString() + ":" + csc;

      for (JInternalFrame frame : getDesktopPane().getAllFrames()) {
        if (frame.getTitle().startsWith(title)) {
          // System.out.println(title + " " +frame.getTitle());
          try {
            frame.setVisible(true);
            frame.setIcon(false);
            frame.setSelected(true);
          }
          catch (java.beans.PropertyVetoException e2) {}
          return ;
        }
      }

      HVCSCFrame doc = new HVCSCFrame(hostid, csc);

      getDesktopPane().add(doc, MODULELAYER);
      try {
        doc.Place();
        doc.setVisible(true);
        doc.setSelected(true);
      }
      catch (java.beans.PropertyVetoException e2) {}

    }

    public void this_internalFrameClosing(InternalFrameEvent e) {
      timer.stop();
      rpcStatus.finalize();
    }
    class HVCSCStatusFrame_this_internalFrameAdapter
        extends InternalFrameAdapter {
      private HVCSCStatusFrame adaptee;
      HVCSCStatusFrame_this_internalFrameAdapter(HVCSCStatusFrame adaptee) {
        this.adaptee = adaptee;
      }

      public void internalFrameClosing(InternalFrameEvent e) {
        adaptee.this_internalFrameClosing(e);
      }
    }

  }
