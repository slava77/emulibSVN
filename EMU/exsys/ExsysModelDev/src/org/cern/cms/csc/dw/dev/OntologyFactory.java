package org.cern.cms.csc.dw.dev;

import java.util.LinkedHashMap;
import java.util.Map;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentSynonym;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonym;

public class OntologyFactory {

    private Map<ComponentClassType, ComponentClass> componentClasses = new LinkedHashMap<ComponentClassType, ComponentClass>();
    private Map<ComponentLinkClassType, ComponentLinkClass> componentLinkClasses = new LinkedHashMap<ComponentLinkClassType, ComponentLinkClass>();

    public Component getComponent(GComponent gc) {
        Component c = new Component();
        c.setId(gc.getId());
        c.setName(gc.getName());
        c.setType(getComponentClass(gc.getType()));
        c.setDescription(gc.getDescription());
        for (GComponentSynonym gcs: gc.getSynonyms()) {
            ComponentSynonym cs = new ComponentSynonym();
            cs.setName(gcs.getName());
            c.getSynonyms().add(cs);
        }
        return c;
    }

    public ComponentClass getComponentClass(GComponentClass gcc) {
        if (!componentClasses.containsKey(gcc.getType())) {
            ComponentClass cc = new ComponentClass();
            cc.setId(gcc.getId());
            cc.setType(gcc.getType());
            cc.setDescription(gcc.getDescription());

            componentClasses.put(cc.getType(), cc);

            for (GComponentClass pgcc: gcc.getParents()) {
                cc.getParents().add(getComponentClass(pgcc));
            }
        }
        return componentClasses.get(gcc.getType());
    }

    public ComponentLinkClass getComponentLinkClass(GComponentLinkClass gclc) {
        if (!componentLinkClasses.containsKey(gclc.getType())) {
            ComponentLinkClass clc = new ComponentLinkClass();
            clc.setId(gclc.getId());
            clc.setType(gclc.getType());
            clc.setDescription(gclc.getDescription());
            clc.setTransitive(gclc.getTransitive());

            componentLinkClasses.put(clc.getType(), clc);

            if (gclc.getParent() != null) {
                clc.setParent(getComponentLinkClass(gclc.getParent()));
            }
            if (gclc.getInverse() != null) {
                clc.setInverse(getComponentLinkClass(gclc.getInverse()));
            }
            
        }
        return componentLinkClasses.get(gclc.getType());
    }

}
