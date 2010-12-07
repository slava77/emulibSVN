package org.cern.cms.csc.dw.metadata;

import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.Modifier;
import java.util.Collection;
import java.util.Collections;
import org.cern.cms.csc.dw.util.ClassUtil;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import javax.persistence.Entity;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.model.annotation.FactAnn;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;

@SuppressWarnings("unchecked")
public class MetadataManager {

    private static Logger logger = Logger.getLogger(MetadataManager.class);

    private static final String CLASS_ROOT                = "org.cern.cms.csc";
    private static final String RESOURCE_BASE             = "/org/cern/cms/csc/dw/metadata/";
    private static final String FACTS_RESOURCE            = "facts.properties";
    private static final String MAIN_ENTITIES_RESOURCE    = "main_entities.properties";
    private static final String MONITOR_ENTITIES_RESOURCE = "monitor_entities.properties";
    private static final String MONITOR_PACKAGE           = "org.cern.cms.csc.dw.model.monitor";

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

    public static void main(String[] args) {

        try {
            
            if (args.length < 1) {
                throw new IllegalArgumentException("Please supply base directory to store property file");
            }

            String baseDir = args[0];

            ClassList factList = new ClassList(baseDir, FACTS_RESOURCE);
            ClassList mainEntitiesList = new ClassList(baseDir, MAIN_ENTITIES_RESOURCE);
            ClassList monitorEntitiesList = new ClassList(baseDir, MONITOR_ENTITIES_RESOURCE);

            for (Class c : ClassUtil.packageClassses(CLASS_ROOT, true)) {
                if (ClassUtil.hasAsSuperClass(c, EntityBase.class) &&
                    c.isAnnotationPresent(Entity.class) &&
                    !Modifier.isAbstract(c.getModifiers())) {

                    if (c.getSuperclass().equals(Fact.class) && c.isAnnotationPresent(FactAnn.class)) {
                        factList.add(c.getSimpleName(), c.getName());
                    }

                    if (c.getPackage().getName().equals(MONITOR_PACKAGE)) {
                        monitorEntitiesList.add(c.getSimpleName(), c.getName());
                    } else {
                        mainEntitiesList.add(c.getSimpleName(), c.getName());
                    }
                    
                }
            }

            factList.store();
            mainEntitiesList.store();
            monitorEntitiesList.store();


        } catch (IOException ex) {
            logger.error(null, ex);
        }

    }

    public static class ClassList {

        private String fullFileName;
        private Properties properties;

        public ClassList(String baseDir, String fileName) {
            this.fullFileName = baseDir + RESOURCE_BASE + fileName;
            this.properties = new Properties();
        }

        public void add(String key, String value) {
            properties.setProperty(key, value);
        }

        public void store() throws IOException {
            File f = new File(this.fullFileName);
            FileWriter fw = new FileWriter(f);
            properties.store(fw, "List of classes\nGenerated by " + MetadataManager.class.getName());
            System.out.print(MetadataManager.class.getSimpleName());
            System.out.println(": " + String.valueOf(this.properties.size()) + " classes saved to " + this.fullFileName);

        }

    }

}
