/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

/**
 *
 * @author valdo
 */
public interface BeanTableExportTemplateSecondary extends BeanTableExportTemplate {

    public enum TemplateType {
        
        XSLT,
        FREEMARKER;
        
        public String getName() {
            return this.name();
        }
        
    };
    
    BeanTableExportTemplate getPreviousTemplate();
    TemplateType getTemplateType();
    String getTemplate();

}
