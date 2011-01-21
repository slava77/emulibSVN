package org.cern.cms.csc.dw.dao.table;

import java.util.List;
import org.cern.cms.csc.dw.model.base.EntityBase;

public interface BeanTableDaoIf {

    List<EntityBase> getData(BeanTableIf table);
    Long getDataCount(BeanTableIf table);

}
