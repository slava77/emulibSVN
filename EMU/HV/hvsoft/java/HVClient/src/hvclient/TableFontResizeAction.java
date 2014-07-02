package hvclient;

import java.awt.event.*;
import javax.swing.*;
import java.awt.Font;

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
public class TableFontResizeAction
    extends AbstractAction {

  public TableFontResizeAction(String name) {
    putValue(NAME, name);
  }

  /**
   * Invoked when an action occurs.
   *
   * @param e ActionEvent
   * @todo Implement this java.awt.event.ActionListener method
   */

  public void actionPerformed(ActionEvent e) {
    if (e.getSource() instanceof JTable) {
      JTable table = (JTable) e.getSource();
      Font font = table.getFont();
      int font_size = font.getSize();
      if (getValue(NAME).toString().compareTo("inc_font") == 0) {
        table.setFont(font.deriveFont(new Float(font_size + 1)));
      }
      else if (getValue(NAME).toString().compareTo("dec_font") == 0) {
        table.setFont(font.deriveFont(new Float(font_size - 1)));
      }
      table.setRowHeight(table.getFontMetrics(font).getHeight());
    }
  }

}
