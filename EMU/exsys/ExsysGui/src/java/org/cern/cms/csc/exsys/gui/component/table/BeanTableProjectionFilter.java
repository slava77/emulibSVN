package org.cern.cms.csc.exsys.gui.component.table;

public class BeanTableProjectionFilter extends BeanTableFilter {

    private BeanTableProjectionFilterItem getItem() {
        if (!getItems().isEmpty()) {
            return (BeanTableProjectionFilterItem) getItems().get(0);
        }
        return null;
    }

    public void setPack(BeanTablePack pack) {
        getItems().clear();
        getItems().add(new BeanTableProjectionFilterItem(pack));
    }

    public void setOperation(String op) {
        BeanTableProjectionFilterItem item = getItem();
        if (item != null) {
            item.setOperation(Operation.fromValue(op));
        }
    }

    public String getOperation() {
        BeanTableProjectionFilterItem item = getItem();
        if (item != null) {
            return item.getOperation().getValue();
        }
        return null;
    }

}
