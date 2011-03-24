package jsf.bean.gui.component.table.column;

import java.io.Serializable;
import javax.faces.convert.Converter;
import jsf.bean.gui.component.table.BeanTableFilter;
import jsf.bean.gui.component.table.converter.FilterConverter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumnBase implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnBase.class);

    protected String name;
    protected final Class type;
    protected final String title;

    protected Converter converter = null;
    protected BeanTableFilter filter = null;
    protected FilterConverter filterConverter;

    public BeanTableColumnBase(PropertyMd propertyMd) {
        this.name = propertyMd.getName();
        this.type = propertyMd.getType();
        this.title = propertyMd.getTitle();
        this.filterConverter = FilterConverter.getFilterConverter(type);
    }

    /**
     *
     * To override
     *
     */

    public boolean isBoolean() {
        return false;
    }

    public boolean isEmbedType() {
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

    public void clearFilter() {
        filter = null;
    }

}
