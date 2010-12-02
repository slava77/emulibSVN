package org.cern.cms.csc.dw.util;

import java.util.Collection;
import org.junit.Test;

public class ClassUtilTest {

    @Test
    public void packageClasses() {

        Collection<Class> cls = ClassUtil.packageClassses("org.cern.cms.csc.dw.model.fact", true);
        for (Class c: cls) {
            System.out.println(c.getName());
        }

    }

}
