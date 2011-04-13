/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

import java.util.Collections;
import java.util.Date;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;
import jsf.bean.gui.component.table.BeanTable;

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
     * Get a list of public templates for a table
     * @return
     */
    public List<BeanTableExportTemplate> getPublicTemplates() {
        return Collections.EMPTY_LIST;
    }

    /**
     * Get a list of user templates for a table
     * @return
     */
    public List<BeanTableExportTemplate> getUserTemplates() {
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

    /**
     * Override this method to get a table attribute rowClass which provides templates
     * @return
     */
    public BeanTable getTable() {
        return null;
    }

}
