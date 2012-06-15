package jsf.bean.gui.component.table;

import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import org.hibernate.criterion.DetachedCriteria;

public interface BeanTableDaoIf {

    List<EntityBeanBase> getData(BeanTable table);

    Long getDataCount(BeanTable table);

    List<EntityBeanBase> getData(BeanTable table,
            int pageSize,
            int pageIndex);
    
    DetachedCriteria getDetachedCriteria(BeanTable table);
    
}
