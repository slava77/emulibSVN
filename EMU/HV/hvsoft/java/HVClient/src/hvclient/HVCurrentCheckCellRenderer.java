package hvclient;

import javax.swing.table.*;
import java.awt.Component;
import java.awt.Color;
import javax.swing.JTable;
import java.text.NumberFormat;

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
public class HVCurrentCheckCellRenderer
    extends DefaultTableCellRenderer {

  private int refCol = -1;
  private int stateCol = -1;

  private double threshold = 1.0;
  // private Object thresh = null;


  public HVCurrentCheckCellRenderer() {
    super();
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
  }

  public HVCurrentCheckCellRenderer(int col) {
    super();
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
    refCol = col;
  }

  public HVCurrentCheckCellRenderer(int col, double thr) {
    super();
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
    refCol = col;
    threshold = thr;
  }

  public HVCurrentCheckCellRenderer(int col, double thr, int statecol) {
    super();
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
    refCol = col;
    threshold = thr;
    stateCol = statecol;
  }

  public void setRefColum(int col) {
    refCol = col;
  }

  public void setThreshold(double thr) {
    threshold = thr;
  }

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

    Double refamount = null;

    if (refCol >= 0) {
      Object refvalue = table.getModel().getValueAt(row, refCol);

      if (refvalue instanceof Double) {
        refamount = (Double) refvalue;
      }

      if (refvalue instanceof String) {

        refamount = Double.parseDouble(refvalue.toString());
      }
    }
    else {
      refamount = 0.0;
    }

    if ( (value instanceof Double) && (refamount != null)) {
      Double amount = (Double) value;
      if (Math.abs(refamount - amount) > threshold &&
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

    if ( (value instanceof Float) && (refamount != null)) {
     Float amount = (Float) value;
     if (Math.abs(refamount - amount) > threshold &&
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
      Float amount = Float.parseFloat(value.toString());

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
       if ((value != null) && (value instanceof Number)) {
          Number numberValue = (Number) value;
          NumberFormat formatter = NumberFormat.getInstance();
          formatter.setMaximumFractionDigits(1);
          formatter.setMinimumFractionDigits(1);
          value = formatter.format(numberValue.floatValue());

        }
         setToolTipText(value.toString());
       super.setValue(value);
     }


}
