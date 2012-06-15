/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

import java.util.Map;

/**
 *
 * @author valdo
 */
public interface BeanTableExportTemplatePrimary extends BeanTableExportTemplate {

    public enum TemplatePosition {
        HEADER,
        ITEM,
        FOOTER
    };
    
    Map<TemplatePosition, String> getTemplates();

}
