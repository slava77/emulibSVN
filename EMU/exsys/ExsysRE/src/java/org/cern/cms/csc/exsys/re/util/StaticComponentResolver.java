/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.util;

import java.util.Arrays;
import java.util.Collection;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;
import org.cern.cms.csc.dw.dao.GOntologyDao;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.util.EjbLookup;

/**
 *
 * @author evka
 */
public class StaticComponentResolver {

    private static final Logger logger = SimpleLogger.getLogger(StaticComponentResolver.class);

    /** OntologyDao. */
    private static EjbLookup<OntologyDaoLocal> ontologyDao = new EjbLookup<OntologyDaoLocal>(OntologyDaoLocal.class, OntologyDaoLocal.class);
    /** GOntologyDao. */
    private static EjbLookup<GOntologyDaoLocal> gOntologyDao = new EjbLookup<GOntologyDaoLocal>(GOntologyDaoLocal.class, GOntologyDao.class);


    /**
     * Same as getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName),
     * but returns component IDs instead of components themselves (useful in rules engine e.g. to do comp ID in {other IDs})
     * @see getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName)
     */
    public static Long[] getRelatedIds(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComp, linkTypeName, relatedComponentTypeName);
        if (comps == null) {
            return new Long[0];
        }
        Long[] ret = new Long[comps.length];
        for (int i=0; i < comps.length; i++) {
            ret[i] = comps[i].getId();
        }
        return ret;
    }
    /**
     * Same as getRelated(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName),
     * but returns component IDs instead of components themselves (useful in rules engine e.g. to do comp ID in {other IDs})
     * @see getRelated(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName)
     */
    public static Long[] getRelatedIds(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComps, linkTypeName, relatedComponentTypeName);
        if (comps == null) {
            return new Long[0];
        }
        Long[] ret = new Long[comps.length];
        for (int i=0; i < comps.length; i++) {
            ret[i] = comps[i].getId();
        }
        return ret;
    }

    /**
     * Same as getRelatedSingle(Component sourceComp, String linkTypeName, String relatedComponentTypeName),
     * but returns component ID instead of the component itself.
     * @see getRelatedSingle(Component sourceComp, String linkTypeName, String relatedComponentTypeName)
     */
    public static Long getRelatedSingleId(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        return getRelatedSingle(sourceComp, linkTypeName, relatedComponentTypeName).getId();
    }

    /**
     * Same as getRelatedSingle(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName),
     * but returns component ID instead of the component itself.
     * @see getRelatedSingle(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName)
     */
    public static Long getRelatedSingleId(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName) {
        Component comp = getRelatedSingle(sourceComps, linkTypeName, relatedComponentTypeName);
        if (comp == null) {
            return new Long(-1);
        }
        return comp.getId();
    }

    /**
     * Same as getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName), but returns only one component.
     * If more than one related components are found - a warning message is printed to the log file and only the first one is returned.
     * @see getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName)
     */
    public static Component getRelatedSingle(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComp, linkTypeName, relatedComponentTypeName);
        if (comps.length == 0) {
            return null;
        } else if (comps.length == 1) {
            return comps[0];
        } else if (comps.length > 1) {
            logger.warn("StaticComponentResolver.getRelatedSingle called with params " +
                    "(" + sourceComp.getName() + ", " + linkTypeName + ", " + relatedComponentTypeName + ") " +
                    "resolved more than one component - returning the first one.");
            return comps[0];
        }
        return null; // dummy - just to keep NetBeans and compiler happy :)
    }

    /**
     * Same as getRelated(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName), but returns only one component.
     * If more than one related components are found - a warning message is printed to the log file and only the first one is returned.
     * @see getRelated(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName)
     */
    public static Component getRelatedSingle(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName) {
        Component[] comps = getRelated(sourceComps, linkTypeName, relatedComponentTypeName);
        if ((comps == null) || (comps.length == 0)) {
            return null;
        } else if (comps.length == 1) {
            return comps[0];
        } else if (comps.length > 1) {
            logger.warn("StaticComponentResolver.getRelatedSingle called with params " +
                    "(" + sourceComps + ", " + linkTypeName + ", " + relatedComponentTypeName + ") " +
                    "resolved more than one component - returning the first one.");
            return comps[0];
        }
        return null; // dummy - just to keep NetBeans and compiler happy :)
    }

    /**
     * Returns one or more components that are related to sourceComp by a link having a name=linkTypeName and which' type name is relatedComponentTypeName
     * @param sourceComp source component
     * @param linkTypeName name of the relation link type
     * @param relatedComponentTypeName name of type of the related component(s) that you want to get
     * @return one or more components that are related to sourceComp by a link having a name=linkTypeName and which' type name is relatedComponentTypeName
     */
    public static Component[] getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName) {
        try {
            ComponentLinkClassType linkType = ComponentLinkClassType.valueOf(linkTypeName);
            GComponent sourceGComp = gOntologyDao.ejb().getGComponent(sourceComp);
            GComponentClass relatedToGClass = gOntologyDao.ejb().getGComponentClass(ComponentClassType.valueOf(relatedComponentTypeName));

            Collection<GComponent> resolvedGComps = sourceGComp.findRelatedGComponents(linkType, relatedToGClass);
            Component[] resolvedComps = new Component[resolvedGComps.size()];
            Iterator<GComponent> gcit = resolvedGComps.iterator();
            for (int i=0; gcit.hasNext(); i++) {
                Component comp = ontologyDao.ejb().getComponent(gcit.next());
                resolvedComps[i] = comp;
            }

            return resolvedComps;
        } catch (Exception ex) {
            String sourceCompName = "null";
            if (sourceComp != null) {
                sourceCompName = sourceComp.getName();
            }
            logger.error("Exception while resolving a related component: from "
                        + sourceCompName + " to " + relatedComponentTypeName + ", through link type " + linkTypeName,
                        ex);
            return null;
        }
    }

    /**
     * Same as getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName), but accepts a collection of source
     * components - useful e.g. when wanting to get related components for a conclusion (which carries a collection of components)
     * @see getRelated(Component sourceComp, String linkTypeName, String relatedComponentTypeName)
     */
    public static Component[] getRelated(Collection<Component> sourceComps, String linkTypeName, String relatedComponentTypeName) {
        Set<Component> ret = new HashSet<Component>();
        for (Component sourceComp: sourceComps) {
            Component[] comps = getRelated(sourceComp, linkTypeName, relatedComponentTypeName);
            if (comps != null) {
                ret.addAll(Arrays.asList(comps));
            }
        }

        return ret.toArray(new Component[]{});
    }

}
