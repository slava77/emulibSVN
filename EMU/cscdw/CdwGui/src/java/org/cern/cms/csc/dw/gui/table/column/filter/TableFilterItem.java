package org.cern.cms.csc.dw.gui.table.column.filter;

import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;

public class TableFilterItem implements BeanTableFilterItemIf {

    private Object value;
    private TableFilter.Operator operator = TableFilter.Operator.OR;
    private TableFilter.Operation operation = TableFilter.Operation.EQUAL;

    public TableFilterItem() {
    }

    public TableFilterItem(Object value) {
        this.value = value;
    }

    public TableFilter.Operator getOperator() {
        return this.operator;
    }

    public void setOperator(TableFilter.Operator operator) {
        this.operator = operator;
    }

    public TableFilter.Operation getOperation() {
        return this.operation;
    }

    public void setOperation(TableFilter.Operation operation) {
        this.operation = operation;
    }

    public Object getValue() {
        return value;
    }

    public void setValue(Object value) {
        this.value = value;
    }

}
