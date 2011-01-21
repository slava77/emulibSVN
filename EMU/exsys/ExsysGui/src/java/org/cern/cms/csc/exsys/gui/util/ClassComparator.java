package org.cern.cms.csc.exsys.gui.util;

import java.util.Comparator;

public class ClassComparator implements Comparator<Class> {

    @Override
    public int compare(Class arg0, Class arg1) {
        return arg0.getSimpleName().compareTo(arg1.getSimpleName());
    }
}
