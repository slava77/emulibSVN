/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jsf.bean.gui.component.table.column;

import java.util.List;
import jsf.bean.gui.annotation.EmbeddedSortBy;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.metadata.EmbeddedPropertyMd;
import jsf.bean.gui.metadata.PropertyMd;

/**
 *
 * @author valdor
 */
public class BeanTableColumnEmbedded extends BeanTableColumnSortable {

    private final List<PropertyMd> properties;
    private String sortByProperty;

    public BeanTableColumnEmbedded(BeanTable table, EmbeddedPropertyMd propertyMd) {
        super(table, propertyMd);
        this.properties = propertyMd.getPropertyMds();
        for (PropertyMd pm: properties) {
            if (pm.getGetterMethod().isAnnotationPresent(EmbeddedSortBy.class)) {
                this.sortByProperty = pm.getName();
                break;
            }
        }
    }

    @Override
    public boolean isSortable() {
        return (sortByProperty != null);
    }

    @Override
    public String getSortName() {
        return getName().concat(".").concat(sortByProperty);
    }

}
