package hvclient;

import java.awt.*;
import javax.swing.*;
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
public class HVTableRenderer
    extends DefaultTableCellRenderer {


  private String status;
  private Color table_color;
  public HVTableRenderer() {
    super();

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
    if (isSelected) {
     setForeground(table.getSelectionForeground());
     super.setBackground(table.getSelectionBackground());
   } else {
     setForeground(table.getForeground());
     setBackground(table.getBackground());
   }
   table_color = table.getBackground();
   if (value instanceof String) {
     status = ( (String) value).toString();
   }
   if (value instanceof Integer) {
//     status = hvstate_str[(Integer)value];
     status = HVData.HV_STATE.getValue((Integer) value).desc();
   }

   setText(status);
   setToolTipText(status);
   // setSelectedIndex(state=="OFF" ? 1 : 0);
   Color bColor;
   bColor = table_color;
         // bColor = Color.GREEN;
         // System.out.println(status);
         if (status.compareTo("OFF")==0)
           bColor = table_color;
         if (status.compareTo("ON")==0)
           bColor = Color.GREEN;
         if ((status.compareTo("ILOCK")==0)
             || (status.compareTo("INTERLOCK")==0))
           bColor = Color.ORANGE;
         if ((status.compareTo("INT TRIP")==0)
             ||  (status.compareTo("CUR TRIP")==0)
             ||  (status.compareTo("OV TRIP")==0))
           bColor = Color.RED;
         if ((status.compareTo("RAMP UP")==0)
             || (status.compareTo("RAMP DOWN")==0))
           bColor = Color.YELLOW;
         if (status.compareTo("DEAD")==0)
           bColor = Color.GRAY;
         cell.setBackground(bColor);

   return this;
  }
  // This is a hack to paint the background.  Normally a JLabel can
  // paint its own background, but due to an apparent bug or
  // limitation in the TreeCellRenderer, the paint method is
  // required to handle this.
  /*
  public void paint( Graphics g )
  {
          Color		bColor;
          bColor = table_color;
          // bColor = Color.GREEN;
          // System.out.println(status);
          if (status.compareTo("OFF")==0)
            bColor = table_color;
          if (status.compareTo("ON")==0)
            bColor = Color.GREEN;
          if (status.compareTo("ILOCK")==0)
            bColor = Color.ORANGE;
          if ((status.compareTo("INT TRIP")==0)
              ||  (status.compareTo("CH INT TRIP")==0))
            bColor = Color.RED;
          if ((status.compareTo("RAMP UP")==0)
              || (status.compareTo("RAMP DOWN")==0))
            bColor = Color.YELLOW;
          if (status.compareTo("DEAD")==0)
            bColor = Color.GRAY;

          g.setColor( bColor );

          // Draw a rectangle in the background of the cell
          g.fillRect( 0, 0, getWidth() - 1, getHeight() - 1 );

          super.paint( g );
  }
  */
}
