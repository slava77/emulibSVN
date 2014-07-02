package hvclient;

import javax.swing.table.*;

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
public class HVPrimaryPSDataTableModel
    extends DefaultTableModel {

  private String[] columnNames = {}; /* {
        "HostID", "Port", "Addr", "Vmon", "Imon",
        "Vset", "Vcur", "Imax",
        "RampUp", "RampDn", "Control",
        "State", "Status"
    };
  */
 private Object[][] data = {};

  /**
   * Returns the number of columns in the model.
   *
   * @return the number of columns in the model
   * @todo Implement this javax.swing.table.TableModel method
   */
  public int getColumnCount() {
    return columnNames.length;
  }

  /**
   * Returns the number of rows in the model.
   *
   * @return the number of rows in the model
   * @todo Implement this javax.swing.table.TableModel method
   */
  public int getRowCount() {
    return data.length;
  }

  /**
   * Returns the value for the cell at <code>columnIndex</code> and
   * <code>rowIndex</code>.
   *
   * @param rowIndex the row whose value is to be queried
   * @param columnIndex the column whose value is to be queried
   * @return the value Object at the specified cell
   * @todo Implement this javax.swing.table.TableModel method
   */
  public Object getValueAt(int rowIndex, int columnIndex) {
    return data[rowIndex][columnIndex];
  }

  public boolean isCellEditable(int row, int col) {
    //Note that the data/cell address is constant,
    //no matter where the cell appears onscreen.
    /*
            if (getColumnName(col) == columnNames[0] ||
        getColumnName(col) == columnNames[1] ||
        getColumnName(col) == columnNames[2] ||
        getColumnName(col) == columnNames[3] ||
        getColumnName(col) == columnNames[4] ||
        getColumnName(col) == columnNames[6] ||
        getColumnName(col) == columnNames[10] ||
        getColumnName(col) == columnNames[12] ) {
        return false;
            } else {
        return true;
            }
     */
    return true;
  }

  public String getColumnName(int col) {
    return columnNames[col];
  }



  public synchronized void setData(Object[][] newdata) {
    data = newdata;
    fireTableDataChanged();
  }

  public synchronized void setColumnIdentifiers(Object[] columns) {
    columnNames = (String[])columns;
    fireTableDataChanged();
  }

  public Class getColumnClass(int c) {
    return getValueAt(0, c).getClass();
  }

  public void setValueAt(Object value, int row, int column) {
    data[row][column] = value;
  }

}
