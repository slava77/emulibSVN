package org.cern.cms.csc.dw.gui.table;

import org.cern.cms.csc.dw.dao.table.BeanTableColumnIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;

public abstract class BeanTableColumnBase implements BeanTableColumnIf {

    protected final BeanTableBase table;
    protected final PropertyMd propertyMd;
    private String name;
    private SORT_DIR dir = SORT_DIR.ASCENDING;
    private Integer width = null;

    public BeanTableColumnBase(BeanTableBase table, PropertyMd propertyMd) {
        this.table = table;
        this.propertyMd = propertyMd;
        this.name = propertyMd.getName();
        
        // Exception for the enumerations
        if (propertyMd.getType().isEnum()) {
            this.name = this.name.concat("Item");
        }
    }

    protected PropertyMd getPropertyMd() {
        return propertyMd;
    }

    public boolean isAscending() {
        return dir.equals(SORT_DIR.ASCENDING);
    }

    public void setAscending(boolean ascending) {
        dir = (ascending ? SORT_DIR.ASCENDING : SORT_DIR.DESCENDING);
    }

    public SORT_DIR getDir() {
       return dir;
    }

    public String getName() {
        return name;
    }

    public String getTitle() {
        return getPropertyMd().getTitle();
    }

    public Integer getWidth() {
        return width;
    }

    public void setWidth(Integer width) {
        this.width = width;
    }

}
