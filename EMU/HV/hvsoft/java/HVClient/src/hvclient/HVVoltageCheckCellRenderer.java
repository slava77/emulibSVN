package hvclient;

import java.lang.Math;
import java.awt.Component;
import java.awt.Color;
import javax.swing.JTable;
import javax.swing.table.DefaultTableCellRenderer;

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
public class HVVoltageCheckCellRenderer
    extends DefaultTableCellRenderer {

  private int refCol = -1;
  private int stateCol = -1;

  private int threshold = 25;
  // private Object thresh = null;


  public HVVoltageCheckCellRenderer() {
  }

  public HVVoltageCheckCellRenderer(int col) {
    refCol = col;
  }

  public HVVoltageCheckCellRenderer(int col, int thr) {
    refCol = col;
    threshold = thr;
  }

  public HVVoltageCheckCellRenderer(int col, int thr, int statecol) {
    refCol = col;
    threshold = thr;
    stateCol = statecol;
  }

  public void setRefColum(int col) {
    refCol = col;
  }

  public void setThreshold(int thr) {
    threshold = thr;
  }

  /*
    public void setThreshold(Object thr) {
      thresh = thr;
    }
   */

  public Component getTableCellRendererComponent
      (JTable table, Object value, boolean isSelected,
       boolean hasFocus, int row, int column) {
    Component cell = super.getTableCellRendererComponent
        (table, value, isSelected, hasFocus, row, column);

    if (isSelected) {
      cell.setBackground(table.getSelectionBackground());
      return cell;
    }

    boolean fEnableCheck = true;

    if (stateCol >= 0) {
      Object statevalue = table.getModel().getValueAt(row, stateCol);
      if (statevalue instanceof Integer) {
        fEnableCheck = ( (Integer) statevalue == 1) ? true : false;
      }

      if (statevalue instanceof String) {
        fEnableCheck = (statevalue.toString().startsWith("ON"));
      }

    }

    Integer refamount = null;

    if (refCol >= 0) {
      Object refvalue = table.getModel().getValueAt(row, refCol);

      if (refvalue instanceof Integer) {
        refamount = (Integer) refvalue;
      }

      if (refvalue instanceof String) {
        refamount = Integer.parseInt(refvalue.toString());
      }
    }
    else {
      refamount = new Integer(0);
    }

    if ( (value instanceof Integer) && (refamount != null)) {
      Integer amount = (Integer) value;
      if (Math.abs(refamount.intValue() - amount.intValue()) > threshold &&
          fEnableCheck) {
        cell.setBackground(Color.orange);
        // You can also customize the Font and Foreground this way
        // cell.setForeground();
        // cell.setFont();
      }
      else {
        cell.setBackground(Color.white);
      }
    }

    // int col = 4;
    // col = table.getModel()




    if ( (value instanceof String) && (refamount != null)) {
      Integer amount = Integer.parseInt(value.toString());

      if (Math.abs(refamount - amount) > threshold && fEnableCheck) {
        cell.setBackground(Color.orange);
        // You can also customize the Font and Foreground this way
        // cell.setForeground();
        // cell.setFont();
      }
      else {
        cell.setBackground(Color.white);
      }
    }

    return cell;

  }

  public void setValue(Object value) {
       // if (value != null)
         setToolTipText(value.toString());
       super.setValue(value);
     }


}
