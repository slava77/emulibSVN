package org.cern.cms.csc.dw.olap.web;

import javax.servlet.ServletContextEvent;
import javax.servlet.ServletContextListener;
import javax.servlet.http.HttpSessionEvent;
import javax.servlet.http.HttpSessionListener;
import org.apache.log4j.Logger;

public class ConfigListener implements ServletContextListener, HttpSessionListener {

    private static final Logger logger = Logger.getLogger(ConfigListener.class);

    public void contextInitialized(ServletContextEvent sce) {
        try {
            ConfigManager.init(sce.getServletContext().getRealPath("."));
        } catch (Exception ex) {
            logger.error(null, ex);
        }
    }

    public void contextDestroyed(ServletContextEvent sce) {
    }

    public void sessionCreated(HttpSessionEvent se) {
    }

    public void sessionDestroyed(HttpSessionEvent se) {
    }
    
}