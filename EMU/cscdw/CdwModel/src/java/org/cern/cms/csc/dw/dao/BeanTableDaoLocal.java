package org.cern.cms.csc.dw.dao;

import org.cern.cms.csc.dw.dao.table.BeanTableIf;
import java.util.List;
import javax.ejb.Local;

@Local
public interface BeanTableDaoLocal {

    List getData(BeanTableIf table, boolean pageOnly);
    Long getDataCount(BeanTableIf table);

}
