package org.cern.cms.csc.dw.gui.jsf;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.Enumeration;
import java.util.List;
import java.util.Properties;
import java.util.SortedSet;
import java.util.TreeSet;
import javax.ejb.EJB;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageConsumer;
import javax.jms.Queue;
import javax.jms.QueueBrowser;
import javax.jms.Session;
import javax.naming.Binding;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.gui.table.BeanTableBase;
import org.cern.cms.csc.dw.gui.table.BeanTableControllerBase;
import org.cern.cms.csc.dw.gui.table.SelectItemComparator;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.data.time.Second;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;

@EJB(name = "MonitorDaoRef", beanInterface = MonitorDaoLocal.class)
public class MonitorController extends BeanTableControllerBase {

    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/monitor_entities.properties";
    private static final SortedSet<SelectItem> entities = new TreeSet<SelectItem>(new SelectItemComparator());

    static {
        try {
            URL entitiesUrl = BeanTableControllerBase.class.getResource(ENTITIES_RESOURCE);
            Properties entitiesPro = new Properties();
            entitiesPro.load(new FileInputStream(new File(entitiesUrl.getFile())));
            Enumeration en = entitiesPro.propertyNames();
            while (en.hasMoreElements()) {
                String k = (String) en.nextElement();
                SelectItem si = new SelectItem(entitiesPro.getProperty(k), k);
                entities.add(si);
            }
        } catch (IOException ex) {
            ex.printStackTrace(System.err);
        }
    }

    @Override
    public Collection<SelectItem> getEntities() {
        return entities;
    }

    @Override
    public BeanTableBase getBeanTable(String title, Class<? extends EntityBase> rowClass) throws Exception {
        return new MonitorBeanTable(title, rowClass, true);
    }
    
    private ServiceLocator locator;
    private MonitorDaoLocal monitorDao;
    private List<QueueItem> queues = new ArrayList<QueueItem>();

    public MonitorController() throws NamingException {
        this.locator = ServiceLocator.getInstance();
        monitorDao = (MonitorDaoLocal) locator.getEnvService("MonitorDaoRef");
        for (Binding b : locator.getJniBindings("jms")) {
            if (!b.getName().endsWith("Factory")) {
                queues.add(new QueueItem(b.getName()));
            }
        }
    }

    public Collection<QueueItem> getQueues() {
        return queues;
    }

    public Date getDatabaseDate() {
        return monitorDao.getSysdate();
    }

    public byte[] getChartImage() {

        TimeSeriesCollection tsds = new TimeSeriesCollection();
        for (MonitorQueueStatus qs: monitorDao.getQueueStatusData(24)) {
            TimeSeries ts = tsds.getSeries(qs.getQueueName());
            if (ts == null) {
                ts = new TimeSeries(qs.getQueueName());
                tsds.addSeries(ts);
            }
            ts.add(new Second(qs.getTime()), qs.getQueueSize());
        }


        JFreeChart chart = ChartFactory.createXYStepChart(
                "Queue Sizes and Database status",
                "Time",
                "Size",
                tsds,
                PlotOrientation.VERTICAL,
                true, true, false);

        chart.getXYPlot().getRangeAxis(0).setLowerBound(0.0);
        
        try {
            byte[] image = ChartUtilities.encodeAsPNG(chart.createBufferedImage(800, 600));
            return image;
        } catch (IOException ex) {
            ex.printStackTrace();
        }
        
        return null;
    }

    public class QueueItem {

        private String name;
        private Queue queue;
        private ConnectionFactory connectionFactory;

        public QueueItem(String name) throws NamingException {
            this.name = name;
            this.queue = (Queue) locator.getService("jms/" + name);
            this.connectionFactory = (ConnectionFactory) locator.getService("jms/" + name + "Factory");
        }

        public String getName() {
            return name;
        }

        public Queue getQueue() {
            return queue;
        }

        public ConnectionFactory getConnectionFactory() {
            return connectionFactory;
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
                        e.printStackTrace(System.err);
                    }
                }
                if (connection != null) {
                    connection.close();
                }
            }
        }

        public void flushMessageListener(ActionEvent ev) throws JMSException {
            Connection connection = null;
            Session session = null;

            try {
                connection = connectionFactory.createConnection();
                session = connection.createSession(false, Session.AUTO_ACKNOWLEDGE);
                MessageConsumer consumer = session.createConsumer(queue);
                Message msg = consumer.receiveNoWait();
            } finally {
                if (session != null) {
                    try {
                        session.close();
                    } catch (JMSException e) {
                        e.printStackTrace(System.err);
                    }
                }
                if (connection != null) {
                    connection.close();
                }
            }
        }
    }
}
