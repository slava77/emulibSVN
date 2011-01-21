package org.cern.cms.csc.exsys.gui.component.table;

import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;

public class BeanTableFilterItem implements BeanTableFilterItemIf {

    private Object value;
    private BeanTableFilter.Operator operator = BeanTableFilter.Operator.OR;
    private BeanTableFilter.Operation operation = BeanTableFilter.Operation.EQUAL;

    public BeanTableFilterItem() {
    }

    public BeanTableFilterItem(Object value) {
        this.value = value;
    }

    @Override
    public BeanTableFilter.Operator getOperator() {
        return this.operator;
    }

    public void setOperator(BeanTableFilter.Operator operator) {
        this.operator = operator;
    }

    @Override
    public BeanTableFilter.Operation getOperation() {
        return this.operation;
    }

    public void setOperation(BeanTableFilter.Operation operation) {
        this.operation = operation;
    }

    @Override
    public Object getValue() {
        return value;
    }

    public void setValue(Object value) {
        this.value = value;
    }

}
