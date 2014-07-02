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
public class HVTableObjectRenderer
    extends DefaultTableCellRenderer {
  public HVTableObjectRenderer() {
  }

  public void setValue(Object value) {
        // if (value != null)
          setToolTipText(value.toString());
        super.setValue(value);
      }

}
