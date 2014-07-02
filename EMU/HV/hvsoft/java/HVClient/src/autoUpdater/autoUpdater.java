package autoUpdater;


import java.io.InputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import javax.swing.JOptionPane;
import java.io.OutputStream;
import java.net.URL;
import java.util.Vector;
import java.net.URLConnection;
import hvclient.URLReader;
import java.io.FileOutputStream;

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
public class autoUpdater {
  protected static String updateURL =
      "http://www.phys.ufl.edu/~barashko/HVClient/";
  public autoUpdater() {
  }

  public static void main(String[] args) {

    autoUpdater autoupdater = new autoUpdater();
    if (args.length > 0) {
      updateURL = args[0];
    }
    System.out.println("Starting AutoUpdate from " + updateURL + " in 3 sec");
    try {
       Thread.currentThread().sleep(3000);
       }
     catch (InterruptedException e) {
       e.printStackTrace();
       }

    autoUpdate(updateURL);


  }

  public static void autoUpdate(String updateURL) {
    try {
      URLReader urlreader = new URLReader(new URL(updateURL + "VERSION.txt"));
      Vector<String>
          filelist = urlreader.readURL(new URL(updateURL + "FILES.txt"));
      for (String file : filelist) {
        download(updateURL + file);
      }
      JOptionPane.showMessageDialog(null, "Please Restart HVClient",
                                    "Restart HVClient",
                                    JOptionPane.INFORMATION_MESSAGE);
    }
    catch (Exception e) {

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

}
