/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.util;

import java.util.Properties;

/**
 *
 * @author evka
 */
public class PropertiesProvider {

    private static Properties exsysProps;

    public static String getProperty(String key) {
        String ret = getProperties().getProperty(key);
        if (ret == null) {
            throw new RuntimeException("Property \"" + key + "\" does not exist in ExsysModel/META-INF/exsys.properties file");
        }
        return ret;
    }

    private static Properties getProperties() {
        try {
            if (exsysProps == null) {
                exsysProps = new Properties();
                exsysProps.load(PropertiesProvider.class.getResourceAsStream("/META-INF/exsys.properties"));
            }
            return exsysProps;
        } catch (Exception ex) {
            throw new RuntimeException("Could not load ExsysModel/META-INF/exsys.properties file!");
        }
    }

}
