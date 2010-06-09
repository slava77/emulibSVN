package org.cern.cms.csc.dw.metadata;

import java.util.Collection;
import java.util.Collections;
import org.cern.cms.csc.dw.util.ClassUtil;
import java.util.HashMap;
import java.util.Map;
import javax.persistence.Entity;
import org.cern.cms.csc.dw.model.annotation.FactAnn;
import org.cern.cms.csc.dw.model.fact.Fact;

@SuppressWarnings("unchecked")
public class MetadataManager {

    private static Map<Class, FactMd> factClasses = null;

    public MetadataManager() {
        if (factClasses == null) {
            factClasses = new HashMap<Class, FactMd>();
            for (Class c: ClassUtil.getLoadedClasses()) {
                if (c.getSuperclass() != null) {
                    if (c.getSuperclass().equals(Fact.class) && c.isAnnotationPresent(Entity.class) && c.isAnnotationPresent(FactAnn.class)) {
                        factClasses.put(c, new FactMd(c));
                    }
                }
            }
        }
    }

    public final Collection<FactMd> getFactClasses() {
        return Collections.unmodifiableCollection(factClasses.values());
    }

    public final FactMd getFactMd(Class clazz) {
        return factClasses.get(clazz);
    }

}
