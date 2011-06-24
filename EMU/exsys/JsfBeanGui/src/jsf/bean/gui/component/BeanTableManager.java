package jsf.bean.gui.component;

import com.icesoft.faces.component.ext.RowSelectorEvent;
import java.io.IOException;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import javax.faces.context.FacesContext;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.BeanTableFilterItem;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.component.table.BeanTableProperties;
import jsf.bean.gui.component.table.BeanTablePropertiesManager;
import jsf.bean.gui.component.table.export.BeanTableExportTemplateProvider;
import jsf.bean.gui.converter.ClassConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;

public abstract class BeanTableManager implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableManager.class);
    
    private BeanTablePack tablePack;
    private List<BeanTablePack> tables = new ArrayList<BeanTablePack>();
    private BeanTableProperties properties;
    private final String id;
    
    public abstract BeanTableDaoIf getBeanTableDao();
    public abstract ClassFinderIf getClassFinder();

    public BeanTableManager(String id, Class<? extends EntityBeanBase> rowClass) throws Exception {
        this.id = id;
        this.properties = new BeanTableProperties(BeanTablePropertiesManager.getProperties(id));
        this.tablePack = new BeanTablePack(this, rowClass);
    }

    public BeanTableProperties getProperties() {
        return properties;
    }

    public String getId() {
        return id;
    }

    public List<BeanTablePack> getTables() {
        return tables;
    }

    public BeanTablePack getTablePack() {
        return this.tablePack;
    }

    public BeanTable getTable() {
        return this.tablePack.getTable();
    }

    public BeanTablePack getTopPack() {
        if (!isTop()) {
            return getTables().get(0);
        } else {
            return getTablePack();
        }
    }

    public void pushTable(BeanTablePack nextTable) {
        this.tables.add(this.tablePack);
        this.tablePack = nextTable;
    }

    public void popTable() {
        if (!this.tables.isEmpty()) {
            this.tablePack = this.tables.remove(this.tables.size() - 1);
        }
    }

    public boolean isTop() {
        return this.tables.isEmpty();
    }

    public int getDepth() {
        return this.tables.size();
    }

    public void backActionListener() {
        popTable();
        this.getTable().refresh();
    }

    public void topActionListener() {
        while (!isTop()) {
            popTable();
        }
        this.getTable().refresh();
    }

    public void toTableActionListener() {
        FacesContext context = FacesContext.getCurrentInstance();
        Map map = context.getExternalContext().getRequestParameterMap();
        int tableIndex = Integer.valueOf((String) map.get("tableIndex"));
        while (this.tables.size() != tableIndex) {
            tablePack = tables.get(tables.size() - 1);
            this.popTable();
        }
    }

    public String buildTablePrefix(String nextPrefix) {
        String tablePrefix = null;
        if (nextPrefix != null) {
            if (this.tablePack != null && getTable() != null) {
                tablePrefix = getTable().getProperties().getTablePrefix();
            }
            tablePrefix = (tablePrefix != null ? tablePrefix.concat(nextPrefix) : nextPrefix).concat(".");
        }
        return tablePrefix;
    }

    /*********************************************
     *
     * Export manager
     *
     *********************************************/
    /**
     * Override for custom template provider
     * @return
     */
    public BeanTableExportTemplateProvider getTemplateProvider() {
        return new BeanTableExportTemplateProvider();
    }

    /*********************************************
     *
     * Row selection manager
     *
     *********************************************/
    
    private EntityBeanBase selected;

    public void setSelectedRowById(Object idValue) {
        Iterator<EntityBeanBase> it = this.getTable().getData().iterator();
        this.selected = null;
        while (it.hasNext()) {
            EntityBeanBase row = it.next();
            if (row.getEntityId().equals(idValue)) {
                this.selected = row;
                this.selected.setSelected(true);
                break;
            }
        }
    }

    public void rowSelectionListener(RowSelectorEvent event) {

        if (event.isSelected()) {
            Iterator<EntityBeanBase> it = this.getTable().getData().iterator();
            while (it.hasNext()) {
                EntityBeanBase row = it.next();
                if (row.getSelected()) {
                    this.selected = row;
                    break;
                }
            }
        } else {
            this.selected = null;
        }
    }

    public final EntityBeanBase getSelected() {
        return selected;
    }

    public final void setSelected(EntityBeanBase selected) {
        this.selected = selected;
    }

    public final void clearSelected() {
        if (this.selected != null) {
            this.selected = null;
        }
    }
    
    /*********************************************
     *
     * Table properties
     *
     *********************************************/
    
    public void savePersonalPropertiesListener() throws IOException {
        BeanTablePropertiesManager.saveProperties(id, properties, false);
    }

    public void saveGlobalPropertiesListener() throws IOException {
        BeanTablePropertiesManager.saveProperties(id, properties, true);
    }

    /*********************************************
     *
     * Table filters
     *
     *********************************************/
    
    public void addPropertyFilter(String property, BeanTableFilter filter) {
        getTopPack().getPropertyFilters().put(property, filter);
    }

    public void addPropertyFilter(String property, BeanTableFilter.Operation operation, Object value) {
        BeanTableFilter filter = new BeanTableFilter();
        BeanTableFilterItem filterItem = new BeanTableFilterItem();
        filterItem.setOperation(operation);
        filterItem.setValue(value);
        filter.getItems().add(filterItem);
        addPropertyFilter(property, filter);
    }

    public void removePropertyFilter(String property) {
        if (getTopPack().getPropertyFilters().containsKey(property)) {
            getTopPack().getPropertyFilters().remove(property);
        }
    }

    public void removeAllPropertyFilters() {
        getTopPack().getPropertyFilters().clear();
    }

    public ClassConverter getClassConverter() {
        return new ClassConverter(getClassFinder());
    }
    
}
