package org.cern.cms.csc.exsys.gui.base;

import java.io.File;
import java.io.FileInputStream;
import java.net.URL;
import java.util.Collection;
import java.util.Enumeration;
import java.util.Properties;
import java.util.SortedMap;
import java.util.TreeMap;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;
import org.cern.cms.csc.exsys.gui.component.BeanTableManager;
import org.cern.cms.csc.exsys.gui.util.ClassComparator;

public abstract class BrowserController extends JsfBeanBase {

    private static final Logger logger = SimpleLogger.getLogger(BrowserController.class);
    private SortedMap<Class, BeanTableManager> tables = new TreeMap<Class, BeanTableManager>(new ClassComparator());

    private Class currentClass = null;
    private BeanTableManager currentTable = null;

    protected abstract BeanTableManager createTable(Class forClass) throws Exception;
    
    public BrowserController(String pathToResource) {
        try {
            URL entitiesUrl = BrowserController.class.getResource(pathToResource);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                tables.put(Class.forName(entitiesPro.getProperty(k)), null);
            }
        } catch (Exception ex) {
            logger.error(ex);
        }
    }

    public Collection<Class> getClasses() {
        return tables.keySet();
    }

    public Class getCurrentClass() {
        return currentClass;
    }

    public void setCurrentClass(Class currentClass) {
        try {
            if (tables.containsKey(currentClass)) {
                if (tables.get(currentClass) == null)  {
                    tables.put(currentClass, createTable(currentClass));
                }
                this.currentClass = currentClass;
                this.currentTable = tables.get(currentClass);
            } else {
                logger.warn("Class %s was not found in BrowserController list...", currentClass);
            }
        } catch (Exception ex) {
            logger.error(ex);
            this.currentClass = null;
        }
    }

    public BeanTableManager getTable() {
        return currentTable;
    }

}
