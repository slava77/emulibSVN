package org.cern.cms.csc.cdw.util;

import org.hibernate.SessionFactory;
import org.hibernate.cfg.Configuration;
import java.util.Properties;
import java.io.FileInputStream;
import java.io.IOException;

/**
 * Hibernate Utility class with a convenient method to get Session Factory object.
 *
 * @author valdo
 */
public class HibernateUtil {

  private static final SessionFactory sessionFactory;

  static {
    try {

      String datasource = WebUtil.GetEnv("db_name");
      String username = WebUtil.GetEnv("db_username");

      String password = null;
      if (username != null) {
        password = WebUtil.getPassword(username);
      }

      Configuration config = new Configuration();

      try {
        Properties props = new Properties();
        props.load(new FileInputStream(props.getClass().getResource("hibernate.properties").getPath()));
        config.setProperties(props);
      } catch (IOException ex) { }

      if (datasource != null) {
        config.setProperty("hibernate.connection.datasource", "java:comp/env/" + datasource);
      }

      if (username != null && password != null) {
        config.setProperty("hibernate.connection.username", username);
        config.setProperty("hibernate.connection.password", password);
      }

      sessionFactory = config.configure().buildSessionFactory();

    } catch (Throwable ex) {
      System.err.println("Initial SessionFactory creation failed." + ex);
      throw new ExceptionInInitializerError(ex);
    }
  }

  public static SessionFactory getSessionFactory() {
    return sessionFactory;
  }
}
