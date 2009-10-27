package org.cern.cms.csc.cdw.util;

import java.util.Properties;
import java.util.Map;
import java.util.HashMap;
import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import javax.naming.Context;
import javax.naming.InitialContext;
import java.util.regex.Pattern;
import java.util.regex.Matcher;

/**
 *
 * @author valdo
 */
public class WebUtil {

  private static Properties osenv = new Properties();
  private static Map<String, String> auths = new HashMap<String, String>();

  static {
    try {
      osenv.load(Runtime.getRuntime().exec("env").getInputStream());
    } catch (java.io.IOException e) {
      System.out.println("Can not get env!");
      e.printStackTrace(System.err);
    }
  }

  public static String GetEnv(String name) {
    try {
      Context initCtx = new InitialContext();
      return (String)initCtx.lookup("java:comp/env/" + name);
    } catch(Exception e) { }
    return null;
  }


 public static String getPassword(String username) throws Exception {

    if (auths.containsKey(username)) return auths.get(username);

    String home = (String) osenv.get("HOME");
    String auth_file = home + "/" + WebUtil.GetEnv("db_auth_file");
    String password = null;

    FileInputStream in = new FileInputStream(auth_file);
    BufferedReader br = new BufferedReader(new InputStreamReader(in));
    String line;
    Pattern p = Pattern.compile("^" + username + "/(.+)$");
    while ((line = br.readLine()) != null) {
      Matcher m = p.matcher(line);
      if (m.find()) {
        password = m.group(1);
        break;
      }
    }
    in.close();
    if (password != null && !password.equals("")) {
      auths.put(username, password);
    }

    return password;
  }

}