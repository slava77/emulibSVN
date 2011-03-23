package jsf.bean.gui.component.table.column;

import javax.faces.event.ActionEvent;
import jsf.bean.gui.component.table.BeanTable;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import jsf.bean.gui.metadata.PropertyMd;

public abstract class BeanTableColumnSortable extends BeanTableColumn {

    private static final Logger logger = SimpleLogger.getLogger(BeanTableColumnSortable.class);

    public BeanTableColumnSortable(BeanTable table, PropertyMd propertyMd) {
        super(table, propertyMd);
    }

    @Override
    public boolean isSortable() {
        return true;
    }

    public String getSortName() {
        return name;
    }

    public boolean isAscending() {
        return table.getProperties().getColumnSortAsc(getName());
    }

    public void setAscending(boolean ascending) {
        table.getProperties().setColumnSortAsc(getName(), ascending);
    }

    public boolean isSorting() {
        return table.getSortingColumns().getTarget().contains(this);
    }

    public void sortListener(ActionEvent ev) {
        if (isSorting() && table.getSortingColumns().getTarget().size() == 1) {
            setAscending(!isAscending());
        } else {

            // All to source
            table.getSortingColumns().getSource().addAll(table.getSortingColumns().getTarget());
            table.getSortingColumns().getTarget().clear();

            // This to target
            table.getSortingColumns().getSource().remove(this);
            table.getSortingColumns().getTarget().add(this);
        }
        table.sortingChangeListener(null);
        table.refreshListener(ev);
    }

}
