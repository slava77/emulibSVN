/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jsf.bean.gui.component.table.column;

import java.util.LinkedList;
import java.util.List;
import javax.faces.event.ActionEvent;
import javax.faces.model.SelectItem;
import jsf.bean.gui.annotation.EmbeddedSortBy;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.metadata.EmbeddedPropertyMd;
import jsf.bean.gui.metadata.PropertyMd;

/**
 *
 * @author valdor
 */
public class BeanTableColumnEmbedded extends BeanTableColumnSortable {

    private List<BeanTableColumnBase> properties = new LinkedList<BeanTableColumnBase>();
    private BeanTableColumnBase selected;
    private String sortByProperty;

    public BeanTableColumnEmbedded(BeanTable table, EmbeddedPropertyMd propertyMd) {
        super(table, propertyMd);
        for (PropertyMd pm : propertyMd.getProperties()) {
            BeanTableColumnBase col = BeanTableColumnFactory.getBeanTableColumnBase(pm);
            if (col != null) {

                col.name = name + "." + col.name;
                properties.add(col);

                if (selected == null) {
                    selected = col;
                }

                if (this.sortByProperty == null) {
                    if (pm.getGetterMethod().isAnnotationPresent(EmbeddedSortBy.class)) {
                        this.sortByProperty = pm.getName();
                    }
                }

            }
        }
    }

    @Override
    public void clearFilter() {
        for (BeanTableColumnBase c: properties) {
            c.clearFilter();
        }
    }



    @Override
    public boolean isFilterSet() {
        for (BeanTableColumnBase c: properties) {
            if (c.isFilterSet()) {
                return true;
            }
        }
        return false;
    }



    public boolean isNotEmpty() {
        return ! properties.isEmpty();
    }

    public List<BeanTableColumnBase> getProperties() {
        return properties;
    }

    public List<SelectItem> getPropertyItems() {
        List<SelectItem> ret = new LinkedList<SelectItem>();
        for (BeanTableColumnBase c: properties) {
            ret.add(new SelectItem(c.getName(), c.getTitle()));
        }
        return ret;
    }

    public BeanTableColumnBase getSelected() {
        return selected;
    }

    public String getSelectedName() {
        return selected.getName();
    }

    public void setSelectedName(String name) {
        for (BeanTableColumnBase c: properties) {
            if (c.getName().equals(name)) {
                this.selected = c;
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

    @Override
    public boolean isEmbedType() {
        return true;
    }

}
