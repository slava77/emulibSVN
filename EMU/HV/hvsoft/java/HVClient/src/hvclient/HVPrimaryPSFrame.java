package hvclient;

import java.util.Calendar;
import java.util.TimeZone;
import javax.swing.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import javax.swing.Timer;
import java.util.*;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import javax.swing.table.DefaultTableModel;
import java.awt.BorderLayout;
import java.awt.Color;
import javax.swing.table.TableColumnModel;
import javax.swing.table.TableColumn;
import javax.swing.table.JTableHeader;
import javax.swing.event.ChangeEvent;
import java.awt.event.MouseAdapter;
import java.awt.Point;
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
public class HVPrimaryPSFrame
    extends JInternalFrame {


  protected String[] set_params_list = {
        "State",
        "Vset (V)", "Ramp Up (V)", "Ramp Down (V)", "Imax (mkA)",
        "Reset Trips"};

  protected int HostID = 500;
  protected int Port = 0;
  protected int Addr = 1;

  protected DimRpcInfo rpcPrimeData = null;
  protected DimRpcInfo rpcPrimeCmd = null;
  protected DimRpcInfo rpcSql = null;

  // protected Vector<HVcmd> primaries = new Vector<HVcmd> (1);

  class ComparatorTest implements Comparator {
   public int compare(Object o1, Object o2) {
     HVcmd s1 = (HVcmd)o1;
     HVcmd s2 = (HVcmd)o2;
     int k1 = (s1.card << 8) + (s1.module);
     int k2 = (s2.card << 8) + (s2.module);

     return k1-k2;
   }
   public boolean equals(Object o) {
     HVcmd s = (HVcmd)o;
     return compare(this, o)==0;
   }
 }

  protected TreeMap<HVcmd, HVprimary> primaries = new TreeMap<HVcmd, HVprimary>(new ComparatorTest());

  private Timer timer;
  private int delay = 2000;

  protected boolean isEditingValue = false;
  private boolean fSinglePrimary = false;

  // HVPrimaryPSDataTableModel dtm = new HVPrimaryPSDataTableModel();
  // DefaultTableModel dtm = new DefaultTableModel ();
  HVDataTableModel dtm = new HVDataTableModel();

  JPopupMenu menu = new JPopupMenu();

  JPanel jPanel1 = new JPanel();
  JPanel DataPanel = new JPanel();
  JTable jt; // = new JTable (dtm);
  JScrollPane jsp = new JScrollPane();

  JTabbedPane jTabbedPane1 = new JTabbedPane();
  JTabbedPane PlotsPanel = new JTabbedPane();
  HVTimeChart VmonPlotsPanel = new HVTimeChart(0, "PS#", "Voltage, V", 300);
  HVTimeChart ImonPlotsPanel = new HVTimeChart(0, "PS#", "Current, mkA", 300);

  BorderLayout borderLayout1 = new BorderLayout();
  JLabel statusBar = new JLabel();
  Calendar cal;
  java.text.SimpleDateFormat sdf;

  public HVPrimaryPSFrame() {
    super("", true, true, true, true);
    readPrimariesCfg();
    create();
  }

  public HVPrimaryPSFrame(int hostid) {
    super("", true, true, true, true);
    HostID = hostid;
    rpcSql = new DimRpcInfo("HV_DIM_SERVER_HID" + hostid +
                           "/HVDB");
    rpcSql.setServiceIDMode(false);

    rpcPrimeData = new DimRpcInfo("HV_DIM_SERVER_HID" + hostid +
                                  "/HV_PRIMARY_DATA");
    rpcPrimeCmd = new DimRpcInfo("HV_DIM_SERVER_HID" + hostid +
                                 "/HV_PRIMARY_CMD");

    readPrimariesCfg();
    create();
  }

  public HVPrimaryPSFrame(int hostid, int port, int addr) {
    super("", true, true, true, true);
    HostID = hostid;
    Port = port;
    Addr = addr;
    fSinglePrimary = true;
    rpcSql = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID +
                           "/HVDB");
    rpcSql.setServiceIDMode(false);

    rpcPrimeData = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID +
                                  "/HV_PRIMARY_DATA");
    rpcPrimeCmd = new DimRpcInfo("HV_DIM_SERVER_HID" + HostID +
                                 "/HV_PRIMARY_CMD");

    readPrimariesCfg(HostID, Port, Addr);
    create();
    updateTitle(HostID, Port, Addr);
  }

  private void create() {

//    openFrameCount++;
    try {

      jbInit();
      String DATE_FORMAT = "HH:mm:ss";
      sdf = new java.text.SimpleDateFormat(DATE_FORMAT);

      ActionListener taskPerformer = new ActionListener() {
        public void actionPerformed(ActionEvent evt) {
          cal = Calendar.getInstance(TimeZone.getDefault());
          if (fSinglePrimary) {
            readPrimariesCfg(HostID, Port, Addr);
          }
          else {
            readPrimariesCfg();
          }
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

    private void readPrimariesCfg(int hostid, int port, int addr) {

      String qryDB =
          "select PRIME.PORT, PRIME.ADDR from PRIME where PRIME.HOST=" +
          hostid + " and PRIME.PORT=" + port + " and PRIME.ADDR=" + addr +
          " order by PRIME.PORT, PRIME.ADDR";
      rpcSql.setData(qryDB);
      String reply = rpcSql.getString();
      // System.out.println(reply);
      String[] strPrimeList = reply.split(";");
      if (strPrimeList.length > 0) {
        primaries.clear();
        for (String strPrime : strPrimeList) {
          String[] strPrimeCfg = strPrime.split(",");
          if (strPrimeCfg.length == 2) {
            HVcmd entry = new HVcmd();
            entry.card = new Integer(strPrimeCfg[0]);
            entry.module = new Integer(strPrimeCfg[1]);
            entry.chan = 0;
            entry.cmd = 0;
            entry.data = new Long(0);
            entry.size = new Long(0);
            //System.out.println(entry.toString());
            primaries.put(entry, new HVprimary());
            // primaries.add(entry);
          }
        }
      }
    }

    private void readPrimariesCfg() {

      String qryDB =
          "select PRIME.PORT, PRIME.ADDR from PRIME where PRIME.HOST=" +
          HostID + " order by PRIME.PORT, PRIME.ADDR";
      rpcSql.setData(qryDB);
      String reply = rpcSql.getString();
      // System.out.println(reply);
      String[] strPrimeList = reply.split(";");
      if (strPrimeList.length > 0) {
        primaries.clear();
        for (String strPrime : strPrimeList) {
          String[] strPrimeCfg = strPrime.split(",");
          if (strPrimeCfg.length == 2) {
            HVcmd entry = new HVcmd();
            entry.card = new Integer(strPrimeCfg[0]);
            entry.module = new Integer(strPrimeCfg[1]);
            entry.chan = 0;
            entry.cmd = 0;
            entry.data = new Long(0);
            entry.size = new Long(0);
            // System.out.println(entry.toString());
            primaries.put(entry, new HVprimary());

            // primaries.add(entry);
          }
        }
      }
    }

    void UpdateData() {
      if (primaries.keySet().size() == 0) {
        return;
      }
      for (HVcmd entry : primaries.keySet()) {
        // System.out.println(entry.toString());
        if ((rpcPrimeData.setData(entry.encode())>0)
            && (rpcPrimeData.getSize() == primaries.get(entry).getSize()))
        {
          primaries.get(entry).decode(rpcPrimeData.getData());
        }
        else 
        {
            // System.out.println(rpcPrimeData.getSize()+ " "+primaries.get(entry).getSize());
        }
      }
      Object[][] data = new Object[primaries.values().size()][new HVprimary().
          getHeaders().length];
      int j = 0;
      for (HVprimary entry : primaries.values()) {
        data[j++] = entry.getValues();
      }

      if (jt.isEditing() || (jt.getSelectedRowCount() > 1) ||
          isEditingValue || menu.isVisible()) {
        statusBar.setText(
            " Status: Not Updated. Cell Editing or Selection Detected");
        return;
      }

      dtm.setData(data);

      int vseries = VmonPlotsPanel.getNumSeries();
      if (vseries != data.length) {
        VmonPlotsPanel.setNumSeries(data.length);
        vseries = data.length;
        for (int i = 0; i < vseries; i++) {
          VmonPlotsPanel.setSerieTitle(i, "PS" + data[i][0].toString() +
                                       ":" + data[i][1].toString() + ":" +
                                       data[i][2].toString());
        }

      }
      for (int i = 0; i < vseries; i++) {
        Object value = data[i][dtm.getColumnPos("Vmon")];
        try {
          Float val = Float.parseFloat(value.toString());
          VmonPlotsPanel.addValue(i, val);

        }
        catch (NumberFormatException nfe) {
          System.out.println("Unable to parse Vmon value. ");
        }
      }

      int iseries = ImonPlotsPanel.getNumSeries();
      if (iseries != data.length) {
        ImonPlotsPanel.setNumSeries(data.length);
        iseries = data.length;
        for (int i = 0; i < vseries; i++) {
          ImonPlotsPanel.setSerieTitle(i, "PS" + data[i][0].toString() +
                                       ":" + data[i][1].toString() + ":" +
                                       data[i][2].toString());
        }

      }
      for (int i = 0; i < iseries; i++) {
        Object value = data[i][dtm.getColumnPos("Imon")];
        try {
          Float val = Float.parseFloat(value.toString());
          ImonPlotsPanel.addValue(i, val);
        }
        catch (NumberFormatException nfe) {
          System.out.println("Unable to parse Imon value. ");
        }

      }

      statusBar.setText(" Status: Last Updated at " + sdf.format(cal.getTime()));
    }

    private void updateTitle(int hostid, int port, int addr) {
      setTitle("PrimaryPS:" + hostid + ":" + port + ":" + addr);
    }

    private void jbInit() throws Exception {
      setContentPane(jPanel1);
      // setContentPane(jTabbedPane1);
      HVprimary data = new HVprimary();
      // dtm.setColumnCount(data.getHeaders().length);
      dtm.setColumnIdentifiers(data.getHeaders(), false);
      dtm.setEditableColumn("Imax", true);
      dtm.setEditableColumn("Vset", true);
      dtm.setEditableColumn("RampUp", true);
      dtm.setEditableColumn("RampDown", true);
      dtm.setEditableColumn("State", true);

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
            HVcmd cmd = new HVcmd();
            // if (jt.getModel().getClass() == HVDataTableModel.class) {
            int portCol = 1; //(HVDataTableModel)(jt.getModel()).getColumnPos("Port");
            int addrCol = 2; //(HVDataTableModel)(jt.getModel()).getColumnPos("Addr");
            // }
            if (jt.getModel().getValueAt(currentRow, portCol).getClass() ==
                Integer.class) {
              cmd.card = (Integer) (jt.getModel().getValueAt(currentRow,
                  portCol));
            }
            if (jt.getModel().getValueAt(currentRow, addrCol).getClass() ==
                Integer.class) {
              cmd.module = (Integer) (jt.getModel().getValueAt(currentRow,
                  addrCol));
            }

//            cmd.card = 0;
//            cmd.module = 1;
            cmd.cmd = HVData.HV_CMD.HV_GET_MOD_DATA.ordinal();
            cmd.chan = HVData.ALL;
            cmd.data = new Long(0);
            cmd.size = new Long(0);

            if (jt.getColumnName(currentCol) == "State") {
              cmd.data = new Long(("ON".equals(value)) ? 1 : 0);
              if (cmd.chan == HVData.ALL) {
                cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
                cmd.chan = 0;
                rpcPrimeCmd.setData(cmd.encode());
              }
              cmd.chan = (currentRow == 0) ? HVData.ALL : currentRow - 1;
              cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
            }

            if (jt.getColumnName(currentCol) == "Vset") {
              cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
              cmd.data = new Long (( (Integer) value));
            }
            if (jt.getColumnName(currentCol) == "RampUp") {
              cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_UP.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (jt.getColumnName(currentCol) == "RampDown") {
              cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_DOWN.ordinal();
              cmd.data = new Long(( (Integer) value));
            }
            if (jt.getColumnName(currentCol) == "Imax") {
              cmd.cmd = HVData.HV_CMD.HV_SET_IMAX.ordinal();
              cmd.data = new Long(( (Float) value).intValue()); // ( (Integer) value);

            }

            rpcPrimeCmd.setData(cmd.encode());
            /*
                         Log.append(sdf.format(cal.getTime()) + "> Module_" +
             Integer.toString(HostID) + "." + Integer.toString(HVCard) +
                       "." + Integer.toString(BusAddr) +
                       "." + Integer.toString(cmd.chan) + " " +
                       HVData.HV_CMD.getValue(cmd.cmd).toString() +
                       " " + Integer.toString(cmd.data) + "\n");
             */

          }
          catch (Exception exception) {
            exception.printStackTrace();
          }
        } //editingStopped

      };

      KeyStroke inc_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_PERIOD, java.awt.event.InputEvent.CTRL_MASK);
      KeyStroke dec_font_size = KeyStroke.getKeyStroke(java.awt.event.KeyEvent.
          VK_COMMA, java.awt.event.InputEvent.CTRL_MASK);
      String inc_font_str = "inc_font";
      String dec_font_str = "dec_font";

      jt.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          inc_font_size, inc_font_str);
      jt.getActionMap().put(inc_font_str, new TableFontResizeAction(inc_font_str));
      jt.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(
          dec_font_size, dec_font_str);
      jt.getActionMap().put(dec_font_str, new TableFontResizeAction(dec_font_str));


      jt.setSelectionForeground(Color.RED);
      jt.setRowSelectionAllowed(true);
      //jt.setColumnSelectionAllowed(true);
      jt.setModel(dtm);
      jt.setDefaultRenderer(Integer.class, new HVTableObjectRenderer());
      jt.setDefaultRenderer(Float.class, new HVTableFloatRenderer());
      // jt.getTableHeader().setToolTipText(jt.getColumnName(1));
      TableColumnModel tcm = jt.getColumnModel();
      TableColumn tc = tcm.getColumn(dtm.getColumnPos("State"));
      tc.setCellRenderer(new HVTableRenderer());
      tc.setCellEditor(new HVTableEditor());
      tc = tcm.getColumn(dtm.getColumnPos("Status"));
      tc.setCellRenderer(new HVTableRenderer());

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



      // dtm.addColumn("Select");

      this.addInternalFrameListener(new
                                    HVPrimaryPSFrame_this_internalFrameAdapter(this));
      pack();
      setSize(800, 800);
      jPanel1.setLayout(borderLayout1);

      // jPanel1.add(jsp, java.awt.BorderLayout.CENTER);
      jPanel1.add(jTabbedPane1, java.awt.BorderLayout.CENTER);
      jPanel1.add(statusBar, java.awt.BorderLayout.SOUTH);

      jTabbedPane1.add(DataPanel, "Data");

      DataPanel.setLayout(new BorderLayout());
      DataPanel.add(jsp, java.awt.BorderLayout.CENTER);

      jTabbedPane1.add(PlotsPanel, "Plots");
      PlotsPanel.add(VmonPlotsPanel, "Voltage");
      PlotsPanel.add(ImonPlotsPanel, "Current");

      statusBar.setText("Status: ");
      jsp.getViewport().add(jt);
      jt.setAutoResizeMode(JTable.AUTO_RESIZE_ALL_COLUMNS);
      setTitle("Primary PS List for HID" + HostID);
    }

    private JPopupMenu CreatePopupMenu() {
      JPopupMenu menu = new JPopupMenu();
      JMenu set_parameters_menu = new JMenu("Set Parameters for Primary PS");
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
     JTable table = jt;
     HVcmd cmd = new HVcmd();
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
       if (value == null) return;
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
             " value:", 4000);
         if (value == null) return;
         if (value != null && value.length() > 0) {
           cmd.cmd = HVData.HV_CMD.HV_SET_VSET.ordinal();
           cmd.data = new Long(Integer.parseInt(value.toString()));
           if (cmd.data > 4200) {
             cmd.data = new Long(4200);
           }
         }
       }

       if (param.startsWith("Ramp Up")) {
         String value = JOptionPane.showInputDialog("Enter " + param +
             " value:", 15);
         if (value == null) return;
         if (value != null && value.length() > 0) {
           cmd.cmd = HVData.HV_CMD.HV_SET_RAMP_UP.ordinal();
           cmd.data = new Long(Integer.parseInt(value.toString()));
           if (cmd.data > 200) {
             cmd.data = new Long(200);
           }
         }
       }

       if (param.startsWith("Ramp Down")) {
         String value = JOptionPane.showInputDialog("Enter " + param +
             " value:", 100);
         if (value == null) return;
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
         if (value == null) return;
         if (value != null && value.length() > 0) {
           cmd.cmd = HVData.HV_CMD.HV_SET_IMAX.ordinal();
           cmd.data = new Long(Integer.parseInt(value.toString()));
           if (cmd.data > 60000) {
             cmd.data = new Long(60000);
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

     if (table.getSelectedRowCount() > 0) {
       int rowIndexStart = table.getSelectedRow();
       int rowIndexEnd = table.getSelectionModel().getMaxSelectionIndex();
       for (int r = rowIndexStart; r <= rowIndexEnd; r++) {
         if (table.isCellSelected(r, 0)) {
           // Get Module Address

           try {
             cmd.card = Integer.parseInt( table.getModel().getValueAt(r,
                 1).toString());
             cmd.module = Integer.parseInt( table.getModel().getValueAt(r,
                 2).toString());
             cmd.chan = HVData.ALL;

           }
           catch (NumberFormatException e) {
             System.out.println("Unable to parse primary PS address. ");
             return;
           }

           // cmd.csc = CSC;

           rpcPrimeCmd.setData(cmd.encode());
           System.out.println(cmd.toString());

           // System.out.println(cmd.toString());
           // cell is selected
         }
       }
       table.setRowSelectionInterval(rowIndexStart, rowIndexStart);
     }

     isEditingValue = false;

   }

   private HVcmd getModuleAddress(String type, String id) {
     HVcmd res = null;
     String query = "";
     if (type.startsWith("Master")) {
       query =
           "select HVCARD.HOST, HVCARD.DATASLOT, MASTERDB.ADDR from HVCARD, " +
           "MASTERDB where HVCARD.ID=MASTERDB.HOSTCARD and MASTERDB.ID=" + id;
     }
     if (type.startsWith("RDB")) {
       query = "select HVCARD.HOST, HVCARD.DATASLOT, RDB.ADDR from HVCARD, " +
           "RDB where HVCARD.ID=RDB.HOSTCARD and RDB.ID=" + id;
     }

     if (query.length() > 0) {
       rpcSql.setData(query);
       String[] SQLreply = (rpcSql.getString()).split(";");
       if (SQLreply.length > 0) {
         String[] csc_param = SQLreply[0].split(",");
         if (csc_param.length == 3) {
           try {
             res = new HVcmd();
             res.data = new Long(Integer.parseInt(csc_param[0])); // Modules Host ID
             res.card = Integer.parseInt(csc_param[1]);
             res.module = Integer.parseInt(csc_param[2]);
           }
           catch (NumberFormatException e) {
             System.out.println("Unable to parse module address. ");
             return null;
           }
         }
       }
     }
     return res;
   }



    public void this_internalFrameClosing(InternalFrameEvent e) {
      timer.stop();
      rpcSql.finalize();
      rpcPrimeCmd.finalize();
      rpcPrimeData.finalize();
    }
  }



  class HVPrimaryPSFrame_this_internalFrameAdapter
      extends InternalFrameAdapter {
    private HVPrimaryPSFrame adaptee;
    HVPrimaryPSFrame_this_internalFrameAdapter(HVPrimaryPSFrame adaptee) {
      this.adaptee = adaptee;
    }

    public void internalFrameClosing(InternalFrameEvent e) {
      adaptee.this_internalFrameClosing(e);
    }
  }
