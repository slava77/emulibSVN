package jsf.bean.gui.component.table.column;

import java.io.Serializable;
import java.util.Collection;
import javax.persistence.Embedded;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public abstract class BeanTableColumnFactory implements Serializable {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnFactory.class);

    public static BeanTableColumn getBeanTableColumn(BeanTable table, PropertyMd propertyMd) {

        if (EntityBeanBase.class.isAssignableFrom(propertyMd.getType())) {
            return new BeanTableColumnEntity(table, propertyMd);
        }

        if (propertyMd.getGetterMethod().isAnnotationPresent(Embedded.class)) {
            return new BeanTableColumnEmbedded(table, null);
        }

        if (propertyMd.getType().isArray() || Collection.class.isAssignableFrom(propertyMd.getType())) {
            return new BeanTableColumnList(table, propertyMd);
        }

        return new BeanTableColumnSimple(table, propertyMd);

    }

}
