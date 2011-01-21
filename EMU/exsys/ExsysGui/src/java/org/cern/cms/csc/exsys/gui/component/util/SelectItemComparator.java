package org.cern.cms.csc.exsys.gui.component.util;

import java.util.Comparator;
import javax.faces.model.SelectItem;

public class SelectItemComparator implements Comparator<SelectItem> {

    @Override
    public int compare(SelectItem arg0, SelectItem arg1) {
        return arg0.getLabel().compareTo(arg1.getLabel());
    }
}
