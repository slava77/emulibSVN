package jsf.bean.gui.component.table.column;

import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.component.table.BeanTableListFilter;
import jsf.bean.gui.component.table.BeanTablePack;
import jsf.bean.gui.component.table.BeanTablePackFilter;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public class BeanTableColumnEntityList extends BeanTableColumnEntity {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnEntityList.class);
    private Class itemType = null;

    public BeanTableColumnEntityList(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);
        this.itemType = BeanTableColumnFactory.getParametrizedType(propertyMd);
    }

    @Override
    public boolean isListType() {
        return true;
    }
   
    @Override
    protected Class getFilterType() {
        return itemType;
    }
    
    @SuppressWarnings("unchecked")
    public void setListPropertyTable(EntityBeanBase currentItem) throws Exception {
        if (isEntityType()) {
            setFilterTablePack();
            BeanTablePackFilter listFilter = new BeanTableListFilter(table.getRowClass(), currentItem, getName());
            BeanTablePack btp = new BeanTablePack(getName(),
                    String.format("List of '%s' %s", currentItem.getEntityTitle(), getTitle()),
                    table.getPack().getManager(),
                    itemType);
            btp.addFilter(listFilter);
            table.getPack().getManager().pushTable(btp);
        }
    }
    
}
