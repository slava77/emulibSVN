package org.cern.cms.csc.exsys.gui.component.table;

import org.cern.cms.csc.exsys.gui.component.BeanTable;

public class BeanTableProjectionFilterItem extends BeanTableFilterItem {

    public BeanTableProjectionFilterItem(BeanTable value) {
        super(value);
        super.setOperation(BeanTableFilter.Operation.IN);
    }

    public BeanTable getBeanTable() {
        return (BeanTable) getValue();
    }
    
}
