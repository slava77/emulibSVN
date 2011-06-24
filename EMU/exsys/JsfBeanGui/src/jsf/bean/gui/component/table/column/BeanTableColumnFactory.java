package jsf.bean.gui.component.table.column;

import java.io.Serializable;
import java.lang.reflect.ParameterizedType;
import java.lang.reflect.Type;
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

        if (isList(propertyMd) && isEntity(propertyMd)) {
            return new BeanTableColumnEntityList(table, propertyMd);
        }

        if (isEntity(propertyMd)) {
            return new BeanTableColumnEntity(table, propertyMd);
        }

        if (isEmbedded(propertyMd)) {
            return new BeanTableColumnEmbedded(table, (EmbeddedPropertyMd) propertyMd);
        }

        return new BeanTableColumnSimple(table, propertyMd);

    }

    public static Class getParametrizedType(PropertyMd propertyMd) {
        Type t = propertyMd.getGenericType();
        if (t instanceof ParameterizedType) {
            ParameterizedType pt = (ParameterizedType) t;
            if (pt.getActualTypeArguments().length == 1) {
                Type lit = pt.getActualTypeArguments()[0];
                if (lit instanceof Class) {
                    return (Class) lit;
                }
            }
        }
        return Object.class;
    }
    
    private static boolean isEntity(PropertyMd propertyMd) {
        return EntityBeanBase.class.isAssignableFrom(propertyMd.getType()) ||
               (isList(propertyMd) && 
                   EntityBeanBase.class.isAssignableFrom(getParametrizedType(propertyMd)));
    }

    private static boolean isEmbedded(PropertyMd propertyMd) {
        return propertyMd instanceof EmbeddedPropertyMd;
    }

    private static boolean isList(PropertyMd propertyMd) {
        return propertyMd.getType().isArray() || Collection.class.isAssignableFrom(propertyMd.getType());
    }
    
}
