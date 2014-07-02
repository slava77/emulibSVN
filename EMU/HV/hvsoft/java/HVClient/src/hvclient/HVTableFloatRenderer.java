package hvclient;

import javax.swing.table.*;
import java.text.NumberFormat;
import javax.swing.JTable;
import java.awt.Component;

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
public class HVTableFloatRenderer
    extends DefaultTableCellRenderer {

  private int precision=1;
  public HVTableFloatRenderer() {
    super();
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
  }

  public HVTableFloatRenderer(int prec) {
    super();
    precision = prec;
    setHorizontalAlignment(javax.swing.SwingConstants.RIGHT);
  }



  public void setValue(Object value) {
        if ((value != null) && (value instanceof Number)) {
          Number numberValue = (Number) value;
          NumberFormat formatter = NumberFormat.getInstance();
          formatter.setMaximumFractionDigits(precision);
          formatter.setMinimumFractionDigits(precision);
          value = formatter.format(numberValue.floatValue());

        }
        setToolTipText(value.toString());
        super.setValue(value);
      }

}
