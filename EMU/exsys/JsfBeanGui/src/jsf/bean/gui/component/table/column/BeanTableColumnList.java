package jsf.bean.gui.component.table.column;

import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import javax.faces.event.ActionEvent;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableListFilter;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.component.table.BeanTablePackFilter;
import jsf.bean.gui.component.table.BeanTableProjectionFilter;
import jsf.bean.gui.component.table.BeanTableProjectionFilterItem;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumnList extends BeanTableColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnList.class);
    private final boolean entityType;
    private Class itemType = null;

    public BeanTableColumnList(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);

        Type t = propertyMd.getGenericType();
        logger.info(t.toString());
        if (t instanceof ParameterizedType) {
            ParameterizedType pt = (ParameterizedType) t;
            logger.info(pt.toString());
            if (pt.getActualTypeArguments().length == 1) {
                Type lit = pt.getActualTypeArguments()[0];
                logger.info(lit.toString());
                if (lit instanceof Class) {
                    this.itemType = (Class) lit;
                }
            }
        }

        if (itemType != null && EntityBeanBase.class.isAssignableFrom(itemType)) {
            this.entityType = true;
        } else {
            this.entityType = false;
        }

    }

    @Override
    public boolean isFilterSet() {

        if (filter == null) {
            return false;
        }

        if (isEntityType()) {
            BeanTableProjectionFilterItem tf = (BeanTableProjectionFilterItem) filter.getItems().get(0);
            if (tf.getTablePack().getTable().isFilterOn()) {
                return true;
            }
            if (!((BeanTablePack) tf.getTablePack()).isSingleClass()) {
                return true;
            }
        } else {
            return !filter.isEmpty();
        }

        return false;

    }

    @Override
    public boolean isEntityType() {
        return entityType;
    }

    @Override
    public boolean isListType() {
        return true;
    }

    public void filterTableListener(ActionEvent ev) throws Exception {
        if (isEntityType()) {
            if (filter == null) {
                this.filter = new BeanTableProjectionFilter();
                BeanTablePack btp = new BeanTablePack(getName(),
                        "Filter by ".concat(getTitle()),
                        table.getPack().getManager(),
                        itemType);
                ((BeanTableProjectionFilter) this.filter).setPack(btp);
            }
            BeanTableProjectionFilterItem fi = (BeanTableProjectionFilterItem) getFilter().getItems().get(0);
            table.getPack().getManager().pushTable((BeanTablePack) fi.getTablePack());
        }
    }

    public void setListPropertyTable(EntityBeanBase currentItem) throws Exception {
        if (isEntityType()) {
            BeanTablePackFilter listFilter = new BeanTableListFilter(table.getRowClass(), currentItem, getName());
            BeanTablePack btp = new BeanTablePack(getName(),
                    String.format("List of '%s' %s", currentItem.getEntityTitle(), getTitle()),
                    table.getPack().getManager(),
                    itemType);
            btp.addFilter(listFilter);
            table.getPack().getManager().pushTable(btp);
        }
    }
}
