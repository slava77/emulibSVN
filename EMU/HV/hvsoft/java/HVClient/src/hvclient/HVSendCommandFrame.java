package hvclient;

import java.awt.*;
import javax.swing.*;
import java.util.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import dim.DimClient;
import java.nio.ByteBuffer;
import java.awt.event.KeyEvent;
import java.awt.event.KeyAdapter;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

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
public class HVSendCommandFrame
    extends JInternalFrame {

  String help = "<html><pre><b>Supported options:</b><br>"
      +"<b>ME-/xx/xx</b>     set CSC name (multiple CSC names parameters supported)<br>"
      +"<b>-h</b>N     set computer host ID (default - use first matching name DIM server)<br>"
      + "<b>-n</b>N     set hostcard address (default 0)<br>"
      + "<b>-a</b>N     set module address N(0..15, All=255, All RDBs=249, RDB30 Only=250, RDB36 Only=251, Masters Only=252)<br>"
      + "<b>-c</b>N     set modules channel (N <= 36, All=255) (multiple channels parameters supported)<br>"
      + "<b>-o</b>N     set module state (0-OFF, 1-ON)<br>"
      + "<b>-s</b>N     set module state or channel state (with -c) (0-OFF, 1-ON)<br>"
      + "<b>-r</b>      reset trip state condition<br>"
      + "<b>-v</b>N     set voltage in DAC counts (0..4000)<br>"
      + "<b>-t</b>N     set maximum trip current level in counts (0..3000)<br>"
      + "<b>-i</b>N     set trip reset delay in msec<br>"
      + "<b>-m</b>N     set max trip resets attempt<br>"
      + "<b>-j</b>N     set trip delay in msec<br>"
      // +"-kN     set linked master trip delay in msec<br>"
      // +"-yN     set maximum voltage level in counts (0..4000)<br>"
      + "<b>-u</b>N     set ramp up speed in DAC counts (0..4000)<br>"
      + "<b>-d</b>N     set ramp down speed in DAC counts (0..4000)<br>"
      // +"-fN     set relay state for master board (0-OFF, >0-ON)<br>"
      + "<b>-q</b>N     set interlock delay in ms)<br>"
      + "<b>-z</b>      disable calibration data, raw DAC-ADC mode<br>"
      + "<b>-w</b>N     saving of data from module in file mode (0-OFF, >0-ON<br>"
      + "<b>-g</b>      set Primary Supply Control mode<br>"
      + "<b>-A</b>      force hostcards module detection and re-configuration"
      // +"-Ttype  force module type filter (type: RDBALL, RDB30, RDB36, MASTER)<br>";
      + "</pre></html>";
  int HostID = 0;

  BorderLayout borderLayout1 = new BorderLayout();
  JPanel jPanel1 = new JPanel();
  JPanel jPanel2 = new JPanel();

  JSplitPane jSplitPane1 = new JSplitPane();
  JLabel statusBar = new JLabel();
  JScrollPane jScrollPane1 = new JScrollPane();
  JEditorPane Log = new JEditorPane();
  JTextField Cmd = new JTextField();
  // JButton ClearHistory()
  JComboBox CmdHistory = new JComboBox(HVClient.config.cmd_history.toArray());

  public HVSendCommandFrame() {
    super("", true, true, true, true);
    try {
      jbInit();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  public HVSendCommandFrame(int hostid) {
    super("", true, true, true, true);
    HostID = hostid;
    try {
      jbInit();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  private void jbInit() throws Exception {
    setContentPane(jPanel1);
    pack();
    setSize(800, 300);
    getContentPane().setLayout(borderLayout1);
    jSplitPane1.setOrientation(JSplitPane.VERTICAL_SPLIT);
    Cmd.addActionListener(new HVSendCommandFrame_Cmd_actionAdapter(this));
    Cmd.setEditable(true);
    Cmd.setToolTipText(help);
    // Cmd.removeAllItems();
    // Cmd.addA
    jPanel2.setLayout(new BorderLayout());
    CmdHistory.addActionListener(new
                                 HVSendCommandFrame_CmdHistory_actionAdapter(this));
    jPanel2.add(Cmd, java.awt.BorderLayout.CENTER);
    jPanel2.add(CmdHistory, java.awt.BorderLayout.SOUTH);
    jSplitPane1.add(jPanel2, JSplitPane.TOP);
    jSplitPane1.add(jScrollPane1, JSplitPane.BOTTOM);
    jScrollPane1.getViewport().add(Log);
    Log.setText("");
    // Log.setEditable(false);
    final JPopupMenu menu = new JPopupMenu();
    menu.add(new JMenuItem("Clear"));

    // Set the component to show the popup menu
    CmdHistory.addMouseListener(new MouseAdapter() {
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

    Log.addMouseListener(new MouseAdapter() {
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

    jPanel1.add(jSplitPane1, java.awt.BorderLayout.CENTER);
    jPanel1.add(statusBar, java.awt.BorderLayout.SOUTH);
    statusBar.setText("Status: ");
    setTitle("Send Command");
  }

  public void Cmd_actionPerformed(ActionEvent e) {

    // System.out.println("Fired");
    int hostid = HostID;
    boolean fPrimaryMode = false;
    boolean fRawMode = false;
    boolean fCSCMode = false;
    String CSC = "";
    String c = Cmd.getText().replaceAll("^\\s+", "").
        replaceAll("\\s+", " ").replaceAll("$\\s+", "");
    Cmd.setText(c);
    /*getSelectedItem().toString().replaceAll("^\\s+", "").
             replaceAll("\\s+", " ").replaceAll("$\\s+", "");
         Cmd.removeItem(Cmd.getSelectedItem());
         Cmd.addItem(c);
         Cmd.setSelectedItem(c);
     */
    // Cmd.


    String[] cmdline = c.split(" ");

    HVcmd cmd = new HVcmd();
    cmd.card = HVData.ALL;
    cmd.module = HVData.ALL;
    cmd.chan = HVData.ALL;
    cmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();

    HVcsccmd csccmd = new HVcsccmd();
    csccmd.cmd = HVData.HV_CMD.LAST_CMD.ordinal();

    // Vector commands = new Vector(0);
    SortedSet<Integer> channels = new TreeSet();
    SortedSet<String> CSCs = new TreeSet<String> ();
    Map<HVData.HV_CMD, Integer>
        commands = new HashMap<HVData.HV_CMD, Integer> (0);

    // Log.setText(new Integer(cmdline.length).toString() + "\n");
    for (String cmdstr : cmdline) {
      try {
        if (cmdstr.toUpperCase().startsWith("ME")) {
          fCSCMode = true;
          CSCs.add(cmdstr);
        }
        if (cmdstr.startsWith("-A")) {
          commands.put(HVData.HV_CMD.HV_DETECT, 1);
        }
        if (cmdstr.startsWith("-h")) {
          HostID = Integer.parseInt(cmdstr.replaceAll("\\D+", ""));
        }
        if (cmdstr.startsWith("-n")) {
          cmd.card = Integer.parseInt(cmdstr.replaceAll("\\D+", ""));
        }
        if (cmdstr.startsWith("-a")) {
          cmd.module = Integer.parseInt(cmdstr.replaceAll("\\D+", ""));
        }
        if (cmdstr.startsWith("-c")) {
          channels.add(Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-v")) {
          commands.put(HVData.HV_CMD.HV_SET_VSET,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-t")) {
          commands.put(HVData.HV_CMD.HV_SET_IMAX,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-u")) {
          commands.put(HVData.HV_CMD.HV_SET_RAMP_UP,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-d")) {
          commands.put(HVData.HV_CMD.HV_SET_RAMP_DOWN,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-j")) {
          commands.put(HVData.HV_CMD.HV_SET_TRIP_DELAY,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-i")) {
          commands.put(HVData.HV_CMD.HV_SET_TRIP_RESET_DELAY,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-q")) {
          commands.put(HVData.HV_CMD.HV_SET_ILOCK_DELAY,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-r")) {
          commands.put(HVData.HV_CMD.HV_RESET_TRIP, 1);
        }
        if (cmdstr.startsWith("-m")) {
          commands.put(HVData.HV_CMD.HV_SET_TRIP_MAX_RESETS,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-o")) {
          commands.put(HVData.HV_CMD.HV_SET_MOD_STATE,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-w")) {
          commands.put(HVData.HV_CMD.HV_SET_MOD_POLICY,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
        if (cmdstr.startsWith("-g")) {
          fPrimaryMode = true;
        }

        if (cmdstr.startsWith("-z")) {
          fRawMode = true;
        }

        if (cmdstr.startsWith("-s")) {
          if (channels.size() == 0) {
            commands.put(HVData.HV_CMD.HV_SET_MOD_STATE,
                         Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
          }
          commands.put(HVData.HV_CMD.HV_SET_STATE,
                       Integer.parseInt(cmdstr.replaceAll("\\D+", "")));
        }
      }
      catch (NumberFormatException nfe) {
        System.out.println("Unable to parse value as Integer");
      }
    }

    if (HostID == 0) {
      HostID = selectHostID();
    }

    String serviceType = (fCSCMode) ? "/HV_CSC_CMD" :
        ( (fRawMode) ? "/HV_RAW_CMD" :
         ( (fPrimaryMode) ? "/HV_PRIMARY_CMD" : "/HV_REAL_CMD"));
    String serviceName = HVClient.config.ServerName + "_HID"
        + HostID + serviceType + "/RpcIn";
    // Log.setText(cmd.toString());

    if (channels.size() == 0) {
      channels.add(cmd.chan);
    }
    int cmdcount = 0;
    if (fCSCMode) {
      for (String csc : CSCs) {
        csccmd.csc = csc;
        for (Integer i : channels) {
          csccmd.chan = i;
          for (HVData.HV_CMD cmdcode : commands.keySet()) {
            csccmd.cmd = cmdcode.ordinal();
            csccmd.data = new Long(commands.get(cmdcode));
            ByteBuffer dataIn = ByteBuffer.allocate(csccmd.encode().capacity() +
                8);
            dataIn.putLong(new Long(hostid));
            dataIn.put(csccmd.encode());
            int status = DimClient.sendCommand(serviceName, dataIn.array());
            cmdcount++;
            Log.setText(Log.getText() + "Send command" + cmdcount +
                        "->  hostid:"
                        + HostID + csccmd.SEP + csccmd.toString() + csccmd.SEP +
                        "Status->" + ( (status > 0) ? "OK" : "FAILED") + "\n");
          }
        }
      }
    }
    else {
      for (Integer i : channels) {
        cmd.chan = i;
        for (HVData.HV_CMD cmdcode : commands.keySet()) {
          cmd.cmd = cmdcode.ordinal();
          cmd.data = new Long(commands.get(cmdcode));
          ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
          dataIn.putLong(new Long(hostid));
          dataIn.put(cmd.encode());
          int status = DimClient.sendCommand(serviceName, dataIn.array());
          cmdcount++;
          System.out.println(cmdcount + ":" + cmdcode);
          Log.setText(Log.getText() + serviceName + "|Send command" + cmdcount + "-> hostid:"
                      + HostID + cmd.SEP + cmd.toString() + cmd.SEP +
                      "Status->" + ( (status > 0) ? "OK" : "FAILED") + "\n");
        }
      }
    }

    if (cmdcount > 0) {
      HVClient.config.cmd_history.add(c);
      // CmdHistory.setSelectedItem(c);
      // System.out.println(CmdHistory.getSelectedIndex());
      CmdHistory.addItem(c);
      // CmdHistory = new JComboBox(HVClient.config.cmd_history.toArray());
      // CmdHistory.revalidate();
    }
// Log.setText(Log.getText()+cmdstr+"\n");
// DimClient.sendCommand(serviceName, cmd.encode().array());


  }

  public int selectHostID() {
    int hostid = HostID;
    String[] services = DimBrowserNew.getServices(HVClient.config.ServerName +
                                                  "*/HVDB");
    String service = HVClient.config.ServerName + "/HVDB";

    // exit if no matched services found
    if (services.length <= 0) {
      JOptionPane.showMessageDialog(this,
                                    "Unable to find services.",
                                    "Dim Error",
                                    JOptionPane.ERROR_MESSAGE);

//      System.out.println("ERROR: Unable to found services");
      return -1;
    }

//    service = services[0];

    if (services.length > 0) {

      service = services[0];
      for (int i = 0; i < services.length; i++) {
        services[i] = services[i].replaceAll(HVClient.config.ServerName +
                                             "_HID", "");
        services[i] = services[i].replaceAll("/HVDB", "");
      }
      // service = services[0];

      try {
        hostid = Integer.parseInt(services[0]);
      }
      catch (NumberFormatException e) {
        System.out.println("Invalid HostID " +
                           "Reverting to " + HostID);
        hostid = HostID;
      }

      if (services.length > 1) {

        String s = (String) JOptionPane.showInputDialog(
            this,
            "Select HV Server HostID:",
            "Select HV Server HostID",
            JOptionPane.PLAIN_MESSAGE,
            null,
            services,
            services[0]);

        //If a string was returned set service name to it
        if ( (s != null) && (s.length() > 0)) {
          try {
            hostid = Integer.parseInt(s);
          }
          catch (NumberFormatException e) {
            System.out.println("Invalid HostID " +
                               "Reverting to " + HostID);
            hostid = HostID;
          }
        }
      }
    }
    return hostid;
  }

  public void CmdHistory_actionPerformed(ActionEvent e) {
    System.out.println(CmdHistory.getSelectedItem().toString());
    Cmd.setText(CmdHistory.getSelectedItem().toString());
  }

}

class HVSendCommandFrame_Cmd_actionAdapter
    implements ActionListener {
  private HVSendCommandFrame adaptee;
  HVSendCommandFrame_Cmd_actionAdapter(HVSendCommandFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.Cmd_actionPerformed(e);
  }
}

class HVSendCommandFrame_CmdHistory_actionAdapter
    implements ActionListener {
  private HVSendCommandFrame adaptee;
  HVSendCommandFrame_CmdHistory_actionAdapter(HVSendCommandFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.CmdHistory_actionPerformed(e);
  }
}
