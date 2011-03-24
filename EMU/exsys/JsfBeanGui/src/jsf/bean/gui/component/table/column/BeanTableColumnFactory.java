package jsf.bean.gui.component.table.column;

import java.io.Serializable;
import java.util.Collection;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.EmbeddedPropertyMd;
import jsf.bean.gui.metadata.PropertyMd;

public abstract class BeanTableColumnFactory implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnFactory.class);

    public static BeanTableColumn getBeanTableColumn(BeanTable table, PropertyMd propertyMd) {

        if (isEntity(propertyMd)) {
            return new BeanTableColumnEntity(table, propertyMd);
        }

        if (isEmbedded(propertyMd)) {
            return new BeanTableColumnEmbedded(table, (EmbeddedPropertyMd) propertyMd);
        }

        if (isList(propertyMd)) {
            return new BeanTableColumnList(table, propertyMd);
        }

        return new BeanTableColumnSimple(table, propertyMd);

    }

    public static BeanTableColumnBase getBeanTableColumnBase(PropertyMd propertyMd) {

        if (isEntity(propertyMd) || isEmbedded(propertyMd) || isList(propertyMd)) {
            return null;
        }

        return new BeanTableColumnBase(propertyMd);

    }

    private static boolean isEntity(PropertyMd propertyMd) {
        return EntityBeanBase.class.isAssignableFrom(propertyMd.getType());
    }

    private static boolean isEmbedded(PropertyMd propertyMd) {
        return propertyMd instanceof EmbeddedPropertyMd;
    }

    private static boolean isList(PropertyMd propertyMd) {
        return propertyMd.getType().isArray() || Collection.class.isAssignableFrom(propertyMd.getType());
    }

}
