package org.cern.cms.csc.dw.olap.web;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;
import javax.servlet.http.HttpSessionEvent;
import javax.servlet.http.HttpSessionListener;

public class ConfigListener implements ServletContextListener, HttpSessionListener {

    public void contextInitialized(ServletContextEvent sce) {
        try {
            ConfigManager.init(sce.getServletContext().getRealPath("."));
        } catch (Exception ex) {
            Logger.getLogger(ConfigListener.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void contextDestroyed(ServletContextEvent sce) {
    }

    public void sessionCreated(HttpSessionEvent se) {
    }

    public void sessionDestroyed(HttpSessionEvent se) {
    }
    
}