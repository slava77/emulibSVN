package org.cern.cms.csc.exsys.gui;

import org.cern.cms.csc.exsys.gui.base.BrowserController;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;
import javax.ejb.EJB;
import javax.faces.bean.ManagedBean;
import javax.faces.bean.SessionScoped;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import javax.naming.Binding;
import javax.naming.NamingException;
import jsf.bean.gui.ClassFinderIf;
import jsf.bean.gui.component.BeanTableManager;
import jsf.bean.gui.component.table.BeanTableDaoIf;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.MonitorDaoLocal;
import org.cern.cms.csc.dw.model.monitor.MonitorFactCollectionLog;
import org.cern.cms.csc.dw.model.monitor.MonitorQueueStatus;
import org.cern.cms.csc.dw.model.monitor.MonitorSystem;
import org.cern.cms.csc.dw.monitor.SysMonitor;
import org.cern.cms.csc.dw.util.JmsWorker;
import org.cern.cms.csc.dw.util.JmsWorkerJni;
import org.cern.cms.csc.dw.util.ServiceLocator;
import org.cern.cms.csc.exsys.gui.util.ClassFinder;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartUtilities;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.XYBarRenderer;
import org.jfree.chart.renderer.xy.XYDotRenderer;
import org.jfree.chart.renderer.xy.XYLineAndShapeRenderer;
import org.jfree.data.time.Second;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.time.TimeSeriesDataItem;

@ManagedBean
@SessionScoped
public class MonitorController extends BrowserController {

    private static final Logger logger = SimpleLogger.getLogger(MonitorController.class);
    private static final String ENTITIES_RESOURCE = "/org/cern/cms/csc/dw/metadata/monitor_entities.properties";

    private ServiceLocator locator = new ServiceLocator();

    @EJB
    private MonitorDaoLocal monitorDao;

    private SysMonitor sysMonitor = new SysMonitor();

    private List<JmsWorker> queues = new ArrayList<JmsWorker>();
    private Integer chartLastHours = 8;

    private byte[] statusImage = null;
    private byte[] systemImage = null;

    public MonitorController() throws NamingException  {
        super(ENTITIES_RESOURCE);

        for (Binding b : locator.getJniBindings("jms")) {
            try {
                if (JmsWorkerJni.isJmsService(b.getName())) {
                    queues.add(new JmsWorkerJni(b.getName()));
                }
            } catch (NamingException ex) {
                logger.info("Naming exception while looking for queues", ex);
            }
        }

    }

    public SysMonitor getSysMonitor() {
        return sysMonitor;
    }

    public List<JmsWorker> getQueues() {
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
        refreshStatusImage();
        refreshSystemImage();
    }

    public List<SelectItem> getChartLastHoursItems() {
        List<SelectItem> items = new ArrayList<SelectItem>();
        for (int i = 1; i <= 24; i++) {
            items.add(new SelectItem(i, String.valueOf(i)));
        }
        return items;
    }

    public byte[] getStatusImage() {
        if (statusImage == null) {
            refreshStatusImage();
        }
        return statusImage;
    }

    public byte[] getSystemImage() {
        if (systemImage == null) {
            refreshSystemImage();
        }
        return systemImage;
    }

    public void refreshAllImagesListener(ActionEvent ev) {
        refreshSystemImage();
        refreshStatusImage();
    }

    public void refreshStatusImage() {

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
            plot.getRangeAxis(0).setUpperBound(maxValue > 100 ? maxValue.doubleValue() : 100.0d);
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
            statusImage = ChartUtilities.encodeAsPNG(chart.createBufferedImage(1000, 400));
        } catch (IOException ex) {
            statusImage = null;
            logger.error("Retrieving status chart image", ex);
        }

    }

    private void refreshSystemImage() {

        TimeSeriesCollection load = new TimeSeriesCollection();

        TimeSeries cpu = new TimeSeries("CPU load");
        load.addSeries(cpu);
        TimeSeries ram = new TimeSeries("RAM");
        load.addSeries(ram);
        TimeSeries swap = new TimeSeries("Swap");
        load.addSeries(swap);

        for (MonitorSystem ms: monitorDao.getMonitorObjects(MonitorSystem.class, chartLastHours)) {
            cpu.add(new Second(ms.getTime()), ms.getCpu());
            ram.add(new Second(ms.getTime()), ms.getRam());
            swap.add(new Second(ms.getTime()), ms.getSwap());
        }

        JFreeChart chart = ChartFactory.createTimeSeriesChart(
                "CPU load and memory",
                "Time",
                "%",
                load,
                true, true, false);

        XYPlot plot = chart.getXYPlot();

        {
            plot.getRangeAxis(0).setLowerBound(0.0);
            plot.getRangeAxis(0).setUpperBound(1.0);
        }

        {
            Date from = new Date();
            from.setTime(from.getTime() - chartLastHours * 60 * 60 * 1000);
            plot.getDomainAxis(0).setLowerBound(new Second(from).getFirstMillisecond());
            plot.getDomainAxis(0).setUpperBound(new Second(new Date()).getLastMillisecond());
        }

        XYLineAndShapeRenderer renderer0 = new XYLineAndShapeRenderer(true, false);
        plot.setRenderer(0, renderer0);
        XYLineAndShapeRenderer renderer1 = new XYLineAndShapeRenderer(true, false);
        plot.setRenderer(1, renderer1);
        XYLineAndShapeRenderer renderer2 = new XYLineAndShapeRenderer(true, false);
        plot.setRenderer(2, renderer2);

        try {
            systemImage = ChartUtilities.encodeAsPNG(chart.createBufferedImage(1000, 400));
        } catch (IOException ex) {
            systemImage = null;
            logger.error("Retrieving system chart image", ex);
        }

    }

    @Override
    protected BeanTableManager createTable(Class forClass) throws Exception {
        return new BeanTableManager(forClass.getSimpleName(), forClass) {
            @Override
            public BeanTableDaoIf getBeanTableDao() {
                return monitorDao;
            }
            @Override
            public ClassFinderIf getClassFinder() {
                return ClassFinder.getInstance();
            }
        };
    }

    public void flushQueueMessageListener(ActionEvent ev) {
        int index = Integer.parseInt((String) getParameter("index"));
        getQueues().get(index).flushObjectMessage();
    }

}
