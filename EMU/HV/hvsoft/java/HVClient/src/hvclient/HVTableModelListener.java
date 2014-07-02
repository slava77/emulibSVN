package hvclient;

import javax.swing.event.*;
import javax.swing.table.TableModel;
import dim.*;

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
public class HVTableModelListener
    implements TableModelListener {
  /**
   * This fine grain notification tells listeners the exact range of cells,
   * rows, or columns that changed.
   *
   * @param e TableModelEvent
   * @todo Implement this javax.swing.event.TableModelListener method
   */
  public void tableChanged(TableModelEvent e) {
    int row = e.getFirstRow();
        int column = e.getColumn();
        TableModel model = (TableModel)e.getSource();
        String columnName = model.getColumnName(column);
        Object data = model.getValueAt(row, column);

  }
}
