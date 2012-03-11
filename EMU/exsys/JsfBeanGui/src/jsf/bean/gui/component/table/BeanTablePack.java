package jsf.bean.gui.component.table;

import java.io.Serializable;
import java.lang.reflect.Modifier;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import javax.faces.model.SelectItem;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.column.BeanTableColumn;
import jsf.bean.gui.component.table.column.BeanTableColumnBase;
import jsf.bean.gui.component.table.column.BeanTableColumnEmbedded;
import jsf.bean.gui.component.table.column.BeanTableColumnEntity;
import jsf.bean.gui.component.table.column.BeanTableColumnEntityList;
import org.json.JSONObject;

public class BeanTablePack implements Serializable {

    private Collection<SelectItem> classes = new ArrayList<SelectItem>();
    private Class<? extends EntityBeanBase> selectedClass;
    private final Class<? extends EntityBeanBase> rowClass;
    private BeanTable table = null;
    private final BeanTableManager manager;
    private final String title;
    private final String prefix;
    private Collection<BeanTablePackFilter> filters;
    
    private Map<String, BeanTableFilter> propertyFilters = new HashMap<String, BeanTableFilter>();
    private String propertyQuery = null;

    public BeanTablePack(BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {
        this(null, null, manager, rowClass);
    }

    @SuppressWarnings(value="unchecked")
    public BeanTablePack(String nextPrefix, String title, BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {

        this.prefix = manager.buildTablePrefix(nextPrefix);
        this.title = title;
        this.manager = manager;
        this.rowClass = rowClass;
        
        Class tobeSelectedClass = null;
        if (Modifier.isAbstract(rowClass.getModifiers())) {
            for (Class cl : manager.getClassFinder().findSubclassesInSamePackage(rowClass)) {
                if (!Modifier.isAbstract(cl.getModifiers())) {
                    if (tobeSelectedClass == null) {
                        tobeSelectedClass = cl;
                    }
                    this.classes.add(new SelectItem(cl, cl.getSimpleName()));
                }
            }
        } else {
            tobeSelectedClass = rowClass;
        }
        
        boolean filterApplied = false;
        if (prefix == null) {
            if (manager.getProperties().getTableFilter() != null) {
                JSONObject tableFilter = new JSONObject(manager.getProperties().getTableFilter());
                if (tableFilter.has("filter")) {
                    setSerializedFilter(tableFilter);
                    filterApplied = true;
                }
            }
        }
        
        if (!filterApplied) {
            this.setSelectedClass(tobeSelectedClass);
        }
        
    }

    public Class<? extends EntityBeanBase> getRowClass() {
        return rowClass;
    }

    public Collection<SelectItem> getClasses() {
        return classes;
    }

    public Class<? extends EntityBeanBase> getSelectedClass() {
        return selectedClass;
    }

    public final void setSelectedClass(Class<? extends EntityBeanBase> selectedClass) throws Exception {
        if (selectedClass != null && (this.selectedClass == null || !this.selectedClass.equals(selectedClass))) {
            this.selectedClass = selectedClass;
            String newPrefix = null;
            if (classes.isEmpty()) {
                newPrefix = prefix;
            } else {
                newPrefix = "{".concat(selectedClass.getSimpleName()).concat("}.");
                if (prefix != null) {
                    newPrefix = prefix.concat(newPrefix);
                }
            }
            this.table = new BeanTable(getManager().getProperties().clone(newPrefix), this, selectedClass);
        }
    }

    public BeanTable getTable() {
        return table;
    }

    public String getTitle() {
        return title;
    }

    public Collection<BeanTablePackFilter> getFilters() {
        return filters;
    }

    public BeanTableManager getManager() {
        return manager;
    }

    public boolean isSingleClass() {
        return classes.isEmpty();
    }

    public void addFilter(BeanTablePackFilter btdf) {
        if (filters == null) {
            filters = new ArrayList<BeanTablePackFilter>();
        }
        filters.add(btdf);
    }

    public Map<String, BeanTableFilter> getPropertyFilters() {
        return propertyFilters;
    }

    public String getPropertyQuery() {
        return this.propertyQuery;
    }
    
    public void setPropertyQuery(String propertyQuery) {
        this.propertyQuery = propertyQuery;
    }
    
    public boolean isPropertyQuery() {
        return this.propertyQuery != null;
    }
    
    public JSONObject getSerializedFilter() throws Exception {
        
        JSONObject parentJson = new JSONObject();
        parentJson.put("rowClass", rowClass.getCanonicalName());
        if (!isSingleClass()) {
            parentJson.put("selectedClass", selectedClass.getCanonicalName());
        }
        
        JSONObject childJson = new JSONObject();
        if (getTable().isFilterOn()) {
            for (BeanTableColumn column : getTable().getColumns()) {
                if (column.isFilterSet()) {
                    if (column.isEmbedType()) {
                        BeanTableColumnEmbedded ec = (BeanTableColumnEmbedded) column;
                        JSONObject eo = new JSONObject();
                        for (BeanTableColumnBase property : ec.getProperties()) {
                            if (property.isFilterSet()) {
                                eo.put(property.getName(), property.getFilter().getString());
                            }
                        }
                        childJson.put(column.getName(), eo);
                    } else {
                        if (column.isEntityType()) {
                            childJson.put(column.getName(), ((BeanTableColumnEntity) column).getFilterTablePack().getSerializedFilter());
                        } else {
                            childJson.put(column.getName(), column.getFilter().getString());
                        }
                    }
                }
            }
        }
        
        if (childJson.length() != 0) {
            parentJson.put("filter", childJson);
        }
        
        if (getTable().isQueryApplied()) {
            parentJson.put("query", getTable().getAppliedQuery());
        }
        
        return parentJson;
    }

    @SuppressWarnings(value="unchecked")
    public final void setSerializedFilter(JSONObject filter) throws Exception {
        
        if (!isSingleClass()) {
            if (prefix == null && filter.has("selectedClass")) {
                Class<? extends EntityBeanBase> sc = 
                        (Class<? extends EntityBeanBase>) 
                        Class.forName(filter.getString("selectedClass"));
                setSelectedClass(sc);
            }
        }

        if (table == null) {
            if (isSingleClass()) {
                setSelectedClass(rowClass);
            } else {
                setSelectedClass((Class) classes.iterator().next().getValue());
            }
        }
        
        if (filter.has("filter")) {
            JSONObject filterJson = filter.getJSONObject("filter");
            for (BeanTableColumn column : getTable().getColumns()) {
                if (filterJson.has(column.getName())) {
                    if (filterJson.optJSONObject(column.getName()) != null) {
                        JSONObject columnFilterJson = filterJson.getJSONObject(column.getName());
                        if (columnFilterJson.has("rowClass")) {
                            if (column.isEntityType()) {
                                if (column.isListType()) {
                                    BeanTableColumnEntityList ce = (BeanTableColumnEntityList) column;
                                    ce.setFilterTablePack(this.prefix);
                                    ce.getFilterTablePack().setSerializedFilter(columnFilterJson);
                                } else {
                                    BeanTableColumnEntity ce = (BeanTableColumnEntity) column;
                                    ce.setFilterTablePack(this.prefix);
                                    ce.getFilterTablePack().setSerializedFilter(columnFilterJson);
                                }
                            }
                        } else {
                            BeanTableColumnEmbedded ec = (BeanTableColumnEmbedded) column;
                            for (BeanTableColumnBase properties : ec.getProperties()) {
                                if (columnFilterJson.has(properties.getName())) {
                                    String filterText = columnFilterJson.getString(properties.getName());
                                    properties.setFilter((BeanTableFilter) properties.getFilterConverter().getAsObject(null, null, filterText));
                                }
                            }
                        }
                    } else {
                        String filterText = filterJson.getString(column.getName());
                        column.setFilter((BeanTableFilter) column.getFilterConverter().getAsObject(null, null, filterText));
                    }
                }
            }
        }
        
        if (filter.has("query")) {
            getTable().setQuery(filter.getString("query"));
        }
        
    }
    
}
