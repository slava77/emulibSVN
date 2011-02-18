package jsf.bean.gui.component.table;

import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Collection;
import java.util.Date;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.faces.convert.Converter;
import javax.faces.convert.DateTimeConverter;
import javax.faces.convert.NumberConverter;
import javax.faces.event.ActionEvent;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.converter.FilterConverter;
import org.apache.commons.beanutils.PropertyUtils;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumn {

    private static final Logger logger = Logger.getLogger(BeanTableColumn.class.getName());

    private final BeanTable table;
    private final PropertyMd propertyMd;
    private String name;

    private final boolean entityType;
    private final boolean listType;
    private Class listItemType = null;

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

        if (EntityBeanBase.class.isAssignableFrom(propertyMd.getType())) {
            this.entityType = true;
            this.listType = false;
        } else {
            if (propertyMd.getType().isArray() || Collection.class.isAssignableFrom(propertyMd.getType())) {

                this.listType = true;

                Type t = propertyMd.getGenericType();
                logger.info(t.toString());
                if (t instanceof ParameterizedType) {
                    ParameterizedType pt = (ParameterizedType) t;
                    logger.info(pt.toString());
                    if (pt.getActualTypeArguments().length == 1) {
                        Type lit = pt.getActualTypeArguments()[0];
                        logger.info(lit.toString());
                        if (lit instanceof Class) {
                            this.listItemType = (Class) lit;
                        }
                    }
                }


                if (listItemType != null && EntityBeanBase.class.isAssignableFrom(listItemType)) {
                    this.entityType = true;
                } else {
                    this.entityType = false;
                }

            } else {

                this.entityType = false;
                this.listType = false;
                this.filterConverter = FilterConverter.getFilterConverter(propertyMd.getType());

                if (propertyMd.getType().equals(BigDecimal.class) ||
                    propertyMd.getType().equals(BigInteger.class) ||
                    propertyMd.getType().equals(Integer.class) ||
                    propertyMd.getType().equals(Long.class) ||
                    propertyMd.getType().equals(Float.class) ||
                    propertyMd.getType().equals(Double.class) ||
                    (propertyMd.getType().isPrimitive() &&
                        (propertyMd.getType().getSimpleName().equals("int") ||
                         propertyMd.getType().getSimpleName().equals("long") ||
                         propertyMd.getType().getSimpleName().equals("float")||
                         propertyMd.getType().getSimpleName().equals("double")))) {

                    NumberConverter numberConverter = new NumberConverter();

                    numberConverter.setGroupingUsed(table.getProperties().getColumnNumberGrouping(name));
                    {
                        Integer v = table.getProperties().getColumnNumberMinFractionDigits(name);
                        if (v != null) {
                            numberConverter.setMinFractionDigits(v);
                        }
                    }
                    {
                        Integer v = table.getProperties().getColumnNumberMaxFractionDigits(name);
                        if (v != null) {
                            numberConverter.setMaxFractionDigits(v);
                        }
                    }
                    numberConverter.setPattern(table.getProperties().getColumnNumberPattern(name));

                    this.converter = numberConverter;

                } else
                if (propertyMd.getType().equals(Date.class)) {

                    this.converter = new DateTimeConverter();
                    ((DateTimeConverter) converter).setTimeZone(table.getProperties().getColumnDateTimeZone(name));
                    ((DateTimeConverter) converter).setPattern(table.getProperties().getColumnDateFormat(name));

                }
            }
        }

    }

    protected PropertyMd getPropertyMd() {
        return propertyMd;
    }

    public boolean isAscending() {
        return table.getProperties().getColumnSortAsc(name);
    }

    public void setAscending(boolean ascending) {
        table.getProperties().setColumnSortAsc(name, ascending);
    }

    public String getName() {
        return name;
    }

    public String getTitle() {
        return getPropertyMd().getTitle();
    }

    public Integer getWidth() {
        return table.getProperties().getColumnWidth(name);
    }

    public void setWidth(Integer width) {
        table.getProperties().setColumnWidth(name, width);
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

    public BeanTableFilter getFilter() {
        return filter;
    }

    public void setFilter(BeanTableFilter filter) {
        this.filter = (BeanTableFilter) filter;
    }

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

    public void clearFilterListener(ActionEvent ev) {
        filter = null;
        this.table.refresh();
    }

    public Object getCellValue() {
        EntityBeanBase o = table.getCurrentRow();
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

    public boolean isSorting() {
        return table.getSortingColumns().getTarget().contains(this);
    }

    public void sortListener(ActionEvent ev) {

        if (isSorting() && table.getSortingColumns().getTarget().size() == 1) {
            
            setAscending(!isAscending());

        } else {

            // All to source
            table.getSortingColumns().getSource().addAll(table.getSortingColumns().getTarget());
            table.getSortingColumns().getTarget().clear();

            // This to target
            table.getSortingColumns().getSource().remove(this);
            table.getSortingColumns().getTarget().add(this);

        }

        table.sortingChangeListener(null);
        table.refreshListener(ev);

    }

    public void filterTableListener(ActionEvent ev) throws Exception {
        if (isEntityType()) {
            if (filter == null) {
                this.filter = new BeanTableProjectionFilter();
                BeanTablePack btp = new BeanTablePack(getName(), 
                                                      "Filter by ".concat(getTitle()),
                                                      table.getPack().getManager(),
                                                      (isListType() ? listItemType : propertyMd.getType()));
                ((BeanTableProjectionFilter) this.filter).setPack(btp);
            }
            BeanTableProjectionFilterItem fi = (BeanTableProjectionFilterItem) getFilter().getItems().get(0);
            table.getPack().getManager().pushTable((BeanTablePack) fi.getTablePack());
        }
    }

    public Class getType() {
        return propertyMd.getType();
    }

    public void setListPropertyTable(EntityBeanBase currentItem) throws Exception {
       if (isEntityType() && isListType()) {
            BeanTablePackFilter listFilter = new BeanTableListFilter(table.getRowClass(), currentItem , getName());
            BeanTablePack btp = new BeanTablePack(getName(),
                                                  String.format("List of '%s' %s", currentItem.getEntityTitle(), getTitle()),
                                                  table.getPack().getManager(),
                                                  listItemType);
            btp.addFilter(listFilter);

            table.getPack().getManager().pushTable(btp);
        }


    }

}
