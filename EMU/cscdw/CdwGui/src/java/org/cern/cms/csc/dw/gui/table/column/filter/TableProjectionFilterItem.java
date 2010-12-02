package org.cern.cms.csc.dw.gui.table.column.filter;

import org.cern.cms.csc.dw.dao.table.BeanTableProjectionFilterItemIf;
import org.cern.cms.csc.dw.gui.table.BeanTableBase;

public class TableProjectionFilterItem extends TableFilterItem implements BeanTableProjectionFilterItemIf {

    public TableProjectionFilterItem(BeanTableBase value) {
        super(value);
        super.setOperation(TableFilter.Operation.IN);
    }

    public BeanTableBase getBeanTable() {
        return (BeanTableBase) getValue();
    }
    
}
