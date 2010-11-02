package org.cern.cms.csc.dw.gui.table;

import java.io.Serializable;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Date;
import java.util.TimeZone;
import javax.faces.convert.Converter;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import org.cern.cms.csc.dw.gui.table.column.filter.converter.FilterConverter;
import org.cern.cms.csc.dw.metadata.PropertyMd;

public class BeanTableSelectColumn implements Serializable {

    private static final String DEFAULT_DATE_PATTERN = "EEE dd-MM-yy HH:mm:ss";
    public static final TimeZone EST_TIME_ZONE = TimeZone.getTimeZone("Europe/Zurich");

    private final PropertyMd propertyMd;
    private Integer width = null;
    private Converter converter = null;
    private FilterConverter filterConverter;

    public BeanTableSelectColumn(PropertyMd propertyMd) {
        this.propertyMd = propertyMd;
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

    public PropertyMd getPropertyMd() {
        return propertyMd;
    }

    public Integer getWidth() {
        return width;
    }

    public void setWidth(Integer width) {
        this.width = width;
    }

    public FilterConverter getFilterConverter() {
        return filterConverter;
    }

    public Converter getConverter() {
        return converter;
    }

}
