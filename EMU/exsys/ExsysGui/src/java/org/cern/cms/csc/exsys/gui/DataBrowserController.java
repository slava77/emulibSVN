package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.BrowserController;
import org.cern.cms.csc.exsys.gui.component.BeanTable;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import org.cern.cms.csc.dw.dao.MainBeanTableDaoLocal;
import org.cern.cms.csc.dw.dao.table.BeanTableDaoIf;
import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

@ManagedBean
@SessionScoped
public class DataBrowserController extends BrowserController {

    private static final Logger logger = SimpleLogger.getLogger(DataBrowserController.class);

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/main_entities.properties";
    private static final int [] PAGE_SIZES = {5, 10, 15, 20, 25};

    @EJB
    private MainBeanTableDaoLocal mainBeanTableDao;

    public DataBrowserController() {
        super(ENTITIES_RESOURCE);
    }

    @Override
    protected BeanTable createTable(Class forClass) throws Exception {
        return new BeanTable(forClass.getSimpleName(), forClass, 25, 5, PAGE_SIZES) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return mainBeanTableDao;
            }
        };
    }


}
