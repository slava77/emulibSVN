package jsf.bean.gui.component.table;

import jsf.bean.gui.EntityBeanBase;


public class BeanTableListFilter extends BeanTablePackFilter {

    private final Class<? extends EntityBeanBase> parentType;
    private final EntityBeanBase parent;
    private final String propertyName;

    public BeanTableListFilter(Class<? extends EntityBeanBase> parentType, EntityBeanBase parent, String propertyName) {
        this.parentType = parentType;
        this.parent = parent;
        this.propertyName = propertyName;
    }

    public EntityBeanBase getParent() {
        return parent;
    }

    public Class<? extends EntityBeanBase> getParentType() {
        return parentType;
    }

    public String getPropertyName() {
        return propertyName;
    }

}
