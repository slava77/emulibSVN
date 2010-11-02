package org.cern.cms.csc.dw.gui.table;

import java.io.Serializable;
import java.util.StringTokenizer;
import org.cern.cms.csc.dw.dao.table.BeanTableSortColumnIf;
import org.cern.cms.csc.dw.metadata.PropertyMd;
import org.cern.cms.csc.exsys.exception.InvalidEntityBeanPropertyException;

public class BeanTableSortColumn implements BeanTableSortColumnIf, Serializable {

    private PropertyMd propertyMd;
    private SORT_DIR dir = SORT_DIR.ASCENDING;

    public BeanTableSortColumn(PropertyMd propertyMd) {
        this.propertyMd = propertyMd;
    }

    public BeanTableSortColumn(BeanTable table, String sortByString) throws InvalidEntityBeanPropertyException {
        StringTokenizer tok = new StringTokenizer(sortByString, " ");
        String sname = tok.nextToken();
        String sdir = tok.nextToken();

        for (PropertyMd pmd: table.getPropertyMds()) {
            if (pmd.getName().equals(sname)) {
                this.propertyMd = pmd;
                break;
            }
        }

        if (this.propertyMd == null) {
            throw new IllegalArgumentException("Sort by property not found in " + sortByString);
        }

        if (tok.hasMoreTokens()) {
            if (sdir.equals(SORT_DIR.DESCENDING.getValue())) {
                dir = SORT_DIR.DESCENDING;
            }
        }
    }

    public PropertyMd getPropertyMd() {
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

    public String getPropertyName() {
        return getPropertyMd().getName();
    }

}

