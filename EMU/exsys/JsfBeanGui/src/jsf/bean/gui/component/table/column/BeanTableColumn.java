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



    /**
     *
     * Width stuff
     *
     */

    public Integer getWidth() {
        return table.getProperties().getColumnWidth(name);
    }

    public void setWidth(Integer width) {
        table.getProperties().setColumnWidth(name, width);
    }

    public void widthUpListener(ActionEvent ev) {
        if (this.getWidth() != null) {
            changeWidth(table.getProperties().getColumnWidthStep());
        }
    }

    public void widthDownListener(ActionEvent ev) {
        if (this.getWidth() != null) {
            if (this.getWidth() > 0) {
                changeWidth((-1) * table.getProperties().getColumnWidthStep());
            }
        }
    }

    private void changeWidth(int step) {
        if (table.getSelectedColumns().getTarget().size() > 1) {

            setWidth(getWidth() + step);
            if (getWidth() <= 0) {
                setWidth(0);
            }

            BeanTableColumn prev = null;
            int myidx = table.getSelectedColumns().getTarget().indexOf(this);
            if (myidx == (table.getSelectedColumns().getTarget().size() - 1)) {
                prev = (BeanTableColumn) table.getSelectedColumns().getTarget().get(myidx - 1);
            } else {
                prev = (BeanTableColumn) table.getSelectedColumns().getTarget().get(myidx + 1);
            }

            if (prev != null) {
                prev.setWidth(prev.getWidth() + ((-1) * step));
                if (prev.getWidth() < 0) {
                    prev.setWidth(0);
                }
            }
        }
    }

}
