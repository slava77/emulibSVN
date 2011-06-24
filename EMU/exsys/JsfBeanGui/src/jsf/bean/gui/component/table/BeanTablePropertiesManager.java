/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import jsf.bean.gui.JsfBeanBase;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 *
 * @author valdo
 */
public class BeanTablePropertiesManager {
    
    private static final Logger logger = SimpleLogger.getLogger(BeanTableManager.class);
    
    private static final String PROPERTIES_BASE_PATH = "resources/tables/";
    private static final String PROPERTIES_EXTENSION = ".properties";
    private static final String COOKIE_NAME_PATTERN = "table.%s.properties";
    
    // Caching default properties
    private static Map<String, Properties> beanProperties = new HashMap<String, Properties>();

    public static Properties getProperties(String tableId) {

        // Loading personal properties
        {
            // Get cookie for the table
            String cookieName = String.format(COOKIE_NAME_PATTERN, tableId);

            String myId = (String) JsfBeanBase.getCookie(cookieName);

            if (myId != null) {
                File f = JsfBeanBase.getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(myId).concat(PROPERTIES_EXTENSION));
                try {
                    FileInputStream fin = new FileInputStream(f);
                    Properties p = new Properties();
                    p.load(fin);
                    fin.close();
                    return p;
                } catch (Exception ex) {
                    logger.warn("Table [id = {0}] personal properties not found at [{1}]. Reseting and loading defaults...", tableId, f.getAbsolutePath());
                    JsfBeanBase.setCookie(cookieName, "", 0);
                }
            }
        }

        // Loading default properties
        {
            if (!beanProperties.containsKey(tableId)) {
                File f = JsfBeanBase.getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(tableId).concat(PROPERTIES_EXTENSION));
                Properties p = new Properties();
                try {
                    p.load(new FileInputStream(f));
                } catch (Exception ex) {
                    logger.warn("Table [id = {0}] properties not found at [{1}]. Loading defaults...", tableId, f.getAbsolutePath());
                }
                beanProperties.put(tableId, p);
            }
        }

        // Copy properties instead of using the same...
        Properties op = beanProperties.get(tableId);
        Properties p = new Properties();
        for (Enumeration keys = op.propertyNames(); keys.hasMoreElements();) {
            Object key = keys.nextElement();
            p.put(key, op.get(key));
        }

        return p;

    }

    public static void saveProperties(String tableId, BeanTableProperties properties, boolean global) throws IOException {

        String cookieName = String.format(COOKIE_NAME_PATTERN, tableId);

        // Writting a file
        Properties p = properties.getProperties();
        String filename = tableId;
        if (!global) {
            filename = (String) JsfBeanBase.getCookie(cookieName);
            if (filename == null) {
                File f = File.createTempFile(tableId.concat("."), "", JsfBeanBase.getRealFile(PROPERTIES_BASE_PATH));
                filename = f.getName();
                f.delete();
            }
        }
        p.store(new FileOutputStream(
                JsfBeanBase.getRealFile(PROPERTIES_BASE_PATH.concat(File.separator).concat(filename).concat(PROPERTIES_EXTENSION))), filename);

        // Saving filename in cookie
        if (!global) {
            JsfBeanBase.setCookie(cookieName, filename, 356 * 24 * 60 * 60);
        }

    }

}
