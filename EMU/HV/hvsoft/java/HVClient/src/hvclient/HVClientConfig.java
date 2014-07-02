package hvclient;

import java.io.*;
import java.util.*;

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
public class HVClientConfig
    implements Serializable {

  String cfgname = "hvclient.cfg";
  String dnsnode = "localhost";
  String ServerName = "HV_DIM_SERVER";
  String version = "1.0";
  int dnsport = 2505;
  SortedSet dns_history= new TreeSet();
  HashSet cmd_history= new HashSet();
  // Vector cmd_history= new Vector();
  int history_depth = 20;
  HVMonitoringThresholds csc_mon_thresholds = new HVMonitoringThresholds();

  public HVClientConfig() {
    dns_history.add(dnsnode+":"+dnsport);
    LoadConfig();
  }

  public HVClientConfig(String filename) {
    dns_history.add(dnsnode+":"+dnsport);
    LoadConfig(filename);
  }

  public int LoadConfig(String filename) {
    int res = 0;
    try {
      FileInputStream file = new FileInputStream(filename);
      ObjectInputStream input = new ObjectInputStream(file);
      HVClientConfig cfg = (HVClientConfig) input.readObject();
      cfgname = cfg.cfgname;
      dnsnode = cfg.dnsnode;
      dnsport = cfg.dnsport;
      version = cfg.version;
      dns_history = cfg.dns_history;
      cmd_history = cfg.cmd_history;
      history_depth = cfg.history_depth;
      csc_mon_thresholds = cfg.csc_mon_thresholds;
      input.close();
      System.out.println("Configuration loaded from " + filename);

      res = 1;
    }
    catch (java.io.IOException IOE) {
      System.out.println(IOE.toString());
//      IOE.printStackTrace();
      // labelOutput.setText("IOException");
    }
    catch (ClassNotFoundException cnfe) {
      // cnfe.printStackTrace();
      System.out.println("ClassNotFoundException");
      // labelOutput.setText("ClassNotFoundException");
    }
    return res;
  }

  public int LoadConfig() {
     return LoadConfig(cfgname);
  }

  public int SaveConfig(String filename) {
    int res = 0;
    try {
      FileOutputStream file = new FileOutputStream(filename);
      ObjectOutputStream out = new ObjectOutputStream(file);
      dns_history.add(dnsnode+":"+dnsport);
      while (cmd_history.size() > history_depth) {
          cmd_history.remove(cmd_history.iterator().next());
      }
      out.writeObject(this);
      out.flush();
      out.close();
      System.out.println("Configuration saved to " + filename);
      res = 1;
    }
    catch (java.io.IOException IOE) {
      System.out.println(IOE.toString());
      // IOE.printStackTrace();
      // labelOutput.setText("IOException");
    }
    return 0;
  }

  public int SaveConfig() {
    return SaveConfig(cfgname);
  }
}
