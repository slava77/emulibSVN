package hvclient;

import java.awt.*;
import java.awt.event.*;
import javax.swing.JFrame;
import javax.swing.JInternalFrame;
import javax.swing.JOptionPane;
import javax.swing.JDesktopPane;
import javax.swing.JPanel;
import javax.swing.JMenuBar;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JToolBar;
import javax.swing.JButton;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import dim.DimClient;
import java.nio.ByteBuffer;
import javax.swing.JComboBox;

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
public class HVClientMainFrame
    extends JFrame {
  static final Integer MODULELAYER = new Integer(5);
  JPanel contentPane;
  JDesktopPane desktop = new JDesktopPane(); ;
  BorderLayout borderLayout1 = new BorderLayout();
  JMenuBar jMenuBar1 = new JMenuBar();
  JMenu jMenuFile = new JMenu();
  JMenuItem jMenuFileExit = new JMenuItem();
  JMenu jMenuOptions = new JMenu();
  JMenuItem jMenuOptionsSetDNS = new JMenuItem();
  JMenu jMenuHelp = new JMenu();
  JMenuItem jMenuHelpAbout = new JMenuItem();
  JToolBar jToolBar = new JToolBar();
  JButton jButton1 = new JButton();
  JButton jButton2 = new JButton();
  JButton jButton3 = new JButton();
  ImageIcon image1 = new ImageIcon(hvclient.HVClientMainFrame.class.getResource(
      "openFile.png"));
  ImageIcon image2 = new ImageIcon(hvclient.HVClientMainFrame.class.getResource(
      "closeFile.png"));
  ImageIcon image3 = new ImageIcon(hvclient.HVClientMainFrame.class.getResource(
      "help.png"));
  JLabel statusBar = new JLabel();
  JMenuItem jMenuItem1 = new JMenuItem();
  JMenuItem jMenuItem2 = new JMenuItem();
  JMenuItem jMenuItem3 = new JMenuItem();
  JMenuItem jMenuItem4 = new JMenuItem();
  int HostID = 500;
  JMenu jMenu1 = new JMenu();
  JMenuItem jMenuItem5 = new JMenuItem();
  JMenuItem jMenuItem6 = new JMenuItem();
  JMenuItem jMenuItem7 = new JMenuItem();
  JMenuItem jMenuItem8 = new JMenuItem();
  JMenu jMenu2 = new JMenu();
  JMenuItem jMenuItem9 = new JMenuItem();
  JMenuItem jMenuItemLogs = new JMenuItem();
  JMenuItem jMenuItemShowLog = new JMenuItem();
  JMenuItem jMenuItem10 = new JMenuItem();
  JMenuItem jMenuItem11 = new JMenuItem();
  public HVClientMainFrame() {
    try {
      setDefaultCloseOperation(EXIT_ON_CLOSE);
      jbInit();
      // OpenServerLogsWindow();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  /**
   * Component initialization.
   *
   * @throws java.lang.Exception
   */
  private void jbInit() throws Exception {
    contentPane = (JPanel) getContentPane();
    contentPane.setLayout(borderLayout1);
    final int inset = 50;
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    setBounds(inset, inset, screenSize.width - inset * 2,
              screenSize.height - inset * 2);

//    setSize(new Dimension(400, 300));
    UpdateTitle();
    // setTitle("HVClient ver." + HVClient.config.version + " DNS:"+HVClient.config.dnsnode+":"+HVClient.config.dnsport);
    this.addWindowListener(new HVClientMainFrame_this_windowAdapter(this));
    jButton1.addActionListener(new
                               HVClientMainFrame_OpenSystemBrowser_actionAdapter(this));
    jButton2.addActionListener(new HVClientMainFrame_jButton2_actionAdapter(this));
    jButton3.addActionListener(new HVClientMainFrame_jButton3_actionAdapter(this));
    jMenuItem4.setText("Open Primary PS List");
    jMenuItem4.addActionListener(new
                                 HVClientMainFrame_OpenPrimaryPSList_actionAdapter(this));

    jMenuItem3.setText("Open System Browser");
    jMenuItem3.addActionListener(new
                                 HVClientMainFrame_OpenSystemBrowser_actionAdapter(this));
    jMenuItem2.setText("Open Module List");
    jMenuItem2.addActionListener(new HVClientMainFrame_jMenuItem2_actionAdapter(this));
    jMenuItem1.setText("Open CSC List");
    jMenuItem1.addActionListener(new HVClientMainFrame_jMenuItem1_actionAdapter(this));
    jMenu1.setText("Actions");
    jMenuItem5.setText("Turn On Modules");
    jMenuItem5.addActionListener(new HVClientMainFrame_jMenuItem5_actionAdapter(this));
    jMenuItem6.setText("Turn Off Modules");
    jMenuItem6.addActionListener(new HVClientMainFrame_jMenuItem6_actionAdapter(this));
    jMenuItem7.setText("Turn On File Writing");
    jMenuItem7.addActionListener(new HVClientMainFrame_jMenuItem7_actionAdapter(this));
    jMenuItem8.setText("Turn Off File Writing");
    jMenuItem8.addActionListener(new HVClientMainFrame_jMenuItem8_actionAdapter(this));
    jMenu2.setText("All Modules");
    jMenuItem9.setText("Send Command");
    jMenuItem9.addActionListener(new HVClientMainFrame_jMenuItem9_actionAdapter(this));
    jMenuItemShowLog.setText("Show Server Log");
    jMenuItemShowLog.addActionListener(new
                                       HVClientMainFrame_jMenuItemShowLog_actionAdapter(this));
    jMenuItem10.setText("Open Wiener PL512");
    jMenuItem10.addActionListener(new
                                  HVClientMainFrame_jMenuItem10_actionAdapter(this));
    jMenuItem11.addActionListener(new
                                  HVClientMainFrame_jMenuItem11_actionAdapter(this));
    jMenuItem11.setText("Force AutoDetection");
    contentPane.add(desktop);
    statusBar.setText(" ");
    jMenuFile.setText("Open");
    jMenuFileExit.setText("Exit");
    jMenuFileExit.addActionListener(new
                                    HVClientMainFrame_jMenuFileExit_ActionAdapter(this));

    jMenuOptions.setText("Options");
    jMenuOptionsSetDNS.setText("Set DIM DNS Node");
    jMenuOptionsSetDNS.addActionListener(new
                                         HVClientMainFrame_jMenuOptionsSetDNS_ActionAdapter(this));

    jMenuHelp.setText("Help");
    jMenuHelpAbout.setText("About");
    jMenuHelpAbout.addActionListener(new
                                     HVClientMainFrame_jMenuHelpAbout_ActionAdapter(this));
    jMenuBar1.add(jMenuFile);
    jMenuBar1.add(jMenu1);
    jMenuFile.add(jMenuItem3);
    jMenuFile.add(jMenuItem2);
    jMenuFile.add(jMenuItem1);
    jMenuFile.add(jMenuItem4);
    jMenuFile.add(jMenuItem10);
    jMenuFile.addSeparator();
    jMenuFile.add(jMenuFileExit);
    jMenuBar1.add(jMenuOptions);
    jMenuOptions.add(jMenuOptionsSetDNS);
    jMenuBar1.add(jMenuHelp);
    jMenuHelp.add(jMenuHelpAbout);
    setJMenuBar(jMenuBar1);
    jButton1.setIcon(image1);
    jButton1.setToolTipText("Open System Browser");
    jButton2.setIcon(image2);
    // jButton2.setToolTipText("Close File");
    jButton3.setIcon(image3);
    jButton3.setToolTipText("Help");
    jToolBar.add(jButton1);
    jToolBar.add(jButton2);
    jToolBar.add(jButton3);
    contentPane.add(jToolBar, BorderLayout.NORTH);
    contentPane.add(statusBar, BorderLayout.SOUTH);
    jMenu1.add(jMenu2);
    jMenu1.add(jMenuItem11);
    jMenu1.addSeparator();
    jMenu1.add(jMenuItem9);
    jMenu1.add(jMenuItemShowLog);
    jMenu2.add(jMenuItem5);
    jMenu2.add(jMenuItem6);
    jMenu2.add(jMenuItem7);
    jMenu2.add(jMenuItem8);
    // statusBar.setText("DNS:" + HVClient.config.dnsnode);

  }

  public void UpdateTitle() {
    setTitle("HVClient ver." + HVClient.config.version + " - (DIM DNS=" +
             HVClient.config.dnsnode + ":" + HVClient.config.dnsport + ")");
  }

  /**
   * File | Exit action performed.
   *
   * @param actionEvent ActionEvent
   */
  void jMenuFileExit_actionPerformed(ActionEvent actionEvent) {
    HVClient.config.SaveConfig();
    System.exit(0);
  }

  /**
   * Options | Set DIM DNS Node action performed.
   *
   * @param actionEvent ActionEvent
   */
  void jMenuOptionsSetDNS_actionPerformed(ActionEvent actionEvent) {

    JComboBox dnslist = new JComboBox(HVClient.config.dns_history.toArray());
//     HVClient.config.dns_history.
    dnslist.setSelectedItem(HVClient.config.dnsnode + ":" +
                            HVClient.config.dnsport);
    dnslist.setEditable(true);
    int action = JOptionPane.showConfirmDialog(null, dnslist,
                                               "Select DIM DNS Node",
                                               JOptionPane.OK_CANCEL_OPTION);

    /*
         String s = (String) JOptionPane.showInputDialog(
        this,
        "Enter DIM DNS Node:",
        "Enter DIM DNS Node",
        JOptionPane.QUESTION_MESSAGE,
        null,
        HVClient.config.dns_history.toArray(),
        HVClient.config.dnsnode + ":" + HVClient.config.dnsport);
     */

    if (action == 0) {
      String s = dnslist.getSelectedItem().toString();
      if (s != null && s.length() > 0) {
        // HVClient.config.dns_history.add(s);
        String[] dnspair = s.split(":");
        if (dnspair.length == 2) {
          HVClient.config.dnsnode = dnspair[0];
          try {
            HVClient.config.dnsport = Integer.parseInt(dnspair[1]);
          }
          catch (NumberFormatException e) {
            System.out.println("Invalid DIM DNS port number. " +
                               "Reverting to " + DimClient.getDnsPort());
            // HVconfig.dnsport = DimClient.getDnsPort();
          }
        }
        else {
          HVClient.config.dnsnode = dnspair[0];
        }
        HVClient.config.dns_history.add(HVClient.config.dnsnode + ":" +
                                        HVClient.config.dnsport);
        DimClient.setDnsNode(HVClient.config.dnsnode, HVClient.config.dnsport);
        System.out.println("Set to DIM_DNS_NODE=" + DimClient.getDnsNode() +
                           ":" + DimClient.getDnsPort());
//        statusBar.setText("DNS:" + HVClient.config.dnsnode);
        UpdateTitle();
      }
    }
  }

  /**
   * Help | About action performed.
   *
   * @param actionEvent ActionEvent
   */
  void jMenuHelpAbout_actionPerformed(ActionEvent actionEvent) {
    HVClientMainFrame_AboutBox dlg = new HVClientMainFrame_AboutBox(this);
    Dimension dlgSize = dlg.getPreferredSize();
    Dimension frmSize = getSize();
    Point loc = getLocation();
    dlg.setLocation( (frmSize.width - dlgSize.width) / 2 + loc.x,
                    (frmSize.height - dlgSize.height) / 2 + loc.y);
    dlg.setModal(true);
    dlg.pack();
    dlg.setVisible(true);
  }

  public void OpenSystemBrowser_actionPerformed(ActionEvent e) {
    OpenSystemBrowserWindow();
  }

  public void OpenModulesStatus_actionPerformed(ActionEvent e) {
    OpenModulesStatusWindow();
  }

  public void OpenCSCStatus_actionPerformed(ActionEvent e) {
    OpenCSCStatusWindow();
  }

  public void OpenPrimaryPSListWindow_actionPerformed(ActionEvent e) {
    OpenPrimaryPSListWindow();
  }

  public void OpenTestFrame_actionPerformed(ActionEvent e) {
    /*
         JInternalFrame framePrime = new TestFrame();
         desktop.add(framePrime, MODULELAYER);
         try {
      framePrime.setVisible(true);
      framePrime.setSelected(true);
         }
         catch (java.beans.PropertyVetoException e2) {}
     */

  }

  public void this_windowClosing(WindowEvent e) {
    HVClient.config.SaveConfig();
  }

  public void OpenSystemBrowserWindow() {

    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    if (isWindowExist(desktop,
                      "System Browser for HID" + hostid)) { // +  DIM_DNS_NODE=" + DimClient.getDnsNode() +
      // ":" + DimClient.getDnsPort())) {
      return;
    }

    JInternalFrame framePrime = new HVSystemBrowser(hostid);
    desktop.add(framePrime, MODULELAYER);
    try {
      framePrime.setVisible(true);
      framePrime.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}

  }

  public void OpenModulesStatusWindow() {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    if (isWindowExist(desktop, "Modules Status for HID" + hostid)) {
      return;
    }

    JInternalFrame frameModuleStatus = new HVModuleStatusFrame(hostid);
    desktop.add(frameModuleStatus, MODULELAYER);
    try {
      frameModuleStatus.setVisible(true);
      frameModuleStatus.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}
  }

  public void OpenCSCStatusWindow() {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    if (isWindowExist(desktop, "CSC Status for HID" + hostid)) {
      return;
    }

    JInternalFrame frameCSCStatus = new HVCSCStatusFrame(hostid);
    desktop.add(frameCSCStatus, MODULELAYER);
    try {
      frameCSCStatus.setVisible(true);
      frameCSCStatus.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}
  }

  public void OpenPrimaryPSListWindow() {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    if (isWindowExist(desktop, "Primary PS List for HID" + hostid)) {
      return;
    }

    JInternalFrame framePrimaryPS = new HVPrimaryPSFrame(hostid);
    desktop.add(framePrimaryPS, MODULELAYER);
    try {
      framePrimaryPS.setVisible(true);
      framePrimaryPS.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}
  }

  public boolean isWindowExist(JDesktopPane desktop, String title) {
    for (JInternalFrame frame : desktop.getAllFrames()) {
      if (frame.getTitle().startsWith(title)) {
        // System.out.println(title + " " +frame.getTitle());
        try {
          frame.setVisible(true);
          frame.setIcon(false);
          frame.setSelected(true);
        }
        catch (java.beans.PropertyVetoException e2) {}
        return true;
      }
    }
    return false;
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

  public void jMenuItem5_actionPerformed(ActionEvent e) {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    TurnOnAllModules(hostid);

  }

  public void jMenuItem6_actionPerformed(ActionEvent e) {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    TurnOffAllModules(hostid);

  }

  public void jMenuItem7_actionPerformed(ActionEvent e) {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    TurnOnFileWriting(hostid);

  }

  public void jMenuItem8_actionPerformed(ActionEvent e) {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    TurnOffFileWriting(hostid);

  }

  public void jMenuItem9_actionPerformed(ActionEvent e) {
    OpenSendCommandWindow();
  }

  public void OpenSendCommandWindow() {
    int hostid = -1;
    if (isWindowExist(desktop, "Send Command")) {
      return;
    }
    /*
         if ( (hostid = selectHostID()) < 0) {
      return;
         }
     */
    JInternalFrame frameSendCommand = new HVSendCommandFrame();
    desktop.add(frameSendCommand, MODULELAYER);
    try {
      frameSendCommand.setVisible(true);
      frameSendCommand.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}

  }

  public void TurnOnFileWriting(int hostid) {
    HVcmd cmd = new HVcmd();
    cmd.card = HVData.ALL;
    cmd.module = HVData.ALL;
    cmd.chan = HVData.ALL;
    cmd.cmd = HVData.HV_CMD.HV_SET_MOD_POLICY.ordinal();
    cmd.data = new Long(1);
    cmd.size = new Long(0);

    ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());

    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());
  }

  public void TurnOffFileWriting(int hostid) {
    HVcmd cmd = new HVcmd();
    cmd.card = HVData.ALL;
    cmd.module = HVData.ALL;
    cmd.chan = HVData.ALL;
    cmd.cmd = HVData.HV_CMD.HV_SET_MOD_POLICY.ordinal();
    cmd.data = new Long(0);
    cmd.size = new Long(0);

    ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());

    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());
  }

  public void TurnOffAllModules(int hostid) {
    HVcmd cmd = new HVcmd();
    cmd.card = HVData.ALL;
    cmd.module = HVData.ALL;
    cmd.chan = HVData.ALL;
    cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
    cmd.data = new Long(HVData.HV_STATE.HV_STATE_OFF.ordinal());
    cmd.size = new Long(0);

    ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());

    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());
    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_PRIMARY_CMD/RpcIn", dataIn.array());
    cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
    dataIn.clear();
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());
    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());

  }

  public void TurnOnAllModules(int hostid) {
    HVcmd cmd = new HVcmd();
    cmd.card = HVData.ALL;
    cmd.module = HVData.ALL;
    cmd.chan = HVData.ALL;
    cmd.cmd = HVData.HV_CMD.HV_SET_MOD_STATE.ordinal();
    cmd.data = new Long(HVData.HV_STATE.HV_STATE_ON.ordinal());
    cmd.size = new Long(0);
    ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());

    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());
    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_PRIMARY_CMD/RpcIn", dataIn.array());

    cmd.cmd = HVData.HV_CMD.HV_SET_STATE.ordinal();
    dataIn.clear();
    dataIn.putLong(new Long(hostid));
    dataIn.put(cmd.encode());
    DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                          "/HV_REAL_CMD/RpcIn", dataIn.array());

  }


  public void ForceModulesAutoDetection(int hostid) {
     HVcmd cmd = new HVcmd();
     cmd.card = HVData.ALL;
     cmd.module = HVData.ALL;
     cmd.chan = HVData.ALL;
     cmd.cmd = HVData.HV_CMD.HV_DETECT.ordinal();
     cmd.data = new Long(1);
     cmd.size = new Long(0);
     ByteBuffer dataIn = ByteBuffer.allocate(cmd.encode().capacity() + 8);
     dataIn.putLong(new Long(hostid));
     dataIn.put(cmd.encode());

     DimClient.sendCommand(HVClient.config.ServerName + "_HID" + hostid +
                           "/HV_REAL_CMD/RpcIn", dataIn.array());
   }


  public void jMenuItemShowLog_actionPerformed(ActionEvent e) {
    OpenServerLogsWindow();
  }

  public void OpenServerLogsWindow() {
    if (isWindowExist(desktop, "Server Logs")) {
      return;
    }
    // System.out.println("Create Log Window");
    try {
      HVServerLogsFrame frameServerLogs = new HVServerLogsFrame();
      desktop.add(frameServerLogs, MODULELAYER);
      frameServerLogs.relocate();


      frameServerLogs.setVisible(true);
      frameServerLogs.setSelected(true);
      frameServerLogs.show();
    }
    catch (java.beans.PropertyVetoException e2) {}


  }

  public void OpenWienerPL512Window() {
    if (isWindowExist(desktop, "WienerPL512")) {
      return;
    }
    /*
         if ( (hostid = selectHostID()) < 0) {
      return;
         }
     */
    JInternalFrame frameWienerPL512 = new WienerPL512Frame();
    desktop.add(frameWienerPL512, MODULELAYER);
    try {
      frameWienerPL512.setVisible(true);
      frameWienerPL512.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}

  }


  public void jMenuItem10_actionPerformed(ActionEvent e) {
    OpenWienerPL512Window();
  }

  public void jMenuItem11_actionPerformed(ActionEvent e) {
    int hostid = 0;
    if ( (hostid = selectHostID()) < 0) {
      return;
    }
    int res = JOptionPane.showConfirmDialog(null,
                                            "Send Force AutoDetection command",
                                            "Force AutoDetection",
                                            JOptionPane.OK_CANCEL_OPTION);
    if (res == JOptionPane.OK_OPTION) {
      ForceModulesAutoDetection(hostid);
    }
  }

}

class HVClientMainFrame_jMenuItem11_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem11_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem11_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem10_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem10_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem10_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItemShowLog_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItemShowLog_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItemShowLog_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem9_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem9_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem9_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem8_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem8_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem8_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem7_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem7_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem7_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem5_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem5_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem5_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem6_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem6_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuItem6_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem1_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem1_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.OpenCSCStatus_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuItem2_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jMenuItem2_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.OpenModulesStatus_actionPerformed(e);
  }
}

class HVClientMainFrame_OpenPrimaryPSList_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_OpenPrimaryPSList_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.OpenPrimaryPSListWindow_actionPerformed(e);
  }
}

class HVClientMainFrame_this_windowAdapter
    extends WindowAdapter {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_this_windowAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void windowClosing(WindowEvent e) {
    adaptee.this_windowClosing(e);
  }
}

class HVClientMainFrame_jButton3_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jButton3_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jMenuHelpAbout_actionPerformed(e);
//    adaptee.OpenCSCStatus_actionPerformed(e);
  }
}

class HVClientMainFrame_jButton2_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_jButton2_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.OpenTestFrame_actionPerformed(e);
  }
}

class HVClientMainFrame_OpenSystemBrowser_actionAdapter
    implements ActionListener {
  private HVClientMainFrame adaptee;
  HVClientMainFrame_OpenSystemBrowser_actionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.OpenSystemBrowser_actionPerformed(e);
  }
}

class HVClientMainFrame_jMenuFileExit_ActionAdapter
    implements ActionListener {
  HVClientMainFrame adaptee;

  HVClientMainFrame_jMenuFileExit_ActionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent actionEvent) {
    adaptee.jMenuFileExit_actionPerformed(actionEvent);
  }
}

class HVClientMainFrame_jMenuHelpAbout_ActionAdapter
    implements ActionListener {
  HVClientMainFrame adaptee;

  HVClientMainFrame_jMenuHelpAbout_ActionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent actionEvent) {
    adaptee.jMenuHelpAbout_actionPerformed(actionEvent);
  }
}

class HVClientMainFrame_jMenuOptionsSetDNS_ActionAdapter
    implements ActionListener {
  HVClientMainFrame adaptee;

  HVClientMainFrame_jMenuOptionsSetDNS_ActionAdapter(HVClientMainFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent actionEvent) {
    adaptee.jMenuOptionsSetDNS_actionPerformed(actionEvent);
  }
}
