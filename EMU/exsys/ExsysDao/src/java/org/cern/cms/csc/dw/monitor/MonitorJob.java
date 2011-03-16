package org.cern.cms.csc.dw.monitor;

import java.io.File;
import java.io.FileInputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import javax.ejb.EJB;
import javax.ejb.Schedule;
import javax.ejb.Stateless;
import javax.naming.Binding;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.monitor.MonitorDatabaseStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorSystem;
import org.cern.cms.csc.dw.util.JmsWorker;
import org.cern.cms.csc.dw.util.JmsWorkerJni;
import org.cern.cms.csc.dw.util.ServiceLocator;

@Stateless
public class MonitorJob {

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/monitor_entities.properties";
    private static final long RETENTION_TIME = 24 * 60 * 60 * 1000;

    private static final Logger logger = SimpleLogger.getLogger(MonitorJob.class);
    private Logger monitor;

    @EJB
    private MonitorDaoLocal monitorDao;

    private SysMonitor sysmon = new SysMonitor();
    private ServiceLocator locator;
    private List<JmsWorker> queues = new ArrayList<JmsWorker>();
    private Set<Class<? extends MonitorEntity>> moClasses = new HashSet<Class<? extends MonitorEntity>>();

    public MonitorJob() {

        try {

            this.locator = ServiceLocator.getInstance();

            for (Binding b: locator.getJniBindings("jms")) {
                if (!b.getName().endsWith("Factory")) {
                    if (JmsWorkerJni.isJmsService(b.getName())) {
                        queues.add(new JmsWorkerJni(b.getName()));
                    }
                }
            }

            URL entitiesUrl = MonitorJob.class.getResource(ENTITIES_RESOURCE);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                Class<? extends MonitorEntity> clazz = (Class<? extends MonitorEntity>) Class.forName(entitiesPro.getProperty(k));
                this.moClasses.add(clazz);
            }

        }catch(Exception ex) {
            logger.error("Error while initializing MonitorJob static objects", ex);
        }
    }

    @Schedule(hour="*", minute="*", second="*/10")
    public void monitorQueues() {

        logger.debug("In monitorQueues");
        for (JmsWorker qi: queues) {
            Integer size = qi.getSize();
            if (size == null) { return; }
            logger.trace("Queue {0} size = {1}", qi.getName(), size);
            if (size > 0) {
                MonitorQueueStatus qstatus = new MonitorQueueStatus();
                qstatus.setQueueName(qi.getName());
                qstatus.setQueueSize(size);
                getMonitor().trace(qstatus);
            }
        }
    }

    @Schedule(hour="*", minute="*", second="*/10")
    public void monitorSystem() {
        logger.debug("In monitorSystem");
        logger.trace("Up time: " + sysmon.getUpTime());
        MonitorSystem ms = new MonitorSystem(sysmon.getCpuUsage(), sysmon.getRamUsedPercentage(), sysmon.getSwapUsedPercentage());
        logger.trace("RAM: " + sysmon.getRamAsString());
        logger.trace("Swap: " + sysmon.getSwapAsString());
        logger.trace("CPU usage: " + String.valueOf(ms.getCpu()));
        getMonitor().trace(ms);
    }

    @Schedule(hour="*", minute="*", second="*/15")
    public void monitorDatabase() {
        logger.debug("In monitorDatabase");
        try {
            Date date = monitorDao.getSysdate();
            logger.trace("Database sysdate = {0}", date);
        } catch (Exception ex) {
            MonitorDatabaseStatus dbstatus = new MonitorDatabaseStatus();
            dbstatus.setAlive(false);
            getMonitor().trace(dbstatus);
            logger.error("Error while accessing database", ex);
        }

    }

    @Schedule(hour="0/2", minute="0", second="0")
    public void monitorRetention() {
        logger.debug("In monitorRetention");
        Date d = new Date();
        d.setTime(d.getTime() - RETENTION_TIME);
        for (Class<? extends MonitorEntity> clazz : this.moClasses) {
            logger.trace("Removing {0} objects earlier than {1}", clazz, d);
            monitorDao.retentMonitorObjects(clazz, d);
        }
    }

    public Logger getMonitor() {
        if (monitor == null) {
            this.monitor = MonitorLogger.getLogger(MonitorJob.class);
        }
        return monitor;
    }


}
