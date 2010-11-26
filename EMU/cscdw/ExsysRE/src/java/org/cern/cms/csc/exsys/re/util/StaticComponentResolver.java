/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.util;

import java.util.Collection;
import java.util.Iterator;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;

/**
 *
 * @author evka
 */
public class StaticComponentResolver {

    private static final Logger logger = Logger.getLogger(StaticComponentResolver.class.getSimpleName());

    /** OntologyDao. */
    private static OntologyDaoLocal ontologyDao;


    public static Long[] getRelatedIds(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComp, linkTypeName, relatedComponentTypeName);
        Long[] ret = new Long[comps.length];
        for (int i=0; i < comps.length; i++) {
            ret[i] = comps[i].getId();
        }
        return ret;
    }

    public static Long getRelatedSingleId(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        return getRelatedSingle(sourceComp, linkTypeName, relatedComponentTypeName).getId();
    }

    public static Component getRelatedSingle(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComp, linkTypeName, relatedComponentTypeName);
        if (comps.length == 0) {
            return null;
        } else if (comps.length == 1) {
            return comps[0];
        } else if (comps.length > 1) {
            logger.log(Level.WARNING, "StaticComponentResolver.getRelatedSingle called with params ({0}, {1}, {2}) " +
                    "resolved more than one component - returning the first one.",
                    new String[]{sourceComp.getName(), linkTypeName, relatedComponentTypeName});
            return comps[0];
        }
        return null; // dummy - just to keep NetBeans and compiler happy :)
    }

    public static Component[] getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        try {
            ComponentLinkClassType linkType = ComponentLinkClassType.valueOf(linkTypeName);
            GComponent sourceGComp = getOntologyDao().getGComponent(sourceComp.getId());
            GComponentClass relatedToGClass = getOntologyDao().getGComponentClass(ComponentClassType.valueOf(relatedComponentTypeName));

            Collection<GComponent> resolvedGComps = sourceGComp.findRelatedGComponents(linkType, relatedToGClass);
            Component[] resolvedComps = new Component[resolvedGComps.size()];
            Iterator<GComponent> gcit = resolvedGComps.iterator();
            for (int i=0; gcit.hasNext(); i++) {
                Component comp = getOntologyDao().getComponent(gcit.next());
                resolvedComps[i] = comp;
            }

            return resolvedComps;
        } catch (Exception ex) {
            logger.log(Level.SEVERE, "Exception while resolving a related component", ex);
            return null;
        }
    }

    protected static OntologyDaoLocal getOntologyDao() {
        if (ontologyDao == null) {
            Context jniContext;
            try {
                jniContext = new InitialContext();
                ontologyDao = (OntologyDaoLocal) jniContext.lookup("java:comp/env/ejb/OntologyDao");
            } catch (NamingException ex) {
                throw new RuntimeException("Naming exception while looking up ontologyDao ComponentResolver.getOntologyDao()", ex);
            }
        }
        return ontologyDao;
    }

}
