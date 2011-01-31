package org.cern.cms.csc.dw.dao.table;

public interface BeanTableColumnIf {

    boolean isAscending();
    String getName();
    boolean isFilterSet();
    BeanTableFilterIf getFilter();
    void setFilter(BeanTableFilterIf filter);
    Class getType();

}
