package jsf.bean.gui.component.table.column;

import javax.faces.event.ActionEvent;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;
import org.apache.commons.beanutils.PropertyUtils;

public abstract class BeanTableColumn extends BeanTableColumnBase {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumn.class);

    protected final BeanTable table;

    public BeanTableColumn(BeanTable table, PropertyMd propertyMd) {
        super(propertyMd);
        this.table = table;
    }

    /**
     *
     * Specific methods
     *
     */

    public void clearFilterListener(ActionEvent ev) {
        clearFilter();
        table.refresh();
    }

    public Object getCellValue() {
        EntityBeanBase o = table.getCurrentRow();
        if (o != null) {
            try {
                return PropertyUtils.getSimpleProperty(o, getName());
            } catch (Exception ex) {
                logger.error("Error while retrieving bean value", ex);
            }
        }
        return null;
    }

    public Integer getWidth() {
        return table.getProperties().getColumnWidth(name);
    }

    public void setWidth(Integer width) {
        table.getProperties().setColumnWidth(name, width);
    }

}
