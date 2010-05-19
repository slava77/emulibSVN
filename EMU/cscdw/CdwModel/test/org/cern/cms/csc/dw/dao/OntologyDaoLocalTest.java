package org.cern.cms.csc.dw.dao;

import org.cern.cms.csc.dw.TestBase;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentSynonym;
import org.junit.Test;

public class OntologyDaoLocalTest extends TestBase {

    public OntologyDaoLocalTest() {
        super();
    }

    @Test
    public void test() {

        ComponentSynonym cs = (ComponentSynonym) em.find(ComponentSynonym.class, "ME+1/1/1");
        System.out.println(cs.getSynonym());
        if (cs != null) {
            Component c = (Component) em.createQuery("select c from org.cern.cms.csc.dw.model.ontology.Component as c where ? = any elements(c.synonyms)")
                    .setParameter(1, cs)
                    .setHint("org.hibernate.cacheable", new Boolean(true))
                    .getSingleResult();
            System.out.println(c.getName());
        }

    }

}