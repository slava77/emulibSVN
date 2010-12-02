package org.cern.cms.csc.dw.gui.table;

import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Date;
import java.util.TimeZone;
import javax.faces.convert.Converter;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.gui.table.column.filter.TableFilter;
import org.cern.cms.csc.dw.gui.table.column.filter.converter.FilterConverter;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.util.ClassUtil;

public class BeanTableColumn extends BeanTableColumnBase {
    
    private static final String DEFAULT_DATE_PATTERN = "EEE dd-MM-yy HH:mm:ss";
    public static final TimeZone EST_TIME_ZONE = TimeZone.getTimeZone("Europe/Zurich");

    private final TYPE type;
    private TableFilter filter = null;
    private Converter converter = null;
    private FilterConverter filterConverter;

    public BeanTableColumn(BeanTableBase table, PropertyMd propertyMd) {
        super(table, propertyMd);

        if (ClassUtil.hasAsSuperClass(propertyMd.getType(), EntityBase.class)) {
            this.type = TYPE.ENTITY;
        } else {
            this.type = TYPE.SIMPLE;
            this.filterConverter = FilterConverter.getFilterConverter(propertyMd.getType());

            if (propertyMd.getType().equals(BigDecimal.class) ||
                propertyMd.getType().equals(BigInteger.class) ||
                propertyMd.getType().equals(Integer.class) ||
                propertyMd.getType().equals(Long.class)) {

                this.converter = new NumberConverter();

            } else
            if (propertyMd.getType().equals(Date.class)) {

                this.converter = new DateTimeConverter();
                ((DateTimeConverter) converter).setTimeZone(EST_TIME_ZONE);
                ((DateTimeConverter) converter).setPattern(DEFAULT_DATE_PATTERN);

            }
        }
    }

    public FilterConverter getFilterConverter() {
        return filterConverter;
    }

    public Converter getConverter() {
        return converter;
    }

    public TYPE getType() {
        return type;
    }

    public TableFilter getFilter() {
        return filter;
    }

    public void setFilter(BeanTableFilterIf filter) {
        this.filter = (TableFilter) filter;
    }

    public boolean isFilterSet() {
        return filter != null && !filter.isEmpty();
    }

    public void clearFilter() {
        filter = null;
    }

}
