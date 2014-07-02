package hvclient;

import java.awt.*;
import javax.swing.*;
import java.awt.BorderLayout;

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
public class dlgSetDimDNSNode
    extends JDialog {
  JPanel panel1 = new JPanel();
  BorderLayout borderLayout1 = new BorderLayout();
  public dlgSetDimDNSNode(Frame owner, String title, boolean modal) {
    super(owner, title, modal);
    try {
      setDefaultCloseOperation(DISPOSE_ON_CLOSE);
      jbInit();
      pack();
    }
    catch (Exception exception) {
      exception.printStackTrace();
    }
  }

  public dlgSetDimDNSNode() {
    this(new Frame(), "dlgSetDimDNSNode", false);
  }

  private void jbInit() throws Exception {
    panel1.setLayout(borderLayout1);
    getContentPane().add(panel1);
  }
}
