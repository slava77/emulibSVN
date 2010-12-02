package org.cern.cms.csc.dw.gui.jsf;

import org.cern.cms.csc.dw.gui.table.*;
import javax.ejb.EJB;
import org.cern.cms.csc.dw.dao.MonitorBeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.util.ServiceLocator;

@EJB(name="MonitorBeanTableDaoRef", beanInterface=MonitorBeanTableDaoLocal.class)
public class MonitorBeanTable extends BeanTableBase {

    private MonitorBeanTableDaoLocal tableDao;

    public MonitorBeanTable(String title, Class<? extends EntityBase> rowClass, boolean interactiveMode) throws Exception {
        super(title, rowClass, interactiveMode);
        this.tableDao = (MonitorBeanTableDaoLocal) ServiceLocator.getInstance().getEnvService("MonitorBeanTableDaoRef");
    }

    @Override
    protected BeanTableDaoIf getBeanTableDao() {
        return this.tableDao;
    }

}
