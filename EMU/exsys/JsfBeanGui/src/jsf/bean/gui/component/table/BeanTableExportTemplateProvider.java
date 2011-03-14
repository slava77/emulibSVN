/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table;

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

}
