package hvclient;

import java.awt.*;
import javax.swing.*;
import javax.swing.JOptionPane;
import javax.swing.plaf.metal.*;
import java.net.*;
import java.io.*;
import java.util.*;
import java.security.*;

import dim.*;

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
public class HVClient {
  boolean packFrame = false;
  protected static String updateURL =
      "http://www.phys.ufl.edu/~barashko/HVClient/";
  private static String version = "1.2.21"; // (build from "+(new Date()).toString()+")";
  private static String clientName = "HVClient ver."+version;
  
  private static String passwd = "UV3GeYIFYcTz7Mq20IbiCPEYkTQ=";

  private static String input = "";
  private static int maxattempts = 3;
  // private static String[] app_args;
  boolean fAccepted = false;
  static HVClientConfig config = new HVClientConfig();
  static URLReader urlreader;

  public static String getVersion() {return version;}

  private static String encrypt(String x) throws Exception {
    try {
      MessageDigest d = null;
      d = MessageDigest.getInstance("SHA-1");
      d.reset();
      d.update(x.getBytes());
      sun.misc.BASE64Encoder encoder = new sun.misc.BASE64Encoder();
      String encoded=encoder.encode(d.digest());

      // return d.digest();
      return encoded;
    }
    catch (NoSuchAlgorithmException nsae) {
      System.out.println("No Such Encryption Algorithm");
    }
    return null;
  }


  /**
   * Construct and show the application.
   */
  public HVClient() {

    // System.out.println(app_args[0]);



    try {
      urlreader = new URLReader(new URL(updateURL + "VERSION.txt"));
      Vector<String> reply = urlreader.readURL();
      if (reply.size() > 0) {
        for (String line : reply) {
          if (line.startsWith("version:")) {
            System.out.println("Current Web Version info found - " + line);

            if (!isNewVersion(line)) {
//            if (line.startsWith("version:" + version)) {
              System.out.println("No updates required");
            }
            else {

              System.out.println("Available update from version:" + version +
                                 " to " + line);
              int action = JOptionPane.showConfirmDialog(null,
                  "Update HVClient from version:" + version + " to " + line +
                  "?", "New HVClient Update Available",
                  JOptionPane.YES_NO_OPTION, JOptionPane.INFORMATION_MESSAGE);
              if (action == 0) {
                autoUpdate(updateURL);
                System.exit(0);
              }
            }
          }
        }
      }
    }
    catch (Exception e) {

    }

    int count = 0;
    while (!fAccepted && (count < maxattempts)) {
      count++;
      JPasswordField pwd = new JPasswordField(10);
      int action = JOptionPane.showConfirmDialog(null, pwd,
                                                 clientName+": Enter Password",
                                                 JOptionPane.OK_CANCEL_OPTION);
      if (action > 0) {
        System.exit(0);
      }
      else {
        try {

          input = new String(pwd.getPassword());
          // String secret = new String(encrypt(input));
          String secret = encrypt(input);

          // == Uncomment below line to print encoded password string
          // == then cut'n'paste it to passwd variable

          // System.out.println(secret);

          if (secret.equals(passwd)) {
            fAccepted = true;
          }
          else {
            JOptionPane.showMessageDialog(null, "Wrong password", "Error",
                                          JOptionPane.ERROR_MESSAGE);
            fAccepted = false;
          }
        }
        catch (Exception e) {
          e.printStackTrace();
        }
      }
    }

    if (!fAccepted) {
      System.exit(0);
    }

    HVClientMainFrame frame = new HVClientMainFrame();
    // Validate frames that have preset sizes
    // Pack frames that have useful preferred size info, e.g. from their layout
    if (packFrame) {
      frame.pack();
    }
    else {
      frame.validate();
    }

    // Center the window
    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension frameSize = frame.getSize();
    if (frameSize.height > screenSize.height) {
      frameSize.height = screenSize.height;
    }
    if (frameSize.width > screenSize.width) {
      frameSize.width = screenSize.width;
    }
    frame.setLocation( (screenSize.width - frameSize.width) / 2,
                      (screenSize.height - frameSize.height) / 2);
    frame.setVisible(true);
  }

  public boolean isNewVersion(String ver) {
    String[] curver = version.split("\\.");
    String[] webver = ver.replaceAll("version:","").split("\\.");
    boolean fNewVer = false;
    boolean fSameVer = true;

    for (int i = 0; i < Math.min(curver.length, webver.length); i++) {
      try {
        if (Integer.parseInt(curver[i]) < Integer.parseInt(webver[i])) {
          fNewVer = true;
        }
        if (Integer.parseInt(curver[i]) != Integer.parseInt(webver[i])) {
          fSameVer = false;
        }
      } catch(NumberFormatException e) {
            System.out.println("Invalid version number");
      }
    }
    if ((curver.length<webver.length) && fSameVer) fNewVer=true;
    return fNewVer;
  }

  public static void autoUpdate(String updateURL) {
    try {
      Runtime rt = Runtime.getRuntime();
      String updater = "java -jar autoUpdater.jar";
      System.out.println("Download autoUpdater.jar");
      download(updateURL+"autoUpdater.jar");
      Process proc = rt.exec(updater);


      /*URLReader urlreader = new URLReader(new URL(updateURL + "VERSION.txt"));
      Vector<String>
          filelist = urlreader.readURL(new URL(updateURL + "FILES.txt"));
      for (String file : filelist) {
        download(updateURL + file);
      }
      JOptionPane.showMessageDialog(null, "Please Restart HVClient",
                                    "Restart HVClient",
                                    JOptionPane.INFORMATION_MESSAGE);
     */
    } catch (Exception e) {

    }

  }


  public static void download(String address, String localFileName) {
    OutputStream out = null;
    URLConnection conn = null;
    InputStream in = null;
    try {
      URL url = new URL(address);
      out = new BufferedOutputStream(
          new FileOutputStream(localFileName));
      conn = url.openConnection();
      in = conn.getInputStream();
      byte[] buffer = new byte[1024];
      int numRead;
      long numWritten = 0;
      while ( (numRead = in.read(buffer)) != -1) {
        out.write(buffer, 0, numRead);
        numWritten += numRead;
      }
      System.out.println(localFileName + "\t" + numWritten);
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
    finally {
      try {
        if (in != null) {
          in.close();
        }
        if (out != null) {
          out.close();
        }
      }
      catch (IOException ioe) {
      }
    }
  }

  public static void download(String address) {
    int lastSlashIndex = address.lastIndexOf('/');
    if (lastSlashIndex >= 0 &&
        lastSlashIndex < address.length() - 1) {
      download(address, address.substring(lastSlashIndex + 1));
    }
    else {
      System.err.println("Could not figure out local file name for " +
                         address);
    }
  }

  /**
   * Application entry point.
   *
   * @param args String[]
   */
  public static void main(String[] args) {
    // app_args = args;
    // System.out.println(app_args[0]);
    config.version = version;
//    config = new HVClientConfig();

    // String dnsnode = "ufcmspc02.cern.ch"; //= DimClient.getDnsNode();
    /* if (System.getenv("DIM_DNS_NODE") != null)
      config.dnsnode = DimClient.getDnsNode();
         config.dnsport = DimClient.getDnsPort();
     */
    if (args.length > 0) {
      String dns = args[0];
      // System.out.println(dns);
      String[] dnspair = dns.split(":");
      if (dnspair.length == 2) {
        config.dnsnode = dnspair[0];
        try {
          config.dnsport = Integer.parseInt(dnspair[1]);
        }
        catch (NumberFormatException e) {
          System.out.println("Invalid DIM DNS port number. " +
                             "Reverting to " + DimClient.getDnsPort());
          config.dnsport = DimClient.getDnsPort();
        }
      }
      else {
        config.dnsnode = dnspair[0];
      }
    }
    DimClient.setDnsNode(config.dnsnode, config.dnsport);
    System.out.println("DIM_DNS_NODE=" + DimClient.getDnsNode() +
                       ":" + DimClient.getDnsPort());

    config.dns_history.add(config.dnsnode + ":" + config.dnsport);
    DimErrorHandler eid = new DimErrorHandler() {
      public void errorHandler(int severity, int code, String msg) {
        System.out.println("Error" + code + ": " + msg + " sev: " +
                           severity);
      }
    };
    DimClient.addErrorHandler(eid);

    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        UIManager.put("swing.boldMetal", Boolean.FALSE);
        JDialog.setDefaultLookAndFeelDecorated(true);
        JFrame.setDefaultLookAndFeelDecorated(true);
        Toolkit.getDefaultToolkit().setDynamicLayout(true);
        System.setProperty("sun.awt.noerasebackground", "true");
        try {
          UIManager.setLookAndFeel(new MetalLookAndFeel());
        }
        catch (UnsupportedLookAndFeelException e) {
          System.out.println(
              "Metal Look & Feel not supported on this platform.");
        }
        catch (Exception exception) {
          exception.printStackTrace();
        }

        // String[] srvcs = DimBrowser.getServices("HV*/HV_PRIMARY_DATA");
       // String[] srvcs = DimBrowser.getServices("HV*/HV_CSC_CMD");
      // String[] srvcs = DimBrowser.getServices("HV*/HV_REAL_DATA");
     // String[] srvcs = DimBrowser.getServices("HV*/HV_CSC_DATA");

    /*
            for (int i = 0; i < srvcs.length; i++) {
              System.out.println(srvcs[i]);
            }
     */

    new HVClient();
      }
    });
  }
}
