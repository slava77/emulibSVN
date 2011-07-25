/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

/**
 *
 * @author valdo
 */
public class BeanTableApiNotConfigured extends Exception {

    public BeanTableApiNotConfigured(String id) {
        super("Bean table API with ID = [".concat(id).concat("] not found?!"));
    }
    
}
