package hvclient;

import java.awt.*;
import javax.swing.*;
import java.util.regex.Pattern;
import javax.swing.table.*;
import java.util.*;
import dim.*;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

/**
 * <p>Title: HVClient</p>
 *
 * <p>Description: HVClient Java GUI Application</p>
 *
 * <p>Copyright: Copyright (c) 2005</p>
 *
 * <p>Company: University of Florida</p>
 *
 * @author Victor Barashko <barvic@ufl.edu>
 * @version 1.0
 */
public class LogPanel
    extends JPanel {

  protected String[] headers = {
      "Date", "Time", "Level", "Message"};
  protected ArrayList<Vector<String>> data = new ArrayList<Vector<String>> ();
  protected ArrayList<String> filters = new ArrayList<String> ();
  protected ArrayList<JCheckBoxMenuItem> menuItems = new ArrayList<
      JCheckBoxMenuItem> ();
  protected int filterColumn = 2;

  BorderLayout borderLayout1 = new BorderLayout();
  private DimUpdatedInfo logInfo;
  private String logServiceName;
  JScrollPane jsp = new JScrollPane();
  DefaultTableModel dtm = new DefaultTableModel();
  JTable jt = new JTable(dtm);
  JPopupMenu menu;

  public LogPanel(String serviceName) {
    try {
      logServiceName = serviceName;
      jbInit();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  protected void finalize() {
    logInfo.releaseService();
  }

  public void setFilter(Vector<String> filter, int column) {
    filters.clear();
    filters = new ArrayList<String> (filter);
    dtm.setRowCount(0);
    Vector<Vector<String>> newdata = new Vector<Vector<String>> ();
    for (Vector<String> entry : data) {
      if (!filters.contains(entry.elementAt(filterColumn))) {
        dtm.addRow(entry);
      }
    }

    jt.revalidate();
    JScrollBar vbar = jsp.getVerticalScrollBar();
    vbar.setValue(vbar.getMaximum());
    vbar.invalidate();

  }

  private void jbInit() throws Exception {
    this.setLayout(borderLayout1);
    this.add(jsp);

    filters.clear();

    for (int i = 0; i < headers.length; i++) {
      dtm.addColumn(headers[i]);
    }

    jt.setSelectionForeground(Color.BLUE);
    jt.setRowSelectionAllowed(true);
    // Set validation column to 2 ("Level")
    jt.getColumnModel().getColumn(2).setCellRenderer(new LogLevelRenderer(2));
    jt.getColumnModel().getColumn(3).setCellRenderer(new LogLevelRenderer(2));
    jt.getColumn("Date").setMaxWidth(100);
    jt.getColumn("Time").setMaxWidth(100);
    jt.getColumn("Level").setMaxWidth(100);

    menu = CreatePopupMenu();

    jt.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent evt) {
        if (evt.isPopupTrigger()) {
          menu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
      }

      public void mouseReleased(MouseEvent evt) {
        if (evt.isPopupTrigger()) {
          menu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
      }
    });

    jsp.addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent evt) {
        if (evt.isPopupTrigger()) {
          menu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
      }

      public void mouseReleased(MouseEvent evt) {
        if (evt.isPopupTrigger()) {
          menu.show(evt.getComponent(), evt.getX(), evt.getY());
        }
      }
    });

    jsp.getViewport().add(jt);
    logInfo = new DimUpdatedInfo(logServiceName, "") {
      public void infoHandler() {
        String msg = getString();
        Vector<String>
            fields = new Vector<String> (Arrays.asList(msg.split(
            Pattern.quote("|"))));
        if (fields.size() == 5) {
          fields.removeElementAt(0);
          data.add(fields);
          if (!filters.contains(fields.elementAt(filterColumn))) {
            dtm.addRow(fields);
            jt.revalidate();
            JScrollBar vbar = jsp.getVerticalScrollBar();
            vbar.setValue(vbar.getMaximum());
            vbar.invalidate();
          }
        }

      }
    };

  }

  private JPopupMenu CreatePopupMenu() {
    String[] filters = {
        "DEBUG", "INFO", "WARN", "ERROR"};

    JPopupMenu menu = new JPopupMenu();

    menu.add(new JMenuItem("Filter Log Levels"));
    menu.addSeparator();
    menuItems.clear();
    for (String name : Arrays.asList(filters)) {
      JCheckBoxMenuItem item = new JCheckBoxMenuItem(name);
      menuItems.add(item);
      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (e.getSource() instanceof JMenuItem) {
            SetFilterParameters();
          }
        }
      });

      item.setSelected(true);
      menu.add(item);
    }
    return menu;
  }

  private void SetFilterParameters() {
    Vector<String> filter = new Vector<String> ();

    for (JCheckBoxMenuItem item : menuItems) {
      if (!item.isSelected()) {
        filter.add(item.getText());
      }
    }

    setFilter(filter, 2);

  }

}
