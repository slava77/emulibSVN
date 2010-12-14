package org.cern.cms.csc.dw.monitor;

import java.io.File;
import java.io.FileInputStream;
import java.net.URL;
import java.util.Date;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.Properties;
import java.util.Set;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.model.monitor.MonitorObject;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

public class MonitorRetentionJob extends MonitorJobBase {

    private static final Logger logger = Logger.getLogger(MonitorRetentionJob.class);
    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/monitor_entities.properties";
    private static final long RETENTION_TIME = 24 * 60 * 60 * 1000;

    private Set<Class<? extends MonitorObject>> moClasses = new HashSet<Class<? extends MonitorObject>>();

    public MonitorRetentionJob() {
        try {
            URL entitiesUrl = MonitorRetentionJob.class.getResource(ENTITIES_RESOURCE);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                Class<? extends MonitorObject> clazz = (Class<? extends MonitorObject>) Class.forName(entitiesPro.getProperty(k));
                this.moClasses.add(clazz);
            }
        } catch (Exception ex) {
            logger.fatal("Error while initializing MonitorRetentionJob", ex);
        }
    }

    public void execute(JobExecutionContext context) throws JobExecutionException {
        Date d = new Date();
        d.setTime(d.getTime() - RETENTION_TIME);
        for (Class<? extends MonitorObject> clazz : this.moClasses) {
            monitorDao.retentMonitorObjects(clazz, d);
        }
    }

}
