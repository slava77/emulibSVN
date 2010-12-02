package org.cern.cms.csc.dw.gui.jsf;

import org.cern.cms.csc.dw.gui.table.*;
import javax.ejb.EJB;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.util.ServiceLocator;

@EJB(name="MainBeanTableDaoRef", beanInterface=MainBeanTableDaoLocal.class)
public class MainBeanTable extends BeanTableBase {

    private MainBeanTableDaoLocal tableDao;

    public MainBeanTable(String title, Class<? extends EntityBase> rowClass, boolean interactiveMode) throws Exception {
        super(title, rowClass, interactiveMode);
        this.tableDao = (MainBeanTableDaoLocal) ServiceLocator.getInstance().getEnvService("MainBeanTableDaoRef");
    }

    @Override
    protected BeanTableDaoIf getBeanTableDao() {
        return this.tableDao;
    }

}
