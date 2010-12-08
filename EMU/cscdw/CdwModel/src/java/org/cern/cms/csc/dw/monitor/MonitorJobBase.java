package org.cern.cms.csc.dw.monitor;

import javax.ejb.EJB;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.quartz.StatefulJob;

@EJB(name="MonitorDaoRef", beanInterface=MonitorDaoLocal.class)
public abstract class MonitorJobBase implements StatefulJob {

    private static final Logger log = Logger.getLogger(MonitorJobBase.class);

    protected static final Logger monitor = MonitorLogger.getLogger();
    protected static ServiceLocator locator;
    protected static MonitorDaoLocal monitorDao;

    static {
        try {

            locator = ServiceLocator.getInstance();
            monitorDao = (MonitorDaoLocal) locator.getEnvService("MonitorDaoRef");

        }catch(NamingException ex) {
            log.fatal("Error while initializing MonitorJobBase static objects", ex);
        }
    }


}
