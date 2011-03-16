package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.BrowserController;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;

@ManagedBean
@SessionScoped
public class DataBrowserController extends BrowserController {

    private static final Logger logger = SimpleLogger.getLogger(DataBrowserController.class);

    private static final String ENTITIES_RESOURCE = MetadataManager.RESOURCE_BASE + MetadataManager.MAIN_ENTITIES_RESOURCE;

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
            public ClassFinderIf getClassFinder() {
                return ClassFinder.getInstance();
            }
        };
    }


}
