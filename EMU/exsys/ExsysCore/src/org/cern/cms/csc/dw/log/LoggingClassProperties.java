/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.log;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

/**
 * Provides chosen logging configuration properties lookup by closest match to a given class
 * E.g. if you create this object with propertyName = "email.level", it will handle all properties ending with .email.level
 * @author evka
 */
public class LoggingClassProperties {

    private static final Logger logger = SimpleLogger.getLogger(LoggingClassProperties.class);

    public static final String PROPS_RESOURCE_NAME = "/META-INF/exsys_logging.properties";
    private static final String GLOBAL_LOGGER_NAME = "*";

    private String propertyName;

    private String globalValue = null;
    private Map<String, String> props;

    /**
     * Constructor.
     * @param propertiesResourceName name of the resource where the properties file could be loaded from
     * @param propertyName name of the property that this object is going to handle e.g. level
     */
    public LoggingClassProperties(String propertyName) {
        this.propertyName = propertyName;
    }

    public String getBestMatchForClass(Class clazz) {
        Map<String, String> pp = getProps(clazz);
        String className = clazz.getCanonicalName();
        int bestMatchingLength = 0;
        String ret = null;
        for (Map.Entry<String, String> entry: pp.entrySet()) {
            if (className.startsWith(entry.getKey()) && (entry.getKey().length() > bestMatchingLength)) {
                bestMatchingLength = entry.getKey().length();
                ret = entry.getValue();
            }
        }

        if (ret == null) {
            return globalValue;
        }
        return ret;
    }

    /**
     * Lazily loads the needed properties from the properties file and returns them as a map.
     * All keys in this map have the propertyName suffix removed.
     * Also if global value for the given property is found, the globalValue field is set.
     */
    private Map<String, String> getProps(Class clazzToUseForLoading) {
        if (props == null) {
            try {
                props = new HashMap<String, String>();
                Properties allProps = new Properties();
                allProps.load(clazzToUseForLoading.getResourceAsStream(PROPS_RESOURCE_NAME));
                for (Map.Entry<Object, Object> entry: allProps.entrySet()) {
                    String key = (String) entry.getKey();
                    if (key.equals(GLOBAL_LOGGER_NAME + "." + propertyName)) {
                        globalValue = (String) entry.getValue();
                    }
                    int propNameIdx = key.lastIndexOf("." + propertyName);
                    if (propNameIdx >= 0) {
                        props.put(key.substring(0, propNameIdx), (String) entry.getValue());
                    }
                }
            } catch (Exception ex) {
                logger.error("Couldn't load logging properties!", ex);
            }
        }
        return props;
    }

}
