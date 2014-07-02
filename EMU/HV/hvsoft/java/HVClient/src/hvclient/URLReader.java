package hvclient;

import java.net.*;
import java.io.*;
import java.util.*;
import javax.swing.JOptionPane;

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



public class URLReader {
  String urlstring = "";
  URL url; // = new URL();

  public URLReader() {
    try {
      url = new URL(urlstring);
    }
    catch (Exception e) {

    }
  }

  public URLReader(URL newurl) {
    url = newurl;
  }

  public Vector<String> readURL() throws Exception {
    return readURL(url);
  }

  public Vector<String> readURL(URL url) throws Exception {

    Vector<String> res = new Vector(0);
    String inputLine;
    try {
      URLConnection con = url.openConnection();
      con.setConnectTimeout(3*1000);
      // System.out.println(con.getConnectTimeout());
      BufferedReader in = new BufferedReader(
          new InputStreamReader(
              con.getInputStream()));

      while ( (inputLine = in.readLine()) != null) {
        // System.out.println(inputLine);
        res.add(inputLine);
      }

      in.close();
    } catch (IOException e) {
      //JOptionPane.showMessageDialog(null, e.toString(), "Updater Exception",
      //                                    JOptionPane.ERROR_MESSAGE);
        System.out.println(e.toString());
    }
    return res;
  };

}
