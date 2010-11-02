package org.cern.cms.csc.dw.gui.table;

import java.util.Comparator;
import javax.faces.model.SelectItem;

public class SelectItemComparator implements Comparator<SelectItem> {

    public int compare(SelectItem arg0, SelectItem arg1) {
        return arg0.getLabel().compareTo(arg1.getLabel());
    }
}
