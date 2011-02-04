package org.cern.cms.csc.dw.dao.table;

import java.util.List;
import org.cern.cms.csc.dw.model.base.EntityBase;

public interface BeanTableIf {

    DualList<BeanTableColumnIf> getSortingColumns();
    int getPageSize();
    int getPageIndex();
    Class<? extends EntityBase> getRowClass();
    List<BeanTableColumnIf> getColumns();
    boolean isFilterOn();

}
