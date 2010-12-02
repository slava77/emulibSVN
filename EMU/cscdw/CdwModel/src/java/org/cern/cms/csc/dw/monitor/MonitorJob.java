package org.cern.cms.csc.dw.monitor;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import javax.ejb.EJB;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Queue;
import javax.jms.QueueBrowser;
import javax.jms.Session;
import javax.naming.Binding;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.model.monitor.MonitorDatabaseStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;
import org.quartz.StatefulJob;

@EJB(name="MonitorDaoRef", beanInterface=MonitorDaoLocal.class)
public class MonitorJob implements StatefulJob {

    private static final Logger log = Logger.getLogger(MonitorJob.class);
    private static final Logger monitor = MonitorLogger.getLogger();

    private static ServiceLocator locator;
    private static MonitorDaoLocal monitorDao;
    private static List<QueueItem> queues = new ArrayList<QueueItem>();

    static {
        try {
            locator = ServiceLocator.getInstance();
            monitorDao = (MonitorDaoLocal) locator.getEnvService("MonitorDaoRef");
            for (Binding b: locator.getJniBindings("jms")) {
                if (!b.getName().endsWith("Factory")) {
                    queues.add(new QueueItem(b.getName()));
                }
            }
        }catch(NamingException ex) {
            log.fatal("Error while initializing MonitorJob static objects", ex);
        }
    }

    public void execute(JobExecutionContext context) throws JobExecutionException {

        for (QueueItem qi: queues) {
            try {
                MonitorQueueStatus qstatus = new MonitorQueueStatus();
                qstatus.setQueueName(qi.getName());
                qstatus.setQueueSize(qi.getSize());
                monitor.trace(qstatus);
            } catch (JMSException ex) {
                log.error("Error while accessing queues", ex);
            }
        }

        MonitorDatabaseStatus dbstatus = new MonitorDatabaseStatus();
        try {
            dbstatus.setAlive(monitorDao.getSysdate() != null);
        } catch (Exception ex) {
            dbstatus.setAlive(false);
            log.error("Error while accessing database", ex);
        }
        monitor.trace(dbstatus);

    }

    private static class QueueItem {

        private Queue queue;
        private ConnectionFactory connectionFactory;

        public QueueItem(String name) throws NamingException {
            this.queue = (Queue) locator.getService("jms/" + name);
            this.connectionFactory = (ConnectionFactory) locator.getService("jms/" + name + "Factory");
        }

        public String getName() throws JMSException {
            return queue.getQueueName();
        }

        public Integer getSize() throws JMSException {
            Connection connection = null;
            Session session = null;
            try {
                connection = connectionFactory.createConnection();
                session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
                QueueBrowser browser = session.createBrowser(queue);
                Enumeration en = browser.getEnumeration();
                Integer num = 0;
                while (en.hasMoreElements()) {
                    en.nextElement();
                    num++;
                }
                return num;
            } finally {
                if (session != null) {
                    try {
                        session.close();
                    } catch (JMSException e) {
                        log.error("Error while closing JMS Session", e);
                    }
                }
                if (connection != null) {
                    connection.close();
                }
            }
        }

    }

}
