package org.cern.cms.csc.exsys.gui.component.table;

import org.cern.cms.csc.dw.dao.table.BeanTablePackIf;
import org.cern.cms.csc.dw.dao.table.BeanTableProjectionFilterItemIf;

public class BeanTableProjectionFilterItem extends BeanTableFilterItem implements BeanTableProjectionFilterItemIf {

    public BeanTableProjectionFilterItem(BeanTablePack value) {
        super(value);
        super.setOperation(BeanTableFilter.Operation.IN);
    }

    @Override
    public BeanTablePackIf getTablePack() {
        return (BeanTablePackIf) getValue();
    }
    
}
