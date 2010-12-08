/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.EJBs;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClass;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.exsys.re.model.ComponentFinder;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionSourceRelation;
import org.cern.cms.csc.exsys.re.model.IntersectingComponentFinder;
import org.cern.cms.csc.exsys.re.model.MergePolicy;
import org.cern.cms.csc.exsys.re.model.RelatedComponentFinder;
import org.cern.cms.csc.exsys.re.model.UnionComponentFinder;

/**
 *
 * @author evka
 */
public class ComponentResolver {

    private static final Logger logger = Logger.getLogger(ComponentResolver.class.getSimpleName());

    /** Component finder to be used. */
    private ComponentFinder componentFinder;
    /** Component class types to filter the output components on. */
    private Set<ComponentClassType> componentClasses;
    /** Component class types as GComponentClass. */
    private Set<GComponentClass> gComponentClasses;
    /** OntologyDao. */
    private OntologyDaoLocal ontologyDao;

    public ComponentResolver(ComponentFinder componentFinder) {
        this.componentFinder = componentFinder;
    }
    
    /**
     * Resolves components to be assigned to the given conclusion.
     * @param targetConclusion conclusion which you want to resolve the components for.
     * @return components to be assigned to the given conclusion.
     */
    public List<Component> getComponents(Conclusion targetConclusion) {
        List<List<Component>> sourceComponents = new ArrayList<List<Component>>();

        // get the source components
        List<ConclusionSourceRelation> sources = targetConclusion.getChildren();
        for (ConclusionSourceRelation source: sources) {
            Fact sourceFact = source.getChildFact();
            if (sourceFact != null) {
                List<Component> compsHolderCollection = new ArrayList<Component>();
                compsHolderCollection.add(sourceFact.getComponent());
                sourceComponents.add(compsHolderCollection);
            } else {
                Conclusion sourceConclusion = source.getChildConclusion();
                if (sourceConclusion != null) {
                    sourceComponents.add(sourceConclusion.getComponents());
                }
            }
        }

        return new ArrayList<Component>(getComponents(sourceComponents));
    }

    /**
     * Resolves components to be assigned to a conclusion having the given sources.
     * @param conclusionSources conclusion sources from which you want to resolve the components from.
     * @return components to be assigned to a conclusion having the given sources.
     */
    public List<Component> getComponents(Collection<EntityBase> conclusionSources) {
        List<List<Component>> sourceComponents = new ArrayList<List<Component>>();
        // get the source components
        for (EntityBase sourceEntity: conclusionSources) {
            if (sourceEntity instanceof Fact) {
                List<Component> compsHolderCollection = new ArrayList<Component>();
                compsHolderCollection.add(((Fact) sourceEntity).getComponent());
                sourceComponents.add(compsHolderCollection);
            } else if (sourceEntity instanceof Conclusion) {
                sourceComponents.add(((Conclusion) sourceEntity).getComponents());
            } else if (sourceEntity instanceof Component) {
                List<Component> compsHolderCollection = new ArrayList<Component>();
                compsHolderCollection.add((Component) sourceEntity);
                sourceComponents.add(compsHolderCollection);
            }
        }

        return new ArrayList<Component>(getComponents(sourceComponents));
    }

    /**
     * Resolve the components using the given source components and the component finder that we have
     */
    private Set<Component> getComponents(List<List<Component>> sourceComponents) {
        Set<Component> retSet = new HashSet<Component>();

        if (componentFinder instanceof UnionComponentFinder) { // UNION component finder
            for (List<Component> comps: sourceComponents) {
                retSet.addAll(comps);
            }
        } else if (componentFinder instanceof IntersectingComponentFinder) { // INTERSECTION component finder
            boolean first = true;
            for (List<Component> comps: sourceComponents) {
                if (first) {
                    retSet.addAll(comps);
                    first = false;
                } else {
                    retSet.retainAll(comps);
                }
            }
        } else if (componentFinder instanceof RelatedComponentFinder) { // RELATED component finder
            RelatedComponentFinder relCompFinder = (RelatedComponentFinder) componentFinder;
            Set<Component> fullSet = new HashSet<Component>();
            for (List<Component> comps: sourceComponents) { // get union
                fullSet.addAll(comps);
            }

            boolean first = true;
            for (Component comp: fullSet) { // resolve the related components
                if (relCompFinder.getMergePolicy().equals(MergePolicy.UNION) || first) {
                    retSet.addAll(getRelatedComponents(comp));
                    first = false;
                } else if (relCompFinder.getMergePolicy().equals(MergePolicy.INTERSECTION)) {
                    retSet.retainAll(getRelatedComponents(comp));
                }
            }
        }

        // clean up
        if (!(componentFinder instanceof RelatedComponentFinder)) {

            retSet = filterComponents(retSet);
        }

        return retSet;
    }

    private Collection<Component> getRelatedComponents(Component component) {
        if (!(componentFinder instanceof RelatedComponentFinder)) {
            return Collections.EMPTY_SET;
        }
        Set<Component> retSet = new HashSet<Component>();
        if (componentFinder.getComponentClasses().contains(component.getType())) {
            retSet.add(component); // add itself
        }
        ComponentLinkClassType linkType = ((RelatedComponentFinder) componentFinder).getLinkClass().getType();
        Set<GComponentClass> gCompClasses = getGComponentClasses();
        for (GComponentClass gCompClass: gCompClasses) {
            try {
                GComponent gComp = getOntologyDao().getGComponent(component);
                Collection<GComponent> relatedGComps = gComp.findRelatedGComponents(linkType, gCompClass);
                for (GComponent relatedGComp: relatedGComps) {
                    retSet.add(getOntologyDao().getComponent(relatedGComp));
                }
            } catch(ComponentNotFoundException cnfEx) {
                logger.log(Level.WARNING, "ComponentResolver: Couldn't find a GComponent with ID={0} and name={1}, skipping this component...",
                        new Object[]{component.getId(), component.getName()});
            }
        }
        return retSet;
    }

    /**
     * clean up - remove all components that are of different class than those specified in the component finder
     * if component finder doesn't have any component classes specified - return everything
     * @note this is not needed for relating component finder since all resolved components are already of the neccessary types (classes)
     */
    private Set<Component> filterComponents(Set<Component> components) {
        Set<Component> retSet = new HashSet<Component>();
        List<ComponentClass> compClasses = componentFinder.getComponentClasses();
        if (compClasses.isEmpty()) {
            return components;
        } else {
            for (Component comp: components) {
                GComponentClass gCompClass = getOntologyDao().getGComponentClass(comp.getType().getType());
                if (getOntologyDao().isGComponentClassParent(getComponentClasses(), gCompClass, true)) {
                    retSet.add(comp);
                }
            }
        }
        return retSet;
    }

    /**
     * Get component class types to filter the output components on.
     * @return component class types to filter the output components on.
     */
    protected Set<ComponentClassType> getComponentClasses() {
        if (componentClasses == null) {
            componentClasses = new HashSet<ComponentClassType>();
            List<ComponentClass> compClasses = componentFinder.getComponentClasses();
            for (ComponentClass compClass: compClasses) {
                componentClasses.add(compClass.getType());
            }
        }
        return componentClasses;
    }

    /**
     * Get component class types as GComponentClass
     * @return component class types as GComponentClass
     */
    protected Set<GComponentClass> getGComponentClasses() {
        if (gComponentClasses == null) {
            gComponentClasses = new HashSet<GComponentClass>();
            for (ComponentClassType compClassType: getComponentClasses()) {
                gComponentClasses.add(getOntologyDao().getGComponentClass(compClassType));
            }
        }
        return gComponentClasses;
    }

    protected OntologyDaoLocal getOntologyDao() {
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
