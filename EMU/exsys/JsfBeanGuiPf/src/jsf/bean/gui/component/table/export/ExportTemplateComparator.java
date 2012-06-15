/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.export;

import java.util.Comparator;
import jsf.bean.gui.component.table.export.BeanTableExportTemplate;

/**
 *
 * @author valdor
 */
public class ExportTemplateComparator implements Comparator<BeanTableExportTemplate> {

    public int compare(BeanTableExportTemplate t1, BeanTableExportTemplate t2) {
        return t1.getTitle().compareTo(t2.getTitle());
    }

}
