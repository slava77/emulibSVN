/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

import java.util.Collections;
import java.util.List;
import jsf.bean.gui.EntityBeanBase;

/**
 *
 * @author valdo
 */
public class BeanTableExportTemplateProvider {

    public List<BeanTableExportTemplate> getTemplates(Class<? extends EntityBeanBase> rowClass) {
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
    public String getCustomTemplatesAction() {
        return null;
    }

}
