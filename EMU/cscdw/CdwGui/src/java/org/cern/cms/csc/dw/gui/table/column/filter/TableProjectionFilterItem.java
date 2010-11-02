package org.cern.cms.csc.dw.gui.table.column.filter;

import org.cern.cms.csc.dw.gui.table.BeanTable;

public class TableProjectionFilterItem extends TableFilterItem {

    private String property;

    public TableProjectionFilterItem(String property, BeanTable value) {
        super(value);
        this.property = property;
        super.setOperation(TableFilter.Operation.IN);
    }

    public String getProperty() {
        return property;
    }

    public BeanTable getBeanTable() {
        return (BeanTable) getValue();
    }
}
