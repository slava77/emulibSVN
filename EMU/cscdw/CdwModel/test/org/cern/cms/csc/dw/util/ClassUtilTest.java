package org.cern.cms.csc.dw.util;

import java.util.Collection;
import org.apache.log4j.Logger;
import org.junit.Test;

public class ClassUtilTest {

    private static final Logger logger = Logger.getLogger(ClassUtilTest.class);

    @Test
    public void packageClasses() {

        Collection<Class> cls = ClassUtil.packageClassses("org.cern.cms.csc.dw.model.fact", false);
        for (Class c: cls) {
            logger.info(c.getName());
        }

    }

}
