package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import junit.framework.Assert;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GNode.PropertyType;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GraphDevServices;
import org.junit.Test;

public class GServicesTest {

    public GServicesTest() {
        super();
    }

    @Test
    public void GNodeByProperty() {
        GServices gsvc = new GraphDevServices();
        try {

            String name = "ME+1/1/09/TMB";
            GComponent c = gsvc.getGNodeByProperty(GComponent.class, GComponentImpl.class, PropertyType.NAME, name);
            Assert.assertNotNull("GComponent is found", c);
            Assert.assertEquals("GComponent found is the one we searched for", c.getName(), name);
            
        }
        finally {
            gsvc.close();
        }
    }

    @Test
    public void GNodeByPropertyFT() {
        GServices gsvc = new GraphDevServices();
        try {

            String name = "ME+1/1/09/TMB";
            Collection<GComponent> m = gsvc.getGNodesByPropertyFT(GComponent.class, GComponentImpl.class, PropertyType.NAME, name);
            assert m.size() == 1;

            m = gsvc.getGNodesByPropertyFT(GComponent.class, GComponentImpl.class, PropertyType.NAME, "ME*");
            Assert.assertFalse("GComponents found in FT", m.size() == 0);

        }
        finally {
            gsvc.close();
        }
    }

}