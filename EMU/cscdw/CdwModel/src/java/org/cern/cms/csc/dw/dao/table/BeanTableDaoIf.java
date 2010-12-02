package org.cern.cms.csc.dw.dao.table;

import java.util.List;

public interface BeanTableDaoIf {

    List getData(BeanTableIf table, boolean pageOnly);
    Long getDataCount(BeanTableIf table);

}
