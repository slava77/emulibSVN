package org.cern.cms.csc.dw.dao.table;

public interface BeanTableFilterItemIf {

    BeanTableFilterIf.Operator getOperator();
    BeanTableFilterIf.Operation getOperation();
    Object getValue();

}
