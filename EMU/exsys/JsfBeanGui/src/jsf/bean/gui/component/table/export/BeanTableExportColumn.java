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
    private final ColumnValue columnValue = new ColumnValue();
    private final BeanTableColumnBase column;
    private final String outputFormat;
    private final List<BeanTableExportColumn> embeddedProperties;

    public BeanTableExportColumn(BeanTableColumnBase column) {

        this.column = column;
        if (column instanceof BeanTableColumnSimple) {
            this.outputFormat = ((BeanTableColumnSimple) column).getOutputFormat();
        } else {
            outputFormat = null;
        }

        this.embeddedProperties = new ArrayList<BeanTableExportColumn>();
        if (column.isEmbedType()) {
            for (BeanTableColumnBase pc: ((BeanTableColumnEmbedded) column).getProperties()) {
                this.embeddedProperties.add(new BeanTableExportColumn(pc));
            }
        }

    }

    public String getName() {
        return column.getName();
    }

    public String getTitle() {
        return column.getTitle();
    }

    public Class getType() {
        return column.getType();
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

    public boolean isEntityType() {
        return column.isEntityType();
    }

    public boolean isBoolean() {
        return column.isBoolean();
    }

    public boolean isNumeric() {
        return column.isNumeric();
    }

    public boolean isListType() {
        return column.isListType();
    }

    public boolean isDate() {
        return column.isDate();
    }

    public boolean isEmbedType() {
        return column.isEmbedType();
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
                Object item = ((StringModel) args.get(0)).getWrappedObject();
                return getValue(item);

            } catch (Exception e) {
                logger.error(e);
                return new SimpleScalar("");
            }
        }

        private TemplateModel getValue(Object item) throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            String s = getName();
            Object value = PropertyUtils.getSimpleProperty(item, s);
            if (column.isEntityType()) {
                return new BeanModel(value, new BeansWrapper());
            }
            if (value == null) {
                return new SimpleScalar("");
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
