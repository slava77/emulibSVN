/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

import java.util.Collections;
import java.util.Date;
import java.util.List;

/**
 *
 * @author valdo
 */
public class BeanTableExportTemplateProvider {

    private static final Date modified = new Date();

    /**
     * Override this to implement modifications in templates.
     * @return
     */
    public Date getModified() {
        return modified;
    }

    /**
     * Get a list of templates for a table
     * @return
     */
    public List<BeanTableExportTemplate> getTemplates() {
        return Collections.EMPTY_LIST;
    }

    /**
     * Override this method to enable custom template editing link
     * @return
     */
    public boolean isEditCustomTemplates() {
        return false;
    }

    /**
     * Override this method to redirect user to custom templates editing page
     * Hint: look for attribute rowClass for table RowClass
     * @return
     */
    public String customTemplatesAction() {
        return null;
    }

}
