package hvclient;

import javax.swing.table.*;
import java.util.Vector;

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
public class HVDataTableModelEx
    extends AbstractTableModel {
  public HVDataTableModelEx() {
  }

  public HVDataTableModelEx(Object[] hdrs) {
    headers = hdrs;
    updateHeaders();

  }

  private Object[] headers = {};
  private boolean[] visibleMap = {};
  private boolean[] editableMap = {};
  private Object[][] data = {};

  private void updateHeaders() {
    visibleMap = new boolean[headers.length];
    editableMap = new boolean[headers.length];
    for (int i = 0; i < headers.length; i++) {
      visibleMap[i] = true;
      editableMap[i] = true;
    }
    this.fireTableStructureChanged();
  }

  public int getVisibleCount() {
    int cnt = 0;
    for (boolean flag : visibleMap) {
      if (flag)
        cnt++;
    }
    return cnt;
  }

  public boolean setVisibleColumn(int col, boolean flag) {
    if (col >= 0 && col < headers.length) {
      visibleMap[col] = flag;
      // updateHeaders();
      this.fireTableStructureChanged();
      return true;
    }
    return false;
  }

  public boolean setVisibleColumn(Object col, boolean flag) {
    return setVisibleColumn(getColumnPos(col), flag);
  }

  public boolean setEditableColumn(int col, boolean flag) {
    if (col >= 0 && col < headers.length) {
      editableMap[col] = flag;
      // updateHeaders();
      this.fireTableStructureChanged();
      return true;
    }
    return false;
  }

  public boolean setEditableColumn(Object col, boolean flag) {
    return setEditableColumn(getColumnPos(col), flag);
  }

  public int getVisibleColumnIndex(Object col) {
    int idx = -1;
    for (int i = 0; i < getHeaders().length; i++) {
      if (col.equals(getHeaders()[i]))
        return i;
    }
    return idx;
  }

  public int getColumnPos(Object col) {
    int idx = -1;
    for (int i = 0; i < headers.length; i++) {
      if (col.equals(headers[i]))
        return i;
    }
    return idx;
  }

  public Object[] getAllHeaders() {
    return headers;
  }

  public boolean isVisible(int col) {
    if (col >= 0 && col < headers.length) {
      return visibleMap[col];
    }
    return false;
  }

  public boolean isVisible(Object col) {
    return isVisible(getColumnPos(col));
  }

  public boolean isEditable(int col) {
    if (col >= 0 && col < headers.length) {
      return editableMap[col];
    }
    return false;
  }

  public boolean isCellEditable(int row, int col) {
    //Note that the data/cell address is constant,
    //no matter where the cell appears onscreen.

    if (col >= 0 && col < getHeaders().length) {
      int c = getColumnPos(getColumnName(col));
      return editableMap[c];
    }
    else {
      return false;
    }

  }

  public boolean isEditable(Object col) {
    return isEditable(getColumnPos(col));
  }

  public void setData(Object[][] newdata) {
    if (newdata.length > 0 && newdata[0].length >= headers.length) {
      /*
       Object[][] realdata = new Object[newdata.length][getHeaders().length];
            for (int i=0; i<newdata.length; i++) {
        int idx=0;
        for (int j=0; j<headers.length; j++) {
          if (visibleMap[j]) {
            realdata[i][idx] = newdata[i][j];
            idx++;
          }
        }
            }
            data = realdata;
       */
      data = newdata;
    }
    else {
      data = new Object[0][getHeaders().length];
    }
    this.fireTableDataChanged();
  }

  public Object[] getHeaders() {
    Vector real_headers = new Vector() {};
    for (int i = 0; i < headers.length; i++) {
      if (visibleMap[i]) {
        real_headers.add(headers[i]);
      }
    }
    return real_headers.toArray();
  }

  public String getColumnName(int col) {
    return getHeaders()[col].toString();
  }

  /**
   * Returns the number of columns in the model.
   *
   * @return the number of columns in the model
   * @todo Implement this javax.swing.table.TableModel method
   */
  public int getColumnCount() {
    return getHeaders().length;
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
    if ( (data.length > 0) && (rowIndex >= 0) && (rowIndex < data.length)
        && (columnIndex >= 0) && (columnIndex < getHeaders().length)) {
      int row = rowIndex;
      int col = getColumnPos(getColumnName(columnIndex));
      return data[row][col];
    }
    return null;
    // return new Integer((rowIndex+1)*(columnIndex+1));
  }

  public void setValueAt(Object value, int rowIndex, int columnIndex) {
    if ( (data.length > 0) && (rowIndex >= 0) && (rowIndex < data.length)
        && (columnIndex >= 0) && (columnIndex < getHeaders().length)) {
      int row = rowIndex;
      int col = getColumnPos(getColumnName(columnIndex));
      data[row][col] = value;
    }
  }

  public Class getColumnClass(int c) {
    int col = c;
    // int col = getColumnPos(getColumnName(c));
    return getValueAt(0, col).getClass();
  }

}
