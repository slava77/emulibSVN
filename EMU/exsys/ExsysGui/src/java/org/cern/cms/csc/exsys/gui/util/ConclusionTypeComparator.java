package org.cern.cms.csc.exsys.gui.util;

import java.util.Comparator;
import org.cern.cms.csc.exsys.re.model.ConclusionType;

public class ConclusionTypeComparator implements Comparator<ConclusionType> {

    @Override
    public int compare(ConclusionType arg0, ConclusionType arg1) {
        return arg0.getName().compareTo(arg1.getName());
    }
}
