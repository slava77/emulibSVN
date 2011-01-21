package org.cern.cms.csc.exsys.gui.component.table;

import org.cern.cms.csc.exsys.gui.base.JsfBeanBase;

public abstract class BeanTableControllerBase extends JsfBeanBase {
/*
    private boolean displayFilter = false;
    private boolean showSelectColumns = false;
    private boolean showSortColumns = false;

    private String tableRowClassName = null;
    private Map<String, BeanTable> topTables = new HashMap<String, BeanTable>();
    private List<BeanTable> tables = new Stack<BeanTable>();

    public BeanTableControllerBase() { }

    public abstract Collection<SelectItem> getEntities();
    public abstract BeanTable getBeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception;

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

    public BeanTable getTable() {
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
                        BeanTable table = getBeanTable(si.getLabel(), tableRowClass);
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
        BeanTable table = getTable();
        BeanTableColumn column = table.getColumnByName(propertyName);
        if (column.getType().equals(BeanTableColumnIf.TYPE.ENTITY)) {
            BeanTableProjectionFilterItem filterItem = null;
            if (!column.isFilterSet()) {
                BeanTable filterTable = getBeanTable(
                        table.getTitle() + ":" + propertyName,
                        ((BeanTableColumn) column).getPropertyMd().getType());
                filterItem = new BeanTableProjectionFilterItem(filterTable);
                BeanTableFilter filter = new BeanTableFilter();
                filter.setString("dummy");
                filter.getItems().add(filterItem);
                column.setFilter(filter);
            } else {
                filterItem = (BeanTableProjectionFilterItem) column.getFilter().getItems().get(0);
            }
            pushTable(filterItem.getBeanTable());
        }
    }

    public boolean isTopTable() {
        return tables.size() == 1;
    }

    public void pushTable(BeanTable table) {
        tables.add(table);
    }

    public BeanTable popTable() {
        BeanTable table = null;
        if (isTableSet()) {
            table = tables.remove(tables.size() - 1);
        }
        return table;
    }

    public void backListener(ActionEvent ev) {
        popTable();
    }
*/
}
