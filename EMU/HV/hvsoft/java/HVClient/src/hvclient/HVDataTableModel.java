package hvclient;

import javax.swing.table.*;

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
public class HVDataTableModel
    extends AbstractTableModel {

  private Object[] columnNames = {};
  private boolean[] editableColMap = new boolean[columnNames.length];
  private boolean[] visibleColMap = new boolean[columnNames.length];
  private boolean mapState = true;

  private void updateColumns(Object[] colnames) {
    columnNames = colnames;
    editableColMap = new boolean[columnNames.length];
    visibleColMap = new boolean[columnNames.length];
    for (int i = 0; i < columnNames.length; i++) {
      editableColMap[i] = mapState;
      visibleColMap[i] = mapState;
    }

  }

  public boolean setEditableColumn(int col, boolean state) {
    if (col >= 0 && col < editableColMap.length) {
      editableColMap[col] = state;
      return true;

    }
    else {
      return false;
    }
  }

  public boolean setVisibleColumn(int col, boolean state) {
    if (col >= 0 && col < visibleColMap.length) {
      visibleColMap[col] = state;
      return true;
    }
    else {
      return false;
    }
  }

  public boolean setEditableColumn(Object colobj, boolean state) {
    return setEditableColumn(getColumnPos(colobj), state);
  }

  public boolean setVisibleColumn(Object colobj, boolean state) {
    return setVisibleColumn(getColumnPos(colobj), state);
  }

  public HVDataTableModel() {
}

  public HVDataTableModel(Object[] colnames) {
    updateColumns(colnames);
  }

  public HVDataTableModel(Object[] colnames, boolean state) {
    mapState = state;
    updateColumns(colnames);
  }



  public void setColumnIdentifiers(Object[] colnames) {
    updateColumns(colnames);
  }

  public void setColumnIdentifiers(Object[] colnames, boolean state) {
    mapState = state;
    updateColumns(colnames);
  }


  private Object[][] data = {};

  public synchronized void setData(Object[][] newdata) {
    data = newdata;
    fireTableDataChanged();
  }

  /**
   * Returns the number of columns in the model.
   *
   * @return the number of columns in the model
   * @todo Implement this javax.swing.table.TableModel method
   */
  public int getColumnCount() {
    /*
         int cols=0;
         for (int i=0; i<visibleColMap.length; i++)
      if (visibleColMap[i]) cols++;
         return cols;
     */
    return columnNames.length;
  }

  public int getColumnPos(Object col) {
    int pos = 0;
    for (pos = 0; pos < columnNames.length; pos++) {
      if (getColumnName(pos) == col.toString()) {
        return pos;
      }
    }
    pos = -1;
    return pos;
  }

  public String getColumnName(int col) {
    return columnNames[col].toString();
  }

  public boolean isCellEditable(int row, int col) {
    //Note that the data/cell address is constant,
    //no matter where the cell appears onscreen.

    if (col >= 0 && col < editableColMap.length) {
      return editableColMap[col];
    }
    else {
      return false;
    }

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
    //if (rowIndex < data.length && columnIndex < columnNames.length &&
    //    visibleColMap[columnIndex]) {
      return data[rowIndex][columnIndex];
    //}
    //else {
    //  return null;
    //}
  }

  public Class getColumnClass(int c) {
    return getValueAt(0, c).getClass();
  }

  public void setValueAt(Object value, int row, int column) {
    data[row][column] = value;
  }

}
