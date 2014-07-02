package hvclient;

import javax.swing.table.*;
import java.awt.Component;
import java.awt.Color;
import javax.swing.JTable;


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
public class LogLevelRenderer
    extends DefaultTableCellRenderer {

  private int validation_column = 0;

  public LogLevelRenderer() {
    super();
  }

  public LogLevelRenderer(int val_col) {
    super();
    validation_column = val_col;
  }



  /**
   * Returns the component used for drawing the cell.
   *
   * @param table the <code>JTable</code> that is asking the renderer to draw;
   *   can be <code>null</code>
   * @param value the value of the cell to be rendered. It is up to the
   *   specific renderer to interpret and draw the value. For example, if
   *   <code>value</code> is the string "true", it could be rendered as a
   *   string or it could be rendered as a check box that is checked.
   *   <code>null</code> is a valid value
   * @param isSelected true if the cell is to be rendered with the selection
   *   highlighted; otherwise false
   * @param hasFocus if true, render cell appropriately. For example, put a
   *   special border on the cell, if the cell can be edited, render in the
   *   color used to indicate editing
   * @param row the row index of the cell being drawn. When drawing the
   *   header, the value of <code>row</code> is -1
   * @param column the column index of the cell being drawn
   * @return Component
   * @todo Implement this javax.swing.table.TableCellRenderer method
   */
  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int column) {

    Component cell = super.getTableCellRendererComponent
        (table, value, isSelected, hasFocus, row, column);

    String status =  table.getModel().getValueAt(row, validation_column ).toString();

    if (isSelected) {
      setForeground(table.getSelectionForeground());
      super.setBackground(table.getSelectionBackground());
    }
    else {
      setForeground(table.getForeground());
      setBackground(table.getBackground());
    }

    setToolTipText(value.toString());
    Color bColor;
    bColor = table.getBackground();


    if (status.equalsIgnoreCase("DEBUG")) {
      bColor = table.getBackground();
    }
    if (status.equalsIgnoreCase("INFO")) {
      bColor = Color.GREEN;
    }
    if (status.equalsIgnoreCase("WARN")) {
      bColor = Color.ORANGE;
    }
    if ( (status.equalsIgnoreCase("ERROR"))
        || (status.compareTo("FATAL") == 0)) {
      bColor = Color.RED;
    }
    //cell.setForeground(bColor);
    cell.setBackground(bColor);

    return this;
  }

}
