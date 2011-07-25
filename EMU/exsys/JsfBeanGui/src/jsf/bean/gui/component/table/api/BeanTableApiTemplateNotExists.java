/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.api;

/**
 *
 * @author valdo
 */
public class BeanTableApiTemplateNotExists extends Exception {

    public BeanTableApiTemplateNotExists(String id) {
        super("Template id = [".concat(id).concat("] not exists?!"));
    }
    
}
