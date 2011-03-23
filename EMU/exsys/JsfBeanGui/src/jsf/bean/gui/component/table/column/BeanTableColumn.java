package jsf.bean.gui.component.table.column;

import java.io.Serializable;
import javax.faces.convert.Converter;
import javax.faces.event.ActionEvent;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.converter.FilterConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;
import org.apache.commons.beanutils.PropertyUtils;

public abstract class BeanTableColumn implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumn.class);

    protected final BeanTable table;
    protected String name;
    protected final Class type;
    protected final String title;

    protected BeanTableFilter filter = null;
    protected Converter converter = null;
    protected FilterConverter filterConverter;

    public BeanTableColumn(BeanTable table, PropertyMd propertyMd) {
        this.table = table;
        this.name = propertyMd.getName();
        this.type = propertyMd.getType();
        this.title = propertyMd.getTitle();
    }

    /**
     *
     * To override
     *
     */

    public boolean isBoolean() {
        return false;
    }

    public boolean isEmbedded() {
        return false;
    }

    public boolean isListType() {
        return false;
    }

    public boolean isEntityType() {
        return false;
    }

    public boolean isSortable() {
        return false;
    }

    /**
     *
     * Getters and setters
     *
     */

    public String getName() {
        return name;
    }

    public String getTitle() {
        return title;
    }

    public Class getType() {
        return type;
    }

    public FilterConverter getFilterConverter() {
        return filterConverter;
    }

    public Converter getConverter() {
        return converter;
    }

    /**
     *
     * Filter stuff
     *
     */

    public BeanTableFilter getFilter() {
        return filter;
    }

    public void setFilter(BeanTableFilter filter) {
        this.filter = (BeanTableFilter) filter;
    }

    public boolean isFilterSet() {
        if (this.filter != null) {
            return !this.filter.isEmpty();
        }
        return false;
    }

    public void clearFilterListener(ActionEvent ev) {
        filter = null;
    }

    /**
     *
     * Get value from table
     *
     */

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
