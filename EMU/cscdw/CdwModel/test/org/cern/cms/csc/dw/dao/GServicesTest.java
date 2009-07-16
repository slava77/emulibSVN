package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import junit.framework.Assert;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
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
            GComponent c = gsvc.getGNodeByProperty(GComponent.class, PropertyType.NAME, name);
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
            Collection<GComponent> m = gsvc.getGNodesByPropertyFT(GComponent.class, PropertyType.NAME, name);
            Assert.assertEquals("GComponent is found and is one", m.size(), 1);

            m = gsvc.getGNodesByPropertyFT(GComponent.class, PropertyType.NAME, "ME*");
            Assert.assertFalse("GComponents found in FT", m.size() == 0);

        }
        finally {
            gsvc.close();
        }
    }

    @Test
    public void ComponentsRecursive() {
        GServices gsvc = new GraphDevServices();
        try {

            GComponentClass soft = gsvc.getGComponentClass(ComponentClassType.SOFTWARE);
            Assert.assertEquals("Software class", soft.getType(), ComponentClassType.SOFTWARE);

            Collection<GComponent> components = soft.getComponents();
            Assert.assertEquals("No direct components", components.size(), 0);

            components = soft.getComponentsRecursive();
            Assert.assertTrue("Many indirect components", components.size() > 0);

        }
        finally {
            gsvc.close();
        }
    }

}