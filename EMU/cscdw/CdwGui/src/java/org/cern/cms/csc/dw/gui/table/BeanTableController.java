package org.cern.cms.csc.dw.gui.table;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Collection;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.SortedSet;
import java.util.TreeSet;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.gui.jsf.JsfBeanBase;
import org.cern.cms.csc.dw.model.base.EntityBase;

public class BeanTableController extends JsfBeanBase {

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/entities.properties";
    private static final SortedSet<SelectItem> entities = new TreeSet<SelectItem>(new SelectItemComparator());

    static {
        try {
            URL entitiesUrl = BeanTableController.class.getResource(ENTITIES_RESOURCE);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                SelectItem si = new SelectItem(entitiesPro.getProperty(k), k);
                entities.add(si);
            }
        } catch (IOException ex) {
            Logger.getLogger(BeanTableController.class.getName()).log(Level.SEVERE, null, ex);
        }

    }
    
    private boolean displayFilter = false;
    private boolean showSelectColumns = false;
    private boolean showSortByColumns = false;

    private Map<String, BeanTable> tables = new HashMap<String, BeanTable>();
    private String tableRowClassName = null;

    public BeanTableController() {
    }

    public BeanTable getTable() {
        if (isTableSet()) {
            return tables.get(tableRowClassName);
        }
        return null;
    }

    public Collection<SelectItem> getEntities() {
        return entities;
    }

    public String getTableRowClassName() {
        return tableRowClassName;
    }

    public void setTableRowClassName(String tableRowClassName) throws Exception {
        if (!tables.containsKey(tableRowClassName)) {
            Class<? extends EntityBase> tableRowClass = (Class<? extends EntityBase>) Class.forName(tableRowClassName);
            for (SelectItem si: entities) {
                if (si.getValue().equals(tableRowClassName)) {
                    tables.put(tableRowClassName, new BeanTable(si.getLabel(), tableRowClass));
                    this.tableRowClassName = tableRowClassName;
                    return;
                }
            }
        }
    }

    public boolean isTableSet() {
        return (tableRowClassName != null);
    }

    public void setDisplayFilter(boolean displayFilter) {
        this.displayFilter = displayFilter;
    }

    public boolean isDisplayFilter() {
        return this.displayFilter;
    }

    public boolean getShowSelectColumns() {
        return showSelectColumns;
    }

    public void setShowSelectColumns(boolean showSelectColumns) {
        this.showSelectColumns = showSelectColumns;
    }

    public boolean isShowSortByColumns() {
        return showSortByColumns;
    }

    public void setShowSortByColumns(boolean showSortByColumns) {
        this.showSortByColumns = showSortByColumns;
    }

    private static final int fastStep = 5;
    private static final int pageSizeMin = 5;
    private static final int pageSizeMax = 100;
    private static final int pageSizeStep = 5;

    public static int getFastStep() {
        return fastStep;
    }

    public static int getPageSizeMax() {
        return pageSizeMax;
    }

    public static int getPageSizeMin() {
        return pageSizeMin;
    }

    public static int getPageSizeStep() {
        return pageSizeStep;
    }

}
