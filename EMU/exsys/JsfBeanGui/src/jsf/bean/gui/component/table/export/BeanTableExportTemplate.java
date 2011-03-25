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
public interface BeanTableExportTemplate {

    public enum TemplateTypeKey {
        HEADER,
        ITEM,
        FOOTER
    };

    String getMimeType();
    String getTitle();
    String getExt();

    Map<TemplateTypeKey, String> getTemplate();

}
