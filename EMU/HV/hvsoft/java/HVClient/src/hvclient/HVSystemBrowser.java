package hvclient;

import dim.*;
import java.util.regex.Pattern;
import java.lang.*;
import java.awt.*;
import javax.swing.*;
import java.awt.BorderLayout;
import javax.swing.tree.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.MouseAdapter;

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
public class HVSystemBrowser
    extends JInternalFrame {
  static final Integer MODULELAYER = new Integer(5);
  // String ServerName = "HV_DIM_SERVER";
  int HostID = 0;
  BorderLayout borderLayout1 = new BorderLayout();
  JSplitPane jSplitPane1 = new JSplitPane();
  JScrollPane jScrollPane2 = new JScrollPane();
  DefaultMutableTreeNode top =
      new DefaultMutableTreeNode("CSCs");

  JTree CSCBrowser; // = new JTree(top);

  JSplitPane jSplitPane2 = new JSplitPane();
  JScrollPane jScrollPane1 = new JScrollPane();
  JEditorPane SQLresults = new JEditorPane();
  JPanel jPanel1 = new JPanel();
  JTextField SQLquery = new JTextField();
  BorderLayout borderLayout2 = new BorderLayout();
  DimRpcInfo sqlRpc;
  String qry_getRDBlist = "select BOARD_types.NAME, RDB.ID, HVCARD.HOST," +
      "HVCARD.DATASLOT, RDB.ADDR FROM RDB, BOARD_types, HVCARD " +
      "where RDB.TYPE = BOARD_types.ID and RDB.HOSTCARD=HVCARD.ID order by RDB.ID";
  /*
   String qry_getMap = "select MAP.HOST, MAP.PRIME, MAP.MASTER, MAP.MASTER_CHAN, MAP.RDB, CSC.NAME " +
      "from MAP, CSC where MAP.CSC=CSC.ID order by MAP.HOST, MAP.PRIME, MAP.MASTER, MAP.MASTER_CHAN, MAP.RDB, MAP.RDB_PART";
   */

  String qry_getMap = "select MAP.HOST, PRIME.PORT, PRIME.ADDR, MAP.MASTER, MAP.MASTER_CHAN, MAP.RDB, CSC.NAME " +
      "from MAP, PRIME, CSC where MAP.PRIME=PRIME.ID and MAP.CSC=CSC.ID order by MAP.HOST, PRIME.PORT, PRIME.ADDR, MAP.MASTER, MAP.MASTER_CHAN, MAP.RDB, MAP.RDB_PART";

  String qry_getMBlist = "select BOARD_types.NAME, MASTERDB.ID, HVCARD.HOST," +
      "HVCARD.DATASLOT, MASTERDB.ADDR FROM MASTERDB, BOARD_types, HVCARD " +
      "where MASTERDB.TYPE = BOARD_types.ID and MASTERDB.HOSTCARD=HVCARD.ID order by MASTERDB.ID";
  String qry_getPrimePSlist = "select PRIME.HOST, PRIME.PORT, PRIME.ADDR from PRIME order by PRIME.HOST, PRIME.PORT, PRIME.ADDR";
  String qry_getCSClist = "select CSC.ENDCAP, CSC.STATION, CSC.RING, CSC.NAME from CSC,MAP where CSC.ID=MAP.CSC order by CSC.STATION, CSC.RING, CSC.POSITION";

  private DefaultTreeModel FillMap(String topnode, String[] srvs) {
    DefaultMutableTreeNode top = new DefaultMutableTreeNode(topnode);
    DefaultTreeModel tree_model = new DefaultTreeModel(top);
    if (srvs.length > 0) {
      for (int i = 0; i < srvs.length; i++) {
        String srv = srvs[i];
        DefaultMutableTreeNode node = top;
        DefaultMutableTreeNode subnode;
        String[] orignames = srv.split(",");
        if (orignames.length >=7) {
          String[] names = new String[5];
          // Log.append(i + " " + srv + " " + names.length+"\n");
          names[0] = "Host:" + orignames[0];
          names[1] = "Primary:" + orignames[1] + ":" + orignames[2];
          names[2] = "Master#" + orignames[3];
          names[3] = "Ch" + orignames[4] + " RDB#" + orignames[5];
          names[4] = orignames[6];

          for (int j = 0; j < names.length; j++) {
            boolean nodeExists = false;
            int count = node.getChildCount();
            for (int k = 0; k < count; k++) {
              // Log.append(count + " " + node.getChildAt(k).toString()+ " " + names[j]+"\n");
              String nodename = node.getChildAt(k).toString();
              if (nodename.compareTo(names[j]) == 0) {
                nodeExists = true;
                node = (DefaultMutableTreeNode) node.getChildAt(k);
                break;
              }
            }
            if (!nodeExists) {
              subnode = new DefaultMutableTreeNode(names[j]);
              node.add(subnode);
              node = subnode;
            }
          }
        }
//            node.setUserObject(srvs[i]);
//            node.setUserObject(names[names.length-1]);
      }
    }
    return tree_model;
    // BrowserTree.setModel(tree_model);
  }

  private DefaultTreeModel FillCSCList(String topnode, String[] srvs) {
    DefaultMutableTreeNode top = new DefaultMutableTreeNode(topnode);
    DefaultTreeModel tree_model = new DefaultTreeModel(top);
    if (srvs.length == 0) {
      System.out.println("ERROR: Unable to get CSC Map list from server");
      return tree_model;
    }
    for (int i = 0; i < srvs.length; i++) {
      String srv = srvs[i];
      DefaultMutableTreeNode node = top;
      DefaultMutableTreeNode subnode;
      String[] names = srv.split(",");
      names[1] = "Station:" + names[1];
      names[2] = "Ring:" + names[2];
      // Log.append(i + " " + srv + " " + names.length+"\n");
      for (int j = 0; j < names.length; j++) {
        boolean nodeExists = false;
        int count = node.getChildCount();
        for (int k = 0; k < count; k++) {
          // Log.append(count + " " + node.getChildAt(k).toString()+ " " + names[j]+"\n");
          String nodename = node.getChildAt(k).toString();
          if (nodename.compareTo(names[j]) == 0) {
            nodeExists = true;
            node = (DefaultMutableTreeNode) node.getChildAt(k);
            break;
          }
        }
        if (!nodeExists) {
          subnode = new DefaultMutableTreeNode(names[j]);
          node.add(subnode);
          node = subnode;
        }
      }
      // node.setUserObject(srvs[i]);
      node.setUserObject(names[names.length - 1]);
    }
    return tree_model;
    // BrowserTree.setModel(tree_model);
  }

  private DefaultTreeModel FillRDBList(String topnode, String[] srvs) {
    DefaultMutableTreeNode top = new DefaultMutableTreeNode(topnode);
    DefaultTreeModel tree_model = new DefaultTreeModel(top);
    for (int i = 0; i < srvs.length; i++) {
      String srv = srvs[i];
      DefaultMutableTreeNode node = top;
      DefaultMutableTreeNode subnode;
      String[] names = srv.split(",");
      // System.out.println(i + " " + srv + " " + names.length+"\n");
      for (int j = 0; j < 2; j++) {
        boolean nodeExists = false;
        int count = node.getChildCount();
        for (int k = 0; k < count; k++) {
          // Log.append(count + " " + node.getChildAt(k).toString()+ " " + names[j]+"\n");
          String nodename = node.getChildAt(k).toString();
          if (nodename.compareTo(names[j]) == 0) {
            nodeExists = true;
            node = (DefaultMutableTreeNode) node.getChildAt(k);
            break;
          }
        }
        if (!nodeExists) {
          subnode = new DefaultMutableTreeNode(names[j]);
          node.add(subnode);
          node = subnode;
        }
      }
//            node.setUserObject(srvs[i]);
      // node.setUserObject(names[names.length-1]);
    }
    return tree_model;
    // BrowserTree.setModel(tree_model);
  }

  private DefaultTreeModel FillPrimeList(String topnode, String[] srvs) {
    DefaultMutableTreeNode top = new DefaultMutableTreeNode(topnode);
    DefaultTreeModel tree_model = new DefaultTreeModel(top);
    for (int i = 0; i < srvs.length; i++) {
      String srv = srvs[i];
      DefaultMutableTreeNode node = top;
      DefaultMutableTreeNode subnode;
      String[] names = srv.split(",");
      // Log.append(i + " " + srv + " " + names.length+"\n");
      for (int j = 1; j < names.length - 1; j++) {
        boolean nodeExists = false;
        int count = node.getChildCount();
        for (int k = 0; k < count; k++) {
          // Log.append(count + " " + node.getChildAt(k).toString()+ " " + names[j]+"\n");
          String nodename = node.getChildAt(k).toString();
          if (nodename.compareTo(names[j]) == 0) {
            nodeExists = true;
            node = (DefaultMutableTreeNode) node.getChildAt(k);
            break;
          }
        }
        if (!nodeExists) {
          subnode = new DefaultMutableTreeNode(names[j]);
          node.add(subnode);
          node = subnode;
        }
      }
//            node.setUserObject(srvs[i]);
      node.setUserObject(names[names.length - 1]);
    }
    return tree_model;
    // BrowserTree.setModel(tree_model);
  }

  public String[] getRDBlist() {
    sqlRpc.setData(qry_getRDBlist);
    String SQLRDBreply = sqlRpc.getString().replaceAll("'", "");
    return SQLRDBreply.split(";");
  }

  public String[] getMBlist() {
    sqlRpc.setData(qry_getMBlist);
    String SQLMBreply = sqlRpc.getString().replaceAll("'", "");
    return SQLMBreply.split(";");
  }

  public String[] getBoardslist() {
    sqlRpc.setData(qry_getRDBlist);
    String SQLRDBreply = sqlRpc.getString();
    sqlRpc.setData(qry_getMBlist);
    String SQLMBreply = sqlRpc.getString();
    String SQLreply = (SQLRDBreply + SQLMBreply).replaceAll("'", "");
    // System.out.println(SQLreply);
    return SQLreply.split(";");
  }

  public String[] getCSClist() {
    sqlRpc.setData(qry_getCSClist);
    String[] SQLreply = {
        ""};
    String reply = sqlRpc.getString();
    if ( (reply != null) && (reply.length() > 0)) {
      SQLreply = (reply.split(";"));
      for (int i = 0; i < SQLreply.length; i++) {
        SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'+'"), "ME+");
        SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'-'"), "ME-");
        SQLreply[i] = SQLreply[i].replaceAll("'", "");
      }
    }
    return SQLreply;
  }

  public String[] getMap() {
    sqlRpc.setData(qry_getMap);
    String[] SQLreply = (sqlRpc.getString()).split(";");
    for (int i = 0; i < SQLreply.length; i++) {
      SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'+'"), "ME+");
      SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'-'"), "ME-");
      SQLreply[i] = SQLreply[i].replaceAll("'", "");
    }
    return SQLreply;
  }

  public HVSystemBrowser() {
    super("", true, true, true, true);
    try {
      String[] services = DimBrowserNew.getServices(HVClient.config.ServerName +
          "*/HVDB");
      String service = HVClient.config.ServerName + "/HVDB";

      // exit if no matched services found
      if (services.length <= 0) {
        JOptionPane.showMessageDialog(this,
                                      "Unable to find services.",
                                      "Dim Error",
                                      JOptionPane.ERROR_MESSAGE);


//        System.out.println("ERROR: Unable to find services");
        return;
      }

      service = services[0];

      if (services.length > 1) {
        for (int i = 0; i < services.length; i++) {
          services[i] = services[i].replaceAll(HVClient.config.ServerName +
                                               "_HID", "");
          services[i] = services[i].replaceAll("/HVDB", "");
        }
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
            HostID = Integer.parseInt(s);
          }
          catch (NumberFormatException e) {
            System.out.println("Invalid HostID");
          }
          service = HVClient.config.ServerName + "_HID" + s + "/HVDB";
        }
      }

      sqlRpc = new DimRpcInfo(service);
      sqlRpc.setServiceIDMode(false);
      jbInit();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  public HVSystemBrowser(int hostid) {
    super("", true, true, true, true);
    HostID = hostid;
    String service = HVClient.config.ServerName + "_HID" + hostid + "/HVDB";
    try {
      sqlRpc = new DimRpcInfo(service);
      sqlRpc.setServiceIDMode(false);
      jbInit();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }

  }

  private void jbInit() throws Exception {
    setContentPane(jSplitPane1);
    this.addInternalFrameListener(new
                                  HVSystemBrowser_this_internalFrameAdapter(this));
    pack();
    setSize(600, 400);
    // createChambersList(top);
    CSCBrowser = new JTree(top);
    CSCBrowser.setModel(FillMap("HV System Map", getMap()));

    // CSCBrowser.setModel(FillRDBList("RDBs", getBoardslist()));
    // CSCBrowser.setModel(FillCSCList("CSCs", getCSClist()));
    CSCBrowser.addMouseListener(new HVSystemBrowser_CSCBrowser_mouseAdapter(this));
    jSplitPane2.setOrientation(JSplitPane.VERTICAL_SPLIT);
    jPanel1.setLayout(borderLayout2);
    SQLquery.addActionListener(new
                               HVSystemBrowser_jTextField1_actionAdapter(this));
    CSCBrowser.addTreeSelectionListener(new
                                        HVSystemBrowser_CSCBrowser_treeSelectionAdapter(this));
    jScrollPane2.getViewport().add(CSCBrowser);
    jSplitPane1.add(jScrollPane2, JSplitPane.LEFT);
    jSplitPane1.add(jSplitPane2, JSplitPane.RIGHT);
    jSplitPane2.add(jScrollPane1, JSplitPane.RIGHT);
    jSplitPane2.add(jPanel1, JSplitPane.LEFT);
    jPanel1.add(SQLquery, java.awt.BorderLayout.NORTH);
    jScrollPane1.getViewport().add(SQLresults);
    jSplitPane1.setDividerLocation(200);
    jSplitPane2.setDividerLocation(50);
    // setTitle("System Browser for );
    setTitle("System Browser for HID" + HostID); // DIM_DNS_NODE=" + DimClient.getDnsNode() +
//             ":" + DimClient.getDnsPort());

  }

  public void jTextField1_actionPerformed(ActionEvent e) {
    SQLresults.setText("");
    sqlRpc.setData(SQLquery.getText());
    String[] SQLreply = (sqlRpc.getString()).split(";");
    for (int i = 0; i < SQLreply.length; i++) {
      SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'+'"), "ME+");
      SQLreply[i] = SQLreply[i].replaceAll(Pattern.quote("'-'"), "ME-");
      SQLreply[i] = SQLreply[i].replaceAll("'", "");
    }
//    StringBuffer
    // CSCBrowser.setModel(FillCSCList("", SQLreply));
    // SQLresults.setText(SQLreply[0]);
    String res = "Query:\n" + SQLquery.getText() + "\nResults:\n";
    for (String s : SQLreply) res += s + "\n";
    SQLresults.setText(res);
  }

  public void CSCBrowser_valueChanged(TreeSelectionEvent e) {

  }

  public void this_internalFrameClosed(InternalFrameEvent e) {
    sqlRpc.finalize();
  }

  public void CSCBrowser_mouseClicked(MouseEvent e) {
    DefaultMutableTreeNode node = (DefaultMutableTreeNode)
        ( (JTree) e.getSource()).getLastSelectedPathComponent();

    if (node == null) {
      return;
    }

    Object nodeInfo = node.getUserObject();

    if (e.getClickCount() == 2) {
      String query = "";
      if (node.isLeaf()) {
        String CSC = nodeInfo.toString();
        // String board = nodeInfo.toString();
        SQLresults.setText(CSC + "\n");
        query = "select HVCARD.HOST from CSC, MAP, HVCARD, RDB where MAP.RDB=RDB.ID and HVCARD.ID=RDB.HOSTCARD and MAP.CSC=CSC.ID and CSC.NAME='" +
            CSC + "'";
        if (query.length() > 0) {
          sqlRpc.setData(query);
          String[] SQLreply = (sqlRpc.getString()).split(";");
          if (SQLreply.length > 0) {
            String[] csc_param = SQLreply[0].split(",");
            if (csc_param.length == 1) {
              OpenCSCFrame(new Integer(csc_param[0]).intValue(), CSC);
            }
          }
        }

      }
      else {
        String nodeName = nodeInfo.toString();
        SQLresults.setText(nodeName + "\n");

        if (nodeName.startsWith("Host")) {
          query = "";
          String param = nodeName.replaceFirst("Host:", "");
          if (param.length() > 1) {
            try {
              int hostid = Integer.parseInt(param);
              OpenModulesStatusWindow(hostid);
            }
            catch (NumberFormatException nfe) {
              System.out.println(
                  "Invalid HostID number.");
            }
          }
        }

        if (nodeName.startsWith("Primary")) {
          query = "";
          String[] param = nodeName.replaceFirst("Primary:", "").split(":");
          if (param.length == 2) {
            try {
              DefaultMutableTreeNode hostnode = node;
              while (!hostnode.getUserObject().toString().startsWith("Host") || hostnode.isRoot())
                hostnode = hostnode.getPreviousNode();

              int hostid = 0;
              if (hostnode.getUserObject().toString().startsWith("Host"))  {
                String hoststr = hostnode.getUserObject().toString().replaceFirst("Host:", "");
                if (hoststr.length() > 1)
                  hostid = Integer.parseInt(hoststr);
              }
              if (hostid > 0) {
                int port = Integer.parseInt(param[0]);
                int addr = Integer.parseInt(param[1]);
                OpenPrimaryPSFrame(hostid, port, addr);
              }
            }
            catch (NumberFormatException nfe) {
              System.out.println(
                  "Invalid Primary Port and GPIB Address number.");
            }
          }
        }

        if (nodeName.startsWith("Master#")) {
          query = "select HVCARD.HOST, HVCARD.DATASLOT, MASTERDB.ADDR from HVCARD, MASTERDB where HVCARD.ID=MASTERDB.HOSTCARD and MASTERDB.ID="
              + nodeName.replaceFirst("Master#", "");
          // System.out.println(nodeName + " " + query);
        }
        if (nodeName.matches("Ch\\d*\\sRDB#\\d*")) {
          query = "select HVCARD.HOST, HVCARD.DATASLOT, RDB.ADDR from HVCARD, RDB where HVCARD.ID=RDB.HOSTCARD and RDB.ID="
              + nodeName.replaceAll("Ch\\d*\\sRDB#", "");
          // System.out.println(nodeName + " " + query);
        }

        if (query.length() > 0) {
          sqlRpc.setData(query);
          String[] SQLreply = (sqlRpc.getString()).split(";");
          if (SQLreply.length > 0) {
            String[] csc_param = SQLreply[0].split(",");
            if (csc_param.length == 3) {
              OpenModuleFrame(new Integer(csc_param[0]).intValue(),
                              new Integer(csc_param[1]).intValue(),
                              new Integer(csc_param[2]).intValue());
            }
          }
        }

      }

    }

    /*
          if (node.isLeaf()) {
                String CSC = nodeInfo.toString();
                // String board = nodeInfo.toString();
                SQLresults.setText(CSC+"\n");
                if (e.getClickCount()== 2) {
                  // String query = "select HVCARD.HOST, HVCARD.DATASLOT, RDB.ADDR from HVCARD, RDB where HVCARD.ID=RDB.HOSTCARD and RDB.ID=" + board;

                  String query = "select HVCARD.HOST, HVCARD.DATASLOT, RDB.ADDR from CSC, MAP, HVCARD, RDB where MAP.RDB=RDB.ID and HVCARD.ID=RDB.HOSTCARD and MAP.CSC=CSC.ID and CSC.NAME='" +
                      CSC + "'";
                  sqlRpc.setData(query);
                  String[] SQLreply = (sqlRpc.getString()).split(";");
                  // System.out.println(sqlRpc.getString());
                  if (SQLreply.length > 0) {
                    // SQLresults.setText(SQLreply[0]);
                    String[] csc_param = SQLreply[0].split(",");
                    if (csc_param.length == 3) {
                      // SQLresults.setText(SQLreply[0]);
                      // JInternalFrame doc = new HVModuleFrame();
                      // JInternalFrame doc = new HVCSCFrame(CSC);
                      //System.out.println(CSC + " " + new Integer(csc_param[0]).toString() + " " +  new Integer(csc_param[1]).toString() + " " +
                      //    new Integer(csc_param[2]).toString());
     JInternalFrame doc = new HVModuleFrame(new Integer(csc_param[
                          0]).intValue(), new Integer(csc_param[1]).intValue(),
                          new Integer(csc_param[2]).intValue());

                      getDesktopPane().add(doc, MODULELAYER);
                      // desktop.add(doc, 5);
                      try {
                        doc.setVisible(true);
                        doc.setSelected(true);
                      }
                      catch (java.beans.PropertyVetoException e2) {}

                    }
                  }
                }
                // Log.append();
            } else {
              String nodeName = nodeInfo.toString();
              SQLresults.setText(nodeName+"\n");
              if (e.getClickCount()== 2 && nodeName.startsWith("Master#")) {
                String query = "select HVCARD.HOST, HVCARD.DATASLOT, MASTERDB.ADDR from HVCARD, MASTER where HVCARD.ID=MASTERDB.HOSTCARD and MASTERDB.ID='"
                    +nodeName.replaceFirst("Master#", "") + "'";
              }
            }
     */
  }

  public void OpenModulesStatusWindow(int hostid) {
  if (isWindowExist(getDesktopPane(), "Modules Status for HID" + hostid)) return;

  JInternalFrame frameModuleStatus = new HVModuleStatusFrame(hostid);
  getDesktopPane().add(frameModuleStatus, MODULELAYER);
  try {
    frameModuleStatus.setVisible(true);
    frameModuleStatus.setSelected(true);
  }
  catch (java.beans.PropertyVetoException e2) {}
}


  private void OpenCSCFrame(int hostid, String csc) {

    if (isWindowExist(getDesktopPane(), hostid+":"+csc))
      return;

    JInternalFrame doc = new HVCSCFrame(hostid, csc);

    getDesktopPane().add(doc, MODULELAYER);
    try {
      doc.setVisible(true);

      doc.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}

  }

  private void OpenPrimaryPSFrame(int hostid, int port, int addr) {
    if (isWindowExist(getDesktopPane(),
                      "PrimaryPS:" + hostid+":"+port+":"+addr))
      return;

    JInternalFrame doc = new HVPrimaryPSFrame(hostid, port, addr);

    getDesktopPane().add(doc, MODULELAYER);
    try {
      doc.setVisible(true);
      doc.setSelected(true);
    }
    catch (java.beans.PropertyVetoException e2) {}

  }

  private void OpenModuleFrame(int hostid, int card, int busaddr) {
    if (isWindowExist(getDesktopPane(), hostid+":"+card+":"+busaddr))
      return;

    JInternalFrame doc = new HVModuleFrame(hostid, card, busaddr);

    getDesktopPane().add(doc, MODULELAYER);
    try {
      doc.setVisible(true);
      doc.setSelected(true);
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

}

class HVSystemBrowser_CSCBrowser_treeSelectionAdapter
    implements TreeSelectionListener {
  private HVSystemBrowser adaptee;
  HVSystemBrowser_CSCBrowser_treeSelectionAdapter(HVSystemBrowser adaptee) {
    this.adaptee = adaptee;
  }

  public void valueChanged(TreeSelectionEvent e) {
    adaptee.CSCBrowser_valueChanged(e);
  }
}

class HVSystemBrowser_CSCBrowser_mouseAdapter
    extends MouseAdapter {
  private HVSystemBrowser adaptee;
  HVSystemBrowser_CSCBrowser_mouseAdapter(HVSystemBrowser adaptee) {
    this.adaptee = adaptee;
  }

  public void mouseClicked(MouseEvent e) {
    adaptee.CSCBrowser_mouseClicked(e);
  }
}

class HVSystemBrowser_this_internalFrameAdapter
    extends InternalFrameAdapter {
  private HVSystemBrowser adaptee;
  HVSystemBrowser_this_internalFrameAdapter(HVSystemBrowser adaptee) {
    this.adaptee = adaptee;
  }

  public void internalFrameClosed(InternalFrameEvent e) {
    adaptee.this_internalFrameClosed(e);
  }
}

class HVSystemBrowser_jTextField1_actionAdapter
    implements ActionListener {
  private HVSystemBrowser adaptee;
  HVSystemBrowser_jTextField1_actionAdapter(HVSystemBrowser adaptee) {
    this.adaptee = adaptee;
  }

  public void actionPerformed(ActionEvent e) {
    adaptee.jTextField1_actionPerformed(e);
  }
}
