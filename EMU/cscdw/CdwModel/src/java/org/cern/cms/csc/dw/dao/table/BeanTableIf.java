package org.cern.cms.csc.dw.dao.table;

import java.util.List;
import java.util.Map;

public interface BeanTableIf {

    int getPageIndex();
    int getPageSize();
    boolean isInteractiveMode();
    List<BeanTableSortColumnIf> getSortByColumns();
    Class getRowClass();
    Map<String, BeanTableFilterIf> getFilter();
    boolean isFilterOn();
    boolean isAdvancedQuerySet();
    String getAdvancedQuery();

}
