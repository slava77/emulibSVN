package org.cern.cms.csc.dw.gui.table.column.filter;

import java.util.ArrayList;
import java.util.List;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterIf;
import org.cern.cms.csc.dw.dao.table.BeanTableFilterItemIf;

public class TableFilter implements BeanTableFilterIf {

    private List<BeanTableFilterItemIf> items = new ArrayList<BeanTableFilterItemIf>();

    public List<BeanTableFilterItemIf> getItems() {
        return items;
    }
    private String string = null;

    public String getString() {
        return this.string;
    }

    public void setString(String string) {
        this.string = string;
    }
    private boolean defaultFilter = false;

    public boolean isDefault() {
        return this.defaultFilter;
    }

    public void setDefault(boolean defaultFilter) {
        this.defaultFilter = defaultFilter;
    }

    public boolean isEmpty() {
        return (string == null || "".equals(string));
    }

}