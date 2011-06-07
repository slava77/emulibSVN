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
import org.json.JSONException;
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

    public BeanTablePack(BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {
        this(null, null, manager, rowClass);
    }

    public BeanTablePack(String nextPrefix, String title, BeanTableManager manager, Class<? extends EntityBeanBase> rowClass) throws Exception {

        this.prefix = manager.buildTablePrefix(nextPrefix);
        this.title = title;
        this.manager = manager;
        this.rowClass = rowClass;

        Class firstClass = null;
        if (Modifier.isAbstract(rowClass.getModifiers())) {
            for (Class cl : manager.getClassFinder().findSubclassesInSamePackage(rowClass)) {
                if (!Modifier.isAbstract(cl.getModifiers())) {
                    if (firstClass == null) {
                        firstClass = cl;
                    }
                    this.classes.add(new SelectItem(cl, cl.getSimpleName()));
                }
            }
            this.setSelectedClass(firstClass);
        } else {
            this.setSelectedClass(rowClass);
        }
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
            BeanTableProperties props = new BeanTableProperties(getManager().getProperties());
            if (classes.isEmpty()) {
                props.setTablePrefix(prefix);
            } else {
                String p = "{".concat(selectedClass.getSimpleName()).concat("}.");
                if (prefix != null) {
                    p = prefix.concat(p);
                }
                props.setTablePrefix(p);
            }
            this.table = new BeanTable(props, this, selectedClass);
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

    public JSONObject getSerializedFilter() throws JSONException {
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
                            BeanTableColumnEntity ec = (BeanTableColumnEntity) column;
                            childJson.put(column.getName(), ec.getFilterTablePack().getSerializedFilter());
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
        return parentJson;
    }

    public void setSerializedFilter(String filter) throws JSONException, Exception {
        JSONObject json = new JSONObject(filter);

        if (json.has("filter")) {
            JSONObject filterJson = json.getJSONObject("filter");
            for (BeanTableColumn column : getTable().getColumns()) {
                if (filterJson.has(column.getName())) {
                    if (filterJson.optJSONObject(column.getName()) != null) {
                        JSONObject columnFilterJson = filterJson.getJSONObject(column.getName());
                        if (columnFilterJson.has("rowClass")) {
                            BeanTableColumnEntity ec = (BeanTableColumnEntity) column;
                            ec.getFilterTablePack().setSerializedFilter(columnFilterJson.getString(ec.getName()));
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


    }
}
