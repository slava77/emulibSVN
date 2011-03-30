package org.cern.cms.csc.dw.monitor;

import java.util.ArrayList;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;
import javax.annotation.Resource;
import javax.ejb.EJB;
import javax.ejb.Schedule;
import javax.ejb.Singleton;
import javax.ejb.Timeout;
import javax.ejb.TimerConfig;
import javax.ejb.TimerService;
import javax.naming.Binding;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.metadata.MetadataManager;
import org.cern.cms.csc.dw.model.monitor.MonitorDatabaseStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorEntity;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorSystem;
import org.cern.cms.csc.dw.util.JmsWorker;
import org.cern.cms.csc.dw.util.JmsWorkerJni;
import org.cern.cms.csc.dw.util.ServiceLocator;

@Singleton
public class MonitorJob {

    private static final String ENTITIES_RESOURCE = MetadataManager.RESOURCE_BASE + MetadataManager.MONITOR_ENTITIES_RESOURCE;
    private static final long RETENTION_TIME = 7 * 24 * 60 * 60 * 1000;
    private static final int CRITICAL_QUEUE_SIZE = 20000;
    private static final int FATAL_QUEUE_SIZE = 99000;

    private static final Logger logger = ExsysLogger.getLogger(MonitorJob.class);
    private Logger monitor;

    @EJB
    private MonitorDaoLocal monitorDao;

    @Resource
    private TimerService timerService;

    private SysMonitor sysmon = new SysMonitor();
    private ServiceLocator locator;
    private List<JmsWorker> queues = new ArrayList<JmsWorker>();
    private Set<Class<? extends MonitorEntity>> moClasses;

    private boolean initialized = false;
    private boolean initTimeoutStarted = false;

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

        }catch(Exception ex) {
            logger.error("Error while initializing MonitorJob static objects", ex);
        }
    }

    private Set<Class<? extends MonitorEntity>> getMonitorEntityClasses() {
        try {
            if (moClasses == null) {
                moClasses = new HashSet<Class<? extends MonitorEntity>>();
                Properties entitiesPro = new Properties();
                entitiesPro.load(MonitorJob.class.getResourceAsStream(ENTITIES_RESOURCE));
                Enumeration en = entitiesPro.propertyNames();
                while (en.hasMoreElements()) {
                    String k = (String) en.nextElement();
                    Class<? extends MonitorEntity> clazz = (Class<? extends MonitorEntity>) Class.forName(entitiesPro.getProperty(k));
                    this.moClasses.add(clazz);
                }
            }
        } catch (Throwable ex) {
            moClasses = null;
            logger.error("Error while getting monitor entity classes from the properties file: " + ENTITIES_RESOURCE, ex);
        }
        return moClasses;
    }

    /** Until this method is called, no errors will be logged - this gives time for the system to start up */
    @Timeout()
    public void initTimeout() {
        logger.trace("Initialized!");
        initialized = true;
    }

    @Schedule(hour="*", minute="*", second="*/10")
    public void monitorQueues() {

        logger.debug("In monitorQueues");
        for (JmsWorker qi: queues) {
            Integer size = qi.getSize();
            if (size == null) { return; }
            logger.trace("Queue {0} size = {1}", qi.getName(), size);
            if (size > 0) {
                String queueName = qi.getName();
                MonitorQueueStatus qstatus = new MonitorQueueStatus();
                qstatus.setQueueName(queueName);
                qstatus.setQueueSize(size);
                getMonitor().info(qstatus);
                if (size >= FATAL_QUEUE_SIZE) {
                    logger.fatal(queueName + " queue size is above FATAL threshold, size = " + size);
                }
                if (size >= CRITICAL_QUEUE_SIZE) {
                    logger.critical(queueName + " queue size is above critical threshold, size = " + size);
                }
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
        getMonitor().info(ms);
    }

    @Schedule(hour="*", minute="*", second="*/15")
    public void monitorDatabase() {
        logger.debug("In monitorDatabase");
        if (!initTimeoutStarted) { // can't do in constructor nor in the postConstruct, so doing like this..
            timerService.createSingleActionTimer(60000, new TimerConfig());
            initTimeoutStarted = true;
        }
        
        try {
            if (initialized) { // if we start doing this too early, oftentimes you'll find DB not accessible very shortly after deployment
                Date date = monitorDao.getSysdate();
                logger.trace("Database sysdate = {0}", date);
            }
        } catch (Exception ex) {
                MonitorDatabaseStatus dbstatus = new MonitorDatabaseStatus();
                dbstatus.setAlive(false);
                getMonitor().info(dbstatus);
                logger.critical("Error while accessing database", ex);
        }

    }

    @Schedule(hour="0/2", minute="0", second="0")
    public void monitorRetention() {
        logger.debug("In monitorRetention");
        Date d = new Date();
        d.setTime(d.getTime() - RETENTION_TIME);
        for (Class<? extends MonitorEntity> clazz : this.getMonitorEntityClasses()) {
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
