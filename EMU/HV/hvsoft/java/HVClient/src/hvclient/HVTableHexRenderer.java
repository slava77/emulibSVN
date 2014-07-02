package hvclient;

import javax.swing.table.*;
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
public class HVTableHexRenderer
    extends DefaultTableCellRenderer {
  public HVTableHexRenderer() {
  }

  public void setValue(Object value) {
    if ( (value != null) && (value instanceof Number)) {
      Number numberValue = (Number) value;
      NumberFormat formatter = NumberFormat.getInstance();
      value = "0x"+Integer.toHexString(numberValue.intValue());

    }

    setToolTipText(value.toString());
    super.setValue(value);
  }
}
