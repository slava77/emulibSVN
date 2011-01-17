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
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.gui.table.BeanTableBase;
import org.cern.cms.csc.dw.gui.table.BeanTableControllerBase;
import org.cern.cms.csc.dw.gui.table.SelectItemComparator;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.monitor.MonitorFactCollectionLog;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYBarRenderer;
import org.jfree.chart.renderer.xy.XYDotRenderer;
import org.jfree.data.time.Second;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.time.TimeSeriesDataItem;

@EJB(name = "MonitorDaoRef", beanInterface = MonitorDaoLocal.class)
public class MonitorController extends BeanTableControllerBase {

    private static final Logger logger = Logger.getLogger(MonitorController.class);
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
            logger.error("IO error", ex);
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
    private Integer chartLastHours = 8;
    private byte[] chartImage = null;

    public MonitorController() throws NamingException {
        this.locator = ServiceLocator.getInstance();
        monitorDao = (MonitorDaoLocal) locator.getEnvService("MonitorDaoRef");
        for (Binding b : locator.getJniBindings("jms")) {
            try {
                Object queueObj = locator.getService("jms/" + b.getName());
                if (queueObj instanceof Queue) {
                    Object factoryObj = locator.getService("jms/" + b.getName() + "Factory");
                    if (factoryObj instanceof ConnectionFactory) {
                        queues.add(new QueueItem(b.getName(), (Queue) queueObj, (ConnectionFactory) factoryObj));
                    }
                }
            } catch (NamingException ex) {
                logger.info("Naming exception while looking for queues", ex);
            }
        }
    }

    public Collection<QueueItem> getQueues() {
        return queues;
    }

    public Date getDatabaseDate() {
        return monitorDao.getSysdate();
    }

    public Integer getChartLastHours() {
        return chartLastHours;
    }

    public void setChartLastHours(Integer chartLastHours) {
        this.chartLastHours = chartLastHours;
        refreshChartImageListener(null);
    }

    public List<SelectItem> getChartLastHoursItems() {
        List<SelectItem> items = new ArrayList<SelectItem>();
        for (int i = 1; i <= 24; i++) {
            items.add(new SelectItem(i, String.valueOf(i)));
        }
        return items;
    }

    public byte[] getChartImage() {
        if (chartImage == null) {
            refreshChartImageListener(null);
        }
        return chartImage;
    }

    public void refreshChartImageListener(ActionEvent ev) {

        TimeSeriesCollection queueSizes = new TimeSeriesCollection();
        Integer maxValue = 0;

        for (MonitorQueueStatus qs: monitorDao.getMonitorObjects(MonitorQueueStatus.class, chartLastHours)) {
            if (qs.getQueueName() == null) {
                qs.setQueueName("null");
            }
            TimeSeries ts = queueSizes.getSeries(qs.getQueueName());
            if (ts == null) {
                ts = new TimeSeries(qs.getQueueName());
                queueSizes.addSeries(ts);
            }
            ts.add(new Second(qs.getTime()), qs.getQueueSize());
            if (maxValue < qs.getQueueSize()) maxValue = qs.getQueueSize();
        }

        TimeSeriesCollection fColls = new TimeSeriesCollection();
        for (MonitorFactCollectionLog fc: monitorDao.getMonitorObjects(MonitorFactCollectionLog.class, chartLastHours)) {
            if (fc.getProvider() == null) {
                fc.setProvider("null");
            }
            TimeSeries ts = fColls.getSeries(fc.getProvider());
            if (ts == null) {
                ts = new TimeSeries(fc.getProvider());
                fColls.addSeries(ts);
            }
            Second sec = new Second(fc.getTime());
            TimeSeriesDataItem dataItem = ts.getDataItem(sec);
            Integer value = fc.getNumberOfFacts();
            if (dataItem != null) {
                value += dataItem.getValue().intValue();
            }
            ts.addOrUpdate(sec, value);
            if (maxValue < value) maxValue = value;
        }

        JFreeChart chart = ChartFactory.createTimeSeriesChart(
                "Facts and Queue Sizes",
                "Time",
                "Size",
                queueSizes,
                true, true, false);

        XYPlot plot = chart.getXYPlot();

        {
            plot.getRangeAxis(0).setLowerBound(0.0);
            plot.getRangeAxis(0).setUpperBound(maxValue.doubleValue());
        }

        {
            Date from = new Date();
            from.setTime(from.getTime() - chartLastHours * 60 * 60 * 1000);
            plot.getDomainAxis(0).setLowerBound(new Second(from).getFirstMillisecond());
            plot.getDomainAxis(0).setUpperBound(new Second(new Date()).getLastMillisecond());
        }

        /*
        NumberAxis axis2 = new NumberAxis("Facts");
        chart.getXYPlot().setRangeAxis(1, axis2);
        chart.getXYPlot().setRangeAxisLocation(1, AxisLocation.BOTTOM_OR_RIGHT);
        */
        plot.setDataset(1, fColls);

        XYBarRenderer renderer0 = new XYBarRenderer();
        plot.setRenderer(0, renderer0);

        XYDotRenderer renderer1 =  new XYDotRenderer();
        renderer1.setDotHeight(2);
        renderer1.setDotWidth(2);
        plot.setRenderer(1, renderer1);

        try {
            chartImage = ChartUtilities.encodeAsPNG(chart.createBufferedImage(1000, 600));
        } catch (IOException ex) {
            chartImage = null;
            logger.error("Retrieving chart image", ex);
        }

    }

    public class QueueItem {

        private String name;
        private Queue queue;
        private ConnectionFactory connectionFactory;

        public QueueItem(String name, Queue queue, ConnectionFactory connectionFactory) throws NamingException {
            this.name = name;
            this.queue = queue;
            this.connectionFactory = connectionFactory;
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
                        logger.error("Closing queue session", e);
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
                        logger.error("Closing queue session", e);
                    }
                }
                if (connection != null) {
                    connection.close();
                }
            }
        }
    }
}
