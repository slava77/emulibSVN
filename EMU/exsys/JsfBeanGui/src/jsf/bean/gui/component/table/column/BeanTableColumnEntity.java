package jsf.bean.gui.component.table.column;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.Collection;
import javax.faces.event.ActionEvent;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.annotation.EntityTitle;
import jsf.bean.gui.annotation.EntityTitleType;
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
    private EntityTitle entityTitle;

    public BeanTableColumnEntity(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);
        if (RestrictedPropertyMd.class.isAssignableFrom(propertyMd.getClass())) {
            RestrictedPropertyMd restrictedPropertyMd = (RestrictedPropertyMd) propertyMd;
            this.referencedProperty = restrictedPropertyMd.getReferencedProperty();
            this.entityTitle = restrictedPropertyMd.getEntityTitle();
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

    @Override
    public Collection<BeanTableQueryColumn> getQueryColumns() {
        try {
            Collection<BeanTableQueryColumn> cols = new ArrayList<BeanTableQueryColumn> ();
            boolean filterWasNull = (filter == null);
            if (filterWasNull) {
                setFilterTablePack(null);
            }
            for (BeanTableColumn c: getFilterTablePack().getTable().getColumns()) {
                cols.add(
                    new BeanTableQueryColumn(
                        this.getTitle().concat(".").concat(c.getTitle()),
                        this.getName().concat(".").concat(c.getName()), 
                        c.getType(), 
                        false,
                        false));
            }
            if (filterWasNull) {
                clearFilter();
            }
            return cols;
        } catch (Exception ex) {
            throw new RuntimeException(ex);
        }
    }

    public Object getEntityTitle() throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
        if (entityTitle != null) {
            if (entityTitle.type().equals(EntityTitleType.CONSTANT)) {
                return entityTitle.value();
            }
            if (entityTitle.type().equals(EntityTitleType.PROPERTY)) {
                return getTable().getColumn(entityTitle.value()).getCellValue();
            }
        }
        EntityBeanBase bean = (EntityBeanBase) getCellValue();
        if (bean != null) {
            return bean.getEntityTitle();
        } else {
            return null;
        }
    }
    
}
