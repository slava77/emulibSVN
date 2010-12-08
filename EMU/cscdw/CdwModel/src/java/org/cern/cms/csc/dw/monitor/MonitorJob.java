package org.cern.cms.csc.dw.monitor;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Queue;
import javax.jms.QueueBrowser;
import javax.jms.Session;
import javax.naming.Binding;
import javax.naming.NamingException;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.model.monitor.MonitorDatabaseStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.quartz.JobExecutionContext;
import org.quartz.JobExecutionException;

public class MonitorJob extends MonitorJobBase {

    private static final Logger log = Logger.getLogger(MonitorJob.class);
    private static List<QueueItem> queues = new ArrayList<QueueItem>();

    static {
        try {
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
                Integer size = qi.getSize();
                if (size > 0) {
                    MonitorQueueStatus qstatus = new MonitorQueueStatus();
                    qstatus.setQueueName(qi.getName());
                    qstatus.setQueueSize(qi.getSize());
                    monitor.trace(qstatus);
                }
            } catch (JMSException ex) {
                log.error("Error while accessing queues", ex);
            }
        }

        try {
            monitorDao.getSysdate();
        } catch (Exception ex) {
            MonitorDatabaseStatus dbstatus = new MonitorDatabaseStatus();
            dbstatus.setAlive(false);
            monitor.trace(dbstatus);
            log.error("Error while accessing database", ex);
        }

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
