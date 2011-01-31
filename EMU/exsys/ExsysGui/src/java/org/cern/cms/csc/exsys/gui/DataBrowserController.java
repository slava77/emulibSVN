package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.BrowserController;
import org.cern.cms.csc.exsys.gui.component.BeanTableManager;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.context.FacesContext;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

@ManagedBean
@SessionScoped
public class DataBrowserController extends BrowserController {

    private static final Logger logger = SimpleLogger.getLogger(DataBrowserController.class);

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/main_entities.properties";

    @EJB
    private MainBeanTableDaoLocal mainBeanTableDao;

    public DataBrowserController() {
        super(ENTITIES_RESOURCE);
    }

    @Override
    protected BeanTableManager createTable(Class forClass) throws Exception {
        return new BeanTableManager(forClass.getSimpleName(), forClass) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return mainBeanTableDao;
            }
            @Override
            public FacesContext getContext() {
                return FacesContext.getCurrentInstance();
            }
        };
    }


}
