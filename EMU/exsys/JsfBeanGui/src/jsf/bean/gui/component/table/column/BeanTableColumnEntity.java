package jsf.bean.gui.component.table.column;

import javax.faces.event.ActionEvent;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.component.table.BeanTableProjectionFilter;
import jsf.bean.gui.component.table.BeanTableProjectionFilterItem;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumnEntity extends BeanTableColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnEntity.class);

    public BeanTableColumnEntity(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);
    }

    @Override
    public boolean isEntityType() {
        return true;
    }

    @Override
    public boolean isFilterSet() {
        if (filter != null) {
            if (getFilterTablePack().getTable().isFilterOn()) {
                return true;
            }

            if (!getFilterTablePack().isSingleClass()) {
                return true;
            }
        }
        return false;
    }

    public BeanTablePack getFilterTablePack() {
        if (filter != null) {
            BeanTableProjectionFilterItem tf = (BeanTableProjectionFilterItem) filter.getItems().get(0);
            return tf.getTablePack();
        } else {
            return null;
        }
    }

    public void filterTableListener(ActionEvent ev) throws Exception {
        if (filter == null) {
            this.filter = new BeanTableProjectionFilter();
            BeanTablePack btp = new BeanTablePack(getName(),
                    "Filter by ".concat(getTitle()),
                    table.getPack().getManager(),
                    type);
            ((BeanTableProjectionFilter) this.filter).setPack(btp);
        }
        BeanTableProjectionFilterItem fi = (BeanTableProjectionFilterItem) getFilter().getItems().get(0);
        table.getPack().getManager().pushTable((BeanTablePack) fi.getTablePack());
    }
}
