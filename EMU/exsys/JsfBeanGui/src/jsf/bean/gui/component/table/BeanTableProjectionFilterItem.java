package jsf.bean.gui.component.table;

public class BeanTableProjectionFilterItem extends BeanTableFilterItem {

    public BeanTableProjectionFilterItem(BeanTablePack value) {
        super(value);
        super.setOperation(BeanTableFilter.Operation.IN);
    }

    public BeanTablePack getTablePack() {
        return (BeanTablePack) getValue();
    }
    
}
