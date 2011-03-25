/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.export;

import freemarker.ext.beans.StringModel;
import freemarker.template.SimpleScalar;
import freemarker.template.TemplateMethodModelEx;
import freemarker.template.TemplateModel;
import freemarker.template.TemplateModelException;
import java.lang.reflect.InvocationTargetException;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.column.BeanTableColumnBase;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.apache.commons.beanutils.PropertyUtils;

public class BeanTableExportColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableExportColumn.class);
    private final ColumnValue columnValue = new ColumnValue();
    private final BeanTableColumnBase column;

    public BeanTableExportColumn(BeanTableColumnBase column) {
        this.column = column;
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

    public class ColumnValue implements TemplateMethodModelEx {

        public TemplateModel exec(List args) throws TemplateModelException {

            if (args.size() != 1) {
                logger.error("Template error. Wrong quantity of arguments was passed to method.");
                return new SimpleScalar("");
            }
            try {
                EntityBeanBase item = (EntityBeanBase) ((StringModel) args.get(0)).getWrappedObject();
                return checkType(PropertyUtils.getSimpleProperty(item, getName()));

            } catch (Exception e) {
                logger.error(e);
                return new SimpleScalar("");
            }
        }

        private TemplateModel checkType(Object value) throws IllegalAccessException, InvocationTargetException, NoSuchMethodException {
            if (EntityBeanBase.class.isAssignableFrom(value.getClass())) {
                return new SimpleScalar(((EntityBeanBase) value).getEntityTitle());
            }
            if (value == null) {
                return null;
            }
            return new SimpleScalar(value.toString());

        }
    }
}
