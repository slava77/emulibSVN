package jsf.bean.gui.component.table;

import java.io.Serializable;

public class BeanTableFilterItem implements Serializable {

    private Object value;
    private BeanTableFilter.Operator operator = BeanTableFilter.Operator.OR;
    private BeanTableFilter.Operation operation = BeanTableFilter.Operation.EQUAL;

    public BeanTableFilterItem() {
    }

    public BeanTableFilterItem(Object value) {
        this.value = value;
    }

    public BeanTableFilter.Operator getOperator() {
        return this.operator;
    }

    public void setOperator(BeanTableFilter.Operator operator) {
        this.operator = operator;
    }

    public BeanTableFilter.Operation getOperation() {
        return this.operation;
    }

    public void setOperation(BeanTableFilter.Operation operation) {
        this.operation = operation;
    }

    public Object getValue() {
        return value;
    }

    public void setValue(Object value) {
        this.value = value;
    }

}
