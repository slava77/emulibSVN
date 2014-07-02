package hvclient;

import javax.swing.*;
import javax.swing.JOptionPane;
import java.util.*;

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
public class HVParameter {
  protected String name;
  protected String label;
  protected Class input;
  protected Object default_value;
  protected Object value;
  private JComponent comp;
  protected Vector<Object> range;
  protected boolean fFirstTime;

  public HVParameter(String parname) {
    name = parname;
    label = parname;
    input = JTextField.class;
    default_value = "";
    value = default_value;
    range = new Vector<Object> ();
    createComponent();
  }

  public HVParameter(String parname, String parlabel) {
    name = parname;
    label = parlabel;
    input = JTextField.class;
    default_value = "";
    value = default_value;
    range = new Vector<Object> ();
    createComponent();

  }

  public HVParameter(String parname, String parlabel, Class parclass) {
    name = parname;
    label = parlabel;
    input = parclass;
    default_value = "";
    value = default_value;
    range = new Vector<Object> ();
    createComponent();

  }

  public HVParameter(String parname, String parlabel, Class parclass,
                     Object pardef_value) {
    name = parname;
    label = parlabel;
    input = parclass;
    default_value = pardef_value;
    value = default_value;
    range = new Vector<Object> ();
    createComponent();

  }

  public HVParameter(String parname, String parlabel, Class parclass,
                     Object pardef_value, Vector<Object> parrange) {
    name = parname;
    label = parlabel;
    input = parclass;
    default_value = pardef_value;
    value = default_value;
    range = parrange;
    createComponent();
  }

  private void createComponent() {
    if (input == JTextField.class) {
      comp = new JTextField(default_value.toString());
    }
    else if (input == JCheckBox.class) {
      comp = new JCheckBox("",
                           new Boolean(default_value.toString()));

    }
    else if (input == JComboBox.class) {
      comp = new JComboBox(range);
      ( (JComboBox) comp).setSelectedItem(default_value);
    }
    fFirstTime = true;
  }

  private Object checkValue(Object val) {
    Object res = val;

    try {
      if (default_value instanceof Integer && range.size() == 2) {
        Integer v = Integer.parseInt(val.toString());
        Integer min = Integer.parseInt(range.elementAt(0).toString());
        Integer max = Integer.parseInt(range.elementAt(1).toString());
        if ( (v < min) || (v > max)) {
          // System.out.println("Out of range");
          res = default_value;
        }
      }
      if (default_value instanceof Double && range.size() == 2) {
        Double v = Double.parseDouble(val.toString());
        Double min = Double.parseDouble(range.elementAt(0).toString());
        Double max = Double.parseDouble(range.elementAt(1).toString());
        if ( (v < min) || (v > max)) {
          // System.out.println("Out of range");
          res = default_value;
        }
      }

    }
    catch (NumberFormatException nfe) {
      System.out.println("Exception " + nfe.toString());
      // return default_value;
      res = default_value;
    }

    return res;
  }

  public Object getValue() {
    if (input == JTextField.class) {
      value = ( (JTextField) comp).getText();
    }
    else if (input == JCheckBox.class) {
      value = ( (JCheckBox) comp).isSelected();
    }
    else if (input == JComboBox.class) {
      value = ( (JComboBox) comp).getSelectedItem();
    }
    return value;
  }

  public void updateValue() {
    default_value = getValue();
    if (input == JTextField.class) {
      ( (JTextField) comp).setText(default_value.toString());
    }
    else if (input == JCheckBox.class) {
      if (default_value.toString().equals("true")) {
         ( (JCheckBox) comp).setSelected(true);
      }
      else {
         ( (JCheckBox) comp).setSelected(false);
      }
    }
    else if (input == JComboBox.class) {
      ( (JComboBox) comp).setSelectedItem(default_value);
    }

  }

  public String getName() {
    return name;
  }

  public boolean isModified() {
    /*
    boolean res = false;
    if (fFirstTime) {
      res = true;
      fFirstTime = false;
    }
    else {
      res = (default_value.toString()).compareTo( (getValue().toString())) != 0;
    }
    return res;
    */
    return (default_value.toString()).compareTo( checkValue(getValue()).toString()) != 0;
  }

  public JComponent getComponent() {
    return comp;
  }

  public void setInputClass(Class in) {
    input = in;
    createComponent();
  }

  public void setLabel(String lbl) {
    label = lbl;
  }

  public void setDefaultValue(Object val) {
    default_value = val;
    createComponent();
  }

  public void setValue(Object val) {
    value = val;
  }

  public void setRange(Vector<Object> rng) {
    range = rng;
    createComponent();

  }
}
