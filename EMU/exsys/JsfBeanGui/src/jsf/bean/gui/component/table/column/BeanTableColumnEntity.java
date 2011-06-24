package jsf.bean.gui.component.table.column;

import javax.faces.event.ActionEvent;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.component.table.BeanTableProjectionFilter;
import jsf.bean.gui.component.table.BeanTableProjectionFilterItem;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;
import jsf.bean.gui.metadata.RestrictedPropertyMd;

public class BeanTableColumnEntity extends BeanTableColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnEntity.class);
    private String referencedProperty = null;

    public BeanTableColumnEntity(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);
        if (RestrictedPropertyMd.class.isAssignableFrom(propertyMd.getClass())) {
            referencedProperty = ((RestrictedPropertyMd) propertyMd).getReferencedProperty();
        }
    }

    public String getReferencedProperty() {
        return referencedProperty;
    }
    
    protected Class getFilterType() {
        return type;
    }
    
    @Override
    public boolean isEntityType() {
        return true;
    }

    @Override
    public boolean isFilterSet() {
        if (filter != null) {
            BeanTablePack filterTablePack = getFilterTablePack();
            
            if (!filterTablePack.isSingleClass()) {
                return true;
            }
            
            if (filterTablePack.getTable().isFilterOn()) {
                return true;
            }
            
        }
        return false;
    }

    public BeanTablePack getFilterTablePack() {
        BeanTableProjectionFilterItem tf = (BeanTableProjectionFilterItem) filter.getItems().get(0);
        return tf.getTablePack();
    }
    
    public BeanTablePack setFilterTablePack() throws Exception {
        return setFilterTablePack(null);
    }
    
    public BeanTablePack setFilterTablePack(String prefix) throws Exception {
        if (filter == null) {
            this.filter = new BeanTableProjectionFilter();
            BeanTablePack btp = new BeanTablePack(
                    prefix != null ? prefix.concat(getName()).concat(".") : getName(),
                    "Filter by ".concat(getTitle()),
                    table.getPack().getManager(),
                    getFilterType());
            ((BeanTableProjectionFilter) this.filter).setPack(btp);
        }
        return getFilterTablePack();
    }

    public void filterTableListener(ActionEvent ev) throws Exception {
        table.getPack().getManager().pushTable(setFilterTablePack());
    }

}
