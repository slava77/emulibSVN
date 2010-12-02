package org.cern.cms.csc.dw.gui.table;

import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.gui.jsf.JsfBeanBase;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;
import org.cern.cms.csc.dw.gui.table.column.filter.TableProjectionFilterItem;
import org.cern.cms.csc.dw.model.base.EntityBase;

public abstract class BeanTableControllerBase extends JsfBeanBase {

    private boolean displayFilter = false;
    private boolean showSelectColumns = false;
    private boolean showSortColumns = false;

    private String tableRowClassName = null;
    private Map<String, BeanTableBase> topTables = new HashMap<String, BeanTableBase>();
    private List<BeanTableBase> tables = new Stack<BeanTableBase>();

    public BeanTableControllerBase() { }

    public abstract Collection<SelectItem> getEntities();
    public abstract BeanTableBase getBeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception;

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

    public boolean isShowSortColumns() {
        return showSortColumns;
    }

    public void setShowSortColumns(boolean showSortColumns) {
        this.showSortColumns = showSortColumns;
    }

    public boolean isTableSet() {
        return !tables.isEmpty();
    }

    public BeanTableBase getTable() {
        if (isTableSet()) {
            return tables.get(tables.size() - 1);
        }
        return null;
    }

    public String getTableRowClassName() {
        return tableRowClassName;
    }

    public void setTableRowClassName(String tableRowClassName) {
        this.tableRowClassName = tableRowClassName;
    }
    
    public void topTableListener(ActionEvent event) throws Exception {
        if (this.tableRowClassName != null) {
            if (!topTables.containsKey(this.tableRowClassName)) {
                Class<? extends EntityBase> tableRowClass = (Class<? extends EntityBase>) Class.forName(this.tableRowClassName);
                for (SelectItem si: getEntities()) {
                    if (si.getValue().equals(this.tableRowClassName)) {
                        BeanTableBase table = getBeanTable(si.getLabel(), tableRowClass);
                        topTables.put(this.tableRowClassName, table);
                    }
                }
            }

            if (topTables.containsKey(this.tableRowClassName)) {
                while (isTableSet()) popTable();
                pushTable(topTables.get(this.tableRowClassName));
            }
        }
    }

    public void setFilterTable(String propertyName) throws Exception {
        BeanTableBase table = getTable();
        BeanTableColumnIf column = table.getColumnByName(propertyName);
        if (column.getType().equals(BeanTableColumnIf.TYPE.ENTITY)) {
            TableProjectionFilterItem filterItem = null;
            if (!column.isFilterSet()) {
                BeanTableBase filterTable = getBeanTable(
                        table.getTitle() + ":" + propertyName,
                        ((BeanTableColumn) column).getPropertyMd().getType());
                filterItem = new TableProjectionFilterItem(filterTable);
                TableFilter filter = new TableFilter();
                filter.setString("dummy");
                filter.getItems().add(filterItem);
                column.setFilter(filter);
            } else {
                filterItem = (TableProjectionFilterItem) column.getFilter().getItems().get(0);
            }
            pushTable(filterItem.getBeanTable());
        }
    }

    public boolean isTopTable() {
        return tables.size() == 1;
    }

    public void pushTable(BeanTableBase table) {
        tables.add(table);
    }

    public BeanTableBase popTable() {
        BeanTableBase table = null;
        if (isTableSet()) {
            table = tables.remove(tables.size() - 1);
        }
        return table;
    }

    public void backListener(ActionEvent ev) {
        popTable();
    }

}
