package org.cern.cms.csc.dw.dao.table;

import java.util.List;

public interface BeanTableIf {

    Class getRowClass();
    int getPageIndex();
    int getPageSize();

    boolean isInteractiveMode();
    boolean isFilterOn();
    boolean isAdvancedQuerySet();

    String getAdvancedQuery();
    List<BeanTableColumnIf> getSortColumns();
    List<BeanTableColumnIf> getAllColumns();

}
