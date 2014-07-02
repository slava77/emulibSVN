package hvclient;

import java.util.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.*;
import javax.swing.event.*;
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
public class HVTableEditor extends JComboBox
    implements TableCellEditor {

  protected EventListenerList listenerList = new EventListenerList();
  protected ChangeEvent changeEvent = new ChangeEvent(this);

  public HVTableEditor() {
    super();
   addItem("OFF");
   addItem("ON");
   addActionListener(new ActionListener() {
     public void actionPerformed(ActionEvent event) {
       fireEditingStopped();
     }
   });
  }

  /**
   * Adds a listener to the list that's notified when the editor stops, or
   * cancels editing.
   *
   * @param l the CellEditorListener
   * @todo Implement this javax.swing.CellEditor method
   */
  public void addCellEditorListener(CellEditorListener l) {
    listenerList.add(CellEditorListener.class, l);
  }

  protected void fireEditingStopped() {
      CellEditorListener listener;
      Object[] listeners = listenerList.getListenerList();
      for (int i = 0; i < listeners.length; i++) {
        if (listeners[i] == CellEditorListener.class) {
          listener = (CellEditorListener) listeners[i + 1];
          listener.editingStopped(changeEvent);
        }
      }
    }

    protected void fireEditingCanceled() {
      CellEditorListener listener;
      Object[] listeners = listenerList.getListenerList();
      for (int i = 0; i < listeners.length; i++) {
        if (listeners[i] == CellEditorListener.class) {
          listener = (CellEditorListener) listeners[i + 1];
          listener.editingCanceled(changeEvent);
        }
      }
    }

  /**
   * Tells the editor to cancel editing and not accept any partially edited
   * value.
   *
   * @todo Implement this javax.swing.CellEditor method
   */
  public void cancelCellEditing() {
    fireEditingCanceled();

  }

  /**
   * Returns the value contained in the editor.
   *
   * @return the value contained in the editor
   * @todo Implement this javax.swing.CellEditor method
   */
  public Object getCellEditorValue() {
    // System.out.println(new String(getSelectedIndex() == 0 ? "OFF" : "ON"));
    return new String(getSelectedIndex() == 0 ? "OFF" : "ON");
  }

  /**
   * Sets an initial <code>value</code> for the editor.
   *
   * @param table the <code>JTable</code> that is asking the editor to edit;
   *   can be <code>null</code>
   * @param value the value of the cell to be edited; it is up to the specific
   *   editor to interpret and draw the value. For example, if value is the
   *   string "true", it could be rendered as a string or it could be rendered
   *   as a check box that is checked. <code>null</code> is a valid value
   * @param isSelected true if the cell is to be rendered with highlighting
   * @param row the row of the cell being edited
   * @param column the column of the cell being edited
   * @return the component for editing
   * @todo Implement this javax.swing.table.TableCellEditor method
   */
  public Component getTableCellEditorComponent(JTable table, Object value,
                                               boolean isSelected, int row,
                                               int column) {
    if (isSelected) {
     setForeground(table.getSelectionForeground());
     super.setBackground(table.getSelectionBackground());
  } else {
    setForeground(table.getForeground());
    setBackground(table.getBackground());
  }

    //String state = ((String) value).toString();
    // setSelectedIndex(state=="OFF" ? 1 : 0);
    if (value.getClass() == Integer.class)
      setSelectedIndex((Integer)value);
    //if (value.getClass() == String.class)
      // setSelectedIndex(value);
    return this;
  }

  /**
   * Asks the editor if it can start editing using <code>anEvent</code>.
   *
   * @param anEvent the event the editor should use to consider whether to
   *   begin editing or not
   * @return true if editing can be started
   * @todo Implement this javax.swing.CellEditor method
   */
  public boolean isCellEditable(EventObject anEvent) {
    return true;
  }

  /**
   *
   * @param l the CellEditorListener
   * @todo Implement this javax.swing.CellEditor method
   */
  public void removeCellEditorListener(CellEditorListener l) {
    listenerList.remove(CellEditorListener.class, l);
  }

  /**
   * Returns true if the editing cell should be selected, false otherwise.
   *
   * @param anEvent the event the editor should use to start editing
   * @return true if the editor would like the editing cell to be selected;
   *   otherwise returns false
   * @todo Implement this javax.swing.CellEditor method
   */
  public boolean shouldSelectCell(EventObject anEvent) {
    return true;
  }

  /**
   * Tells the editor to stop editing and accept any partially edited value as
   * the value of the editor.
   *
   * @return true if editing was stopped; false otherwise
   * @todo Implement this javax.swing.CellEditor method
   */
  public boolean stopCellEditing() {
    fireEditingStopped();
    return true;
  }
}

