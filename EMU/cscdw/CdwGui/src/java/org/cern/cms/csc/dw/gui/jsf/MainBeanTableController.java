package org.cern.cms.csc.dw.gui.jsf;

import org.cern.cms.csc.dw.gui.table.*;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Collection;
import java.util.Enumeration;
import java.util.Properties;
import java.util.SortedSet;
import java.util.TreeSet;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.model.base.EntityBase;

public class MainBeanTableController extends BeanTableControllerBase {

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/main_entities.properties";
    private static final SortedSet<SelectItem> entities = new TreeSet<SelectItem>(new SelectItemComparator());

    static {
        try {
            URL entitiesUrl = BeanTableControllerBase.class.getResource(ENTITIES_RESOURCE);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                SelectItem si = new SelectItem(entitiesPro.getProperty(k), k);
                entities.add(si);
            }
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        }
    }

    public MainBeanTableController() { }

    @Override
    public Collection<SelectItem> getEntities() {
        return entities;
    }

    @Override
    public BeanTableBase getBeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception {
        return new MainBeanTable(title, rowClass, true);
    }

}
