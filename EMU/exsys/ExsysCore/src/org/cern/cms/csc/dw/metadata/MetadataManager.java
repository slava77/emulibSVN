package org.cern.cms.csc.dw.metadata;

import java.io.FileReader;
import java.io.InputStream;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;

@SuppressWarnings("unchecked")
public class MetadataManager {

    private static Logger logger = ExsysLogger.getLogger(MetadataManager.class);

    public static final String CLASS_ROOT                = "org.cern.cms.csc";
    public static final String RESOURCE_BASE             = "/org/cern/cms/csc/dw/metadata/";
    public static final String FACTS_RESOURCE            = "facts.properties";
    public static final String MAIN_ENTITIES_RESOURCE    = "main_entities.properties";
    public static final String MONITOR_ENTITIES_RESOURCE = "monitor_entities.properties";
    public static final String MONITOR_PACKAGE           = "org.cern.cms.csc.dw.model.monitor";

    private static Map<Class, FactMd> factClasses = null;

    public MetadataManager() {
        if (factClasses == null) {
            factClasses = new HashMap<Class, FactMd>();
            FileReader fr = null;
            try {
                Properties p = new Properties();
                InputStream in = MetadataManager.class.getResourceAsStream(FACTS_RESOURCE);
                if (in != null) {
                    p.load(in);
                    for (Object key: p.keySet()) {
                        String className = p.getProperty((String) key);
                        Class c = MetadataManager.class.getClassLoader().loadClass(className);
                        factClasses.put(c, new FactMd(c));
                    }
                }
            } catch (Exception ex) {
                logger.error(null, ex);
            }
        }
    }

    public final Collection<FactMd> getFactMDs() {
        return Collections.unmodifiableCollection(factClasses.values());
    }

    public final FactMd getFactMd(Class clazz) {
        return factClasses.get(clazz);
    }

}
