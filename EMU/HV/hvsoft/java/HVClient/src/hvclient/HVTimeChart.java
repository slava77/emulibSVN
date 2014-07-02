package hvclient;

import java.awt.Color;
import java.awt.BasicStroke;
import java.awt.BorderLayout;
import javax.swing.*;
import org.jfree.data.time.TimeSeries;
import org.jfree.data.time.TimeSeriesCollection;
import org.jfree.data.time.Second;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.DateAxis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.plot.XYPlot;
import org.jfree.chart.renderer.xy.*;
/**
 * <p>Title: </p>
 *
 * <p>Description: </p>
 *
 * <p>Copyright: Copyright (c) 2004</p>
 *
 * <p>Company: </p>
 *
 * @author not attributable
 * @version 1.0
 */
public class HVTimeChart
    extends JPanel {
  private TimeSeries total;

  private int numSeries = 1;
  private TimeSeries[] series;
  private String axisTitle = "Total";
  private String serTitle = "Total";
  private int history = 300;
  private  TimeSeriesCollection dataset;
  public HVTimeChart(int seriesN, String sTitle, String aTitle, int hist) {
    super(new BorderLayout());
    numSeries = seriesN;
    axisTitle = aTitle;
    serTitle = sTitle;
    history = hist;
    create();
  }
  public HVTimeChart() {
    super(new BorderLayout());
    create();
  }

  public int getNumSeries() {
    return numSeries;
  }
  public void setNumSeries(int num) {
    numSeries = num;
    createSeries(num);
  }

  public void setSerieTitle(int serNum, String title) {
    if (serNum>=0 && serNum <numSeries) {
      // series[serNum].setDescription(title);
      series[serNum].setKey(title);
//      series[serNum].set
    }
  }

  private void createSeries(int num) {
    dataset.removeAllSeries();
    series = new TimeSeries[numSeries];
    for (int i = 0; i < numSeries; i++) {
      series[i] = new TimeSeries((i==0)?"ALL":serTitle + new Integer(i).toString());
      series[i].setMaximumItemCount(history);// setHistoryCount(history);
      dataset.addSeries(series[i]);
    }

  }
  private void create() {
    dataset = new TimeSeriesCollection();
    createSeries(numSeries);
    DateAxis domain = new DateAxis("Time");
    NumberAxis range = new NumberAxis(axisTitle);
    XYPlot xyplot = new XYPlot(dataset, domain, range, new XYLineAndShapeRenderer(true, false));
    XYItemRenderer renderer = xyplot.getRenderer();
  //    renderer.setSeriesPaint(0, Color.red);
  //   renderer.setSeriesPaint(1, Color.green);
    renderer.setBaseStroke(
        new BasicStroke(2f, BasicStroke.CAP_BUTT, BasicStroke.JOIN_BEVEL)
        );
    domain.setAutoRange(true);
    domain.setLowerMargin(0.0);
    domain.setUpperMargin(0.0);
    domain.setTickLabelsVisible(true);
    // range.setStandardTickUnits(NumberAxis.createIntegerTickUnits());
    range.setStandardTickUnits(NumberAxis.createStandardTickUnits());

    JFreeChart chart = new JFreeChart(
        axisTitle,
        JFreeChart.DEFAULT_TITLE_FONT,
        xyplot,
        true
        );
    ChartPanel chartPanel = new ChartPanel(chart);
    add(chartPanel);
  }

  public void addValue(int seriesN, double y) {
    if (seriesN>=0 && seriesN<series.length)
      series[seriesN].addOrUpdate(new Second(), y);
}


}
