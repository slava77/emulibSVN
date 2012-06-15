/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import freemarker.ext.beans.BeanModel;
import freemarker.ext.beans.BeansWrapper;
import freemarker.ext.beans.StringModel;
import freemarker.template.SimpleDate;
import freemarker.template.SimpleNumber;
import freemarker.template.SimpleScalar;
import freemarker.template.TemplateBooleanModel;
import freemarker.template.TemplateMethodModelEx;
import freemarker.template.TemplateModel;
import freemarker.template.TemplateModelException;
import java.lang.reflect.InvocationTargetException;
import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.List;
import jsf.bean.gui.component.table.column.BeanTableColumnBase;
import jsf.bean.gui.component.table.column.BeanTableColumnEmbedded;
import jsf.bean.gui.component.table.column.BeanTableColumnSimple;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.apache.commons.beanutils.PropertyUtils;

public class BeanTableExportColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportColumn.class);
    private final BeanTableColumnBase column;
    private final String name;
    private final String title;
    private final Class type;
    private final ColumnValue columnValue = new ColumnValue();
    private final String outputFormat;
    private final List<BeanTableExportColumn> embeddedProperties;
    private final Boolean isEntityType;
    private final Boolean isBoolean;
    private final Boolean isNumeric;
    private final Boolean isListType;
    private final Boolean isDate;
    private final Boolean isEmbedType;

    public BeanTableExportColumn(BeanTableColumnBase column) {

        this.column = column;
        if (column instanceof BeanTableColumnSimple) {
            this.outputFormat = ((BeanTableColumnSimple) column).getOutputFormat();
        } else {
            outputFormat = null;
        }

        this.embeddedProperties = new ArrayList<BeanTableExportColumn>();
        if (column.isEmbedType()) {
            for (BeanTableColumnBase pc : ((BeanTableColumnEmbedded) column).getProperties()) {
                this.embeddedProperties.add(new BeanTableExportColumn(pc));
            }
        }

        name = column.getName();
        title = column.getTitle();
        type = column.getType();
        isEntityType = column.isEntityType();
        isBoolean = column.isBoolean();
        isNumeric = column.isNumeric();
        isListType = column.isListType();
        isDate = column.isDate();
        isEmbedType = column.isEmbedType();

    }

    public BeanTableExportColumn() {
        column = null;
        name = null;
        title = null;
        type = null;
        outputFormat = null;
        embeddedProperties = new ArrayList<BeanTableExportColumn>();
        isEntityType = null;
        isBoolean = null;
        isNumeric = null;
        isListType = null;
        isDate = null;
        isEmbedType = null;
    }

    public String getName() {
        return name;
    }

    public String getTitle() {
        return title;
    }

    public Class getType() {
        return type;
    }

    public ColumnValue getColumnValue() {
        return columnValue;
    }

    public String getOutputFormat() {
        return outputFormat;
    }

    public List<BeanTableExportColumn> getEmbeddedProperties() {
        return embeddedProperties;
    }

    public Boolean getIsEntityType() {
        return isEntityType;
    }

    public Boolean getIsBoolean() {
        return isBoolean;
    }

    public Boolean getIsNumeric() {
        return isNumeric;
    }

    public Boolean getIsListType() {
        return isListType;
    }

    public Boolean getIsDate() {
        return isDate;
    }

    public Boolean getIsEmbedType() {
        return isEmbedType;
    }

    public class ColumnValue implements TemplateMethodModelEx {

        public TemplateModel exec(List args) throws TemplateModelException {

            if (args.size() != 1) {
                logger.error("Template error. Wrong quantity of arguments was passed to method.");
                return new SimpleScalar("");
            }
            try {

                if (column.isEmbedType()) {
                    Object item = ((BeanModel) args.get(0)).getWrappedObject();
                    return getValue(item);
                }

                Object o = args.get(0);
                if (o instanceof StringModel) {
                    Object item = ((StringModel) o).getWrappedObject();
                    TemplateModel returnValue = getValue(item);
                    return returnValue;
                }

                return (SimpleScalar) o;

            } catch (Exception e) {
                logger.error(e);
                return new SimpleScalar("");
            }
        }

        private TemplateModel getValue(Object item) throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            String s = getName();
            Object value = PropertyUtils.getSimpleProperty(item, s);
            if (value == null) {
                return SimpleScalar.EMPTY_STRING;
            }
            if (column.isEntityType()) {
                return new BeanModel(value, new BeansWrapper());
            }
            if (column.isBoolean()) {
                if ((Boolean) value) {
                    return TemplateBooleanModel.TRUE;
                } else {
                    return TemplateBooleanModel.FALSE;
                }
            }
            if (column.isDate()) {
                return new SimpleDate((Timestamp) value);
            }
            if (column.isNumeric()) {
                return new SimpleNumber((Number) value);
            }
            if (column.isEmbedType()) {
                return new StringModel(value, new BeansWrapper());
            }
            return new SimpleScalar(value.toString());

        }
    }
}
