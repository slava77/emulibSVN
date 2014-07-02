package hvclient;

import javax.swing.*;
import java.awt.*;
import java.awt.BorderLayout;
import javax.swing.event.InternalFrameEvent;
import javax.swing.event.InternalFrameAdapter;
import java.util.Map;
import java.util.HashMap;
import java.util.TimeZone;
import java.util.Calendar;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

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
public class HVServerLogsFrame
    extends JInternalFrame {

  JPanel mainPanel = new JPanel();
  JTabbedPane jTabbedPane1 = new JTabbedPane();
  protected Map<String, LogPanel> loggers = new HashMap<String, LogPanel> ();

  private int DEFAULT_WIDTH=600;
  private int DEFAULT_HEIGHT=200;
  private Timer timer;
  private int delay = 30 * 1000;

  public HVServerLogsFrame() {
    super("", true, true, true, true);
    create();
  }

  private void create() {
    try {

      jbInit();

      setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  public synchronized void relocate() {

    if (this.getDesktopPane() != null) {
      Dimension desktopSize = getDesktopPane().getSize();
      // System.out.println((desktopSize.width / 2));
      int width = desktopSize.width / 3;
      width = (width<DEFAULT_WIDTH)?DEFAULT_WIDTH:width;
      int height = desktopSize.height /4;
      height = (height <DEFAULT_HEIGHT)?DEFAULT_HEIGHT:height;
      setBounds(2*desktopSize.width / 3, 3 * desktopSize.height / 4,
                width, height);
    } else {
      System.out.println("Cant find desktop");
    }
  }

  private String[] getLogServices() {
    String[] services = DimBrowserNew.getServices(HVClient.config.ServerName +
                                                  "*/LOGGER");
    /*
         if (services.length <= 0) {
      JOptionPane.showMessageDialog(this,
                                    "Unable to find services.",
                                    "Dim Error",
                                    JOptionPane.ERROR_MESSAGE);
         }
     */

    return services;
  }

  private String getLogPageName(String service) {
    if (service.length() != 0) {
      service = service.replaceAll(HVClient.config.ServerName +
                                   "_HID", "");
      service = service.replaceAll("/LOGGER", "");
      service = "HostID " + service;
    }
    return service;

  }

  private void jbInit() throws Exception {
    setContentPane(mainPanel);

    this.addInternalFrameListener(new
                                  HVServerLogsFrame_this_internalFrameAdapter(this));
    pack();

    setSize(800, 400);

    mainPanel.setLayout(new BorderLayout());

    mainPanel.add(jTabbedPane1, java.awt.BorderLayout.CENTER);

    loggers.clear();
    addLogPanels();

    ActionListener taskPerformer = new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        addLogPanels();
      }
    };
    timer = new Timer(delay, taskPerformer);
    timer.start();

    setTitle("Server Logs");
  }

  private void addLogPanels() {
    String[] logs = getLogServices();
    if (logs.length > 0) {
      // loggers.clear();
      for (int i = 0; i < logs.length; i++) {
        String panelName = getLogPageName(logs[i]);
        if (!loggers.containsKey(panelName)) {
          LogPanel logPanel = new LogPanel(logs[i]);
          loggers.put(panelName, logPanel);
          jTabbedPane1.add(logPanel, panelName);
        }
      }
    }
  }

  public void this_internalFrameClosing(InternalFrameEvent e) {
    timer.stop();
    for (String entry : loggers.keySet()) {
      loggers.get(entry).finalize();
    }
    // rpcPrimeData.finalize();

  }

}

class HVServerLogsFrame_this_internalFrameAdapter
    extends InternalFrameAdapter {
  private HVServerLogsFrame adaptee;
  HVServerLogsFrame_this_internalFrameAdapter(HVServerLogsFrame adaptee) {
    this.adaptee = adaptee;
  }

  public void internalFrameClosing(InternalFrameEvent e) {
    adaptee.this_internalFrameClosing(e);
  }
}
