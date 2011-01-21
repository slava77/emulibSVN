package org.cern.cms.csc.exsys.gui.component.table;

import org.cern.cms.csc.exsys.gui.component.BeanTable;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Collection;
import java.util.Date;
import java.util.TimeZone;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.faces.convert.Converter;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import javax.faces.event.ActionEvent;
import org.apache.commons.beanutils.PropertyUtils;
import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.exsys.gui.component.table.converter.FilterConverter;

public class BeanTableColumn implements BeanTableColumnIf {

    private static final Logger logger = Logger.getLogger(BeanTableColumn.class.getName());
    private static final int FILTER_TABLE_PAGE_SIZE = 20;
    private static final int FILTER_TABLE_PAGE_FAST_STEP = 5;
    private static final int[] FILTER_TABLE_PAGE_SIZES = {5, 10, 15, 20, 25, 30};
    private static final String DEFAULT_DATE_PATTERN = "EEE dd-MM-yy HH:mm:ss";
    public static final TimeZone EST_TIME_ZONE = TimeZone.getTimeZone("Europe/Zurich");

    private final BeanTable table;
    private final PropertyMd propertyMd;
    private String name;
    private boolean ascending = true;
    private Integer width = null;

    private final boolean entityType;
    private final boolean listType;

    private BeanTableFilter filter = null;
    private Converter converter = null;
    private FilterConverter filterConverter;

    public BeanTableColumn(BeanTable table, PropertyMd propertyMd) {

        this.table = table;
        this.propertyMd = propertyMd;
        this.name = propertyMd.getName();

        // Exception for the enumerations
        if (propertyMd.getType().isEnum()) {
            this.name = this.name.concat("Item");
        }

        if (EntityBase.class.isAssignableFrom(propertyMd.getType())) {
            this.entityType = true;
            this.listType = false;
        } else {
            if (propertyMd.getType().isArray() || Collection.class.isAssignableFrom(propertyMd.getType())) {
                this.entityType = false;
                this.listType = true;
            } else {
                this.entityType = false;
                this.listType = false;
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

    }

    protected PropertyMd getPropertyMd() {
        return propertyMd;
    }

    @Override
    public boolean isAscending() {
        return ascending;
    }

    public void setAscending(boolean ascending) {
        this.ascending = ascending;
    }

    @Override
    public String getName() {
        return name;
    }

    public String getTitle() {
        return getPropertyMd().getTitle();
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

    public boolean isEntityType() {
        return entityType;
    }

    @Override
    public BeanTableFilter getFilter() {
        return filter;
    }

    @Override
    public void setFilter(BeanTableFilterIf filter) {
        this.filter = (BeanTableFilter) filter;
    }

    @Override
    public boolean isFilterSet() {
        return filter != null && !filter.isEmpty();
    }

    public void clearFilterListener(ActionEvent ev) {
        filter = null;
    }

    public Object getCellValue() {
        EntityBase o = table.getCurrentRow();
        if (o != null) {
            try {
                return PropertyUtils.getSimpleProperty(o, getName());
            } catch (Exception ex) {
                logger.log(Level.SEVERE, "Error while retrieving bean value", ex);
            }
        }
        return null;
    }

    public boolean isListType() {
        return listType;
    }


    public void widthUpListener(ActionEvent ev) {
        if (this.width != null) {
            changeWidth(table.getWidthChangeStep());
        }
    }

    public void widthDownListener(ActionEvent ev) {
        if (this.width != null) {
            if (this.width > 0) {
                changeWidth((-1) * table.getWidthChangeStep());
            }
        }
    }

    private void changeWidth(int step) {
        if (table.getSelectedColumns().getTarget().size() > 1) {

            this.width += step;
            if (width <= 0) {
                width = 0;
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

    public boolean isSorting() {
        return table.getSortingColumns().getTarget().contains(this);
    }

    public void sortListener(ActionEvent ev) {
        if (isSorting() && table.getSortingColumns().getTarget().size() == 1) {
                this.ascending = !this.ascending;
        } else {

            // All to source
            table.getSortingColumns().getSource().addAll(table.getSortingColumns().getTarget());
            table.getSortingColumns().getTarget().clear();

            // This to target
            table.getSortingColumns().getSource().remove(this);
            table.getSortingColumns().getTarget().add(this);
        }
        table.refreshListener(ev);
    }

    public void filterTableListener(ActionEvent ev) throws Exception {
        if (isEntityType()) {
            if (filter == null) {
                BeanTable bt = new BeanTable(getTitle(),
                                            propertyMd.getType(),
                                            true,
                                            FILTER_TABLE_PAGE_SIZE,
                                            FILTER_TABLE_PAGE_FAST_STEP,
                                            FILTER_TABLE_PAGE_SIZES) {
                        @Override
                        public BeanTableDaoIf getBeanTableDao() {
                            return table.getBeanTableDao();
                        }
                };
                BeanTableFilterItem fi = new BeanTableProjectionFilterItem(bt);
                this.filter = new BeanTableFilter();
                this.filter.getItems().add(fi);
            }
            BeanTableProjectionFilterItem fi = (BeanTableProjectionFilterItem) getFilter().getItems().get(0);
            table.setFilterTable(fi.getBeanTable());
        }
    }

}
