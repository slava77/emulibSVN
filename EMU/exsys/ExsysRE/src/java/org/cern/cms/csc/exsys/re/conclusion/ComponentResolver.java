/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import org.cern.cms.csc.dw.dao.GOntologyDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.ComponentLinkClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.exsys.re.exception.ComponentResolverException;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.ComponentFinder;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTrigger;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource;
import org.cern.cms.csc.exsys.re.model.RelatedComponentFinder;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.SimpleComponentFinder;

/**
 *
 * @author evka
 */
public class ComponentResolver {

    private static final Logger logger = ExsysLogger.getLogger(ComponentResolver.class);

    /** Component finder to be used. */
    private ComponentFinder componentFinder;
    /** Component class type to filter the output components on. */
    private ComponentClassType componentClassType;
    /** Component class type as GComponentClass. */
    private GComponentClass gComponentClass;
    /** Name of the caller e.g. name of the rule that uses it or name of the action that uses it. */
    private String callerName;
    /** OntologyDao. */
    private EjbLookup<OntologyDaoLocal> ontologyDao = new EjbLookup<OntologyDaoLocal>(EjbLookup.Module.DAO,
                                                                                      OntologyDaoLocal.class);
    /** GOntologyDao. */
    private EjbLookup<GOntologyDaoLocal> gOntologyDao = new EjbLookup<GOntologyDaoLocal>(EjbLookup.Module.DAO,
                                                                                         GOntologyDaoLocal.class);

    /**
     * Constructor.
     * @param componentFinder associated component finder.
     * @param callerRule rule that's calling this component resolver (used in log messages).
     */
    public ComponentResolver(ComponentFinder componentFinder, Rule callerRule) {
        this(componentFinder);
        this.callerName = callerRule.toString();
    }

    /**
     * Constructor.
     * @param componentFinder associated component finder.
     * @param callerAction action that's calling this component resolver (used in log messages).
     */
    public ComponentResolver(ComponentFinder componentFinder, Action callerAction) {
        this(componentFinder);
        this.callerName = callerAction.toString();
    }

    private ComponentResolver(ComponentFinder componentFinder) {
        this.componentFinder = componentFinder;
        this.callerName = "N/A";
    }

    /**
     * Resolves component to be assigned to the given conclusion.
     * @param targetConclusion conclusion which you want to resolve the component for.
     * @return component to be assigned to the given conclusion.
     */
    public Component getComponent(Conclusion targetConclusion) throws ComponentResolverException {
        List<Component> sourceComponents = new ArrayList<Component>();

        for (ConclusionTrigger trigger: targetConclusion.getTriggers()) {
            for (ConclusionTriggerSource triggerSource: trigger.getSources()) {
                Fact sourceFact = triggerSource.getFact();
                if (sourceFact != null) {
                    sourceComponents.add(sourceFact.getComponent());
                } else {
                    Conclusion sourceConclusion = triggerSource.getConclusion();
                    if (sourceConclusion != null) {
                        sourceComponents.add(sourceConclusion.getComponent());
                    }
                }
            }
        }

        return getComponent(sourceComponents);
    }

    /**
     * Resolves component to be assigned to a conclusion having the given sources.
     * @param conclusionSources conclusion sources from which you want to resolve the component from.
     * @return component to be assigned to a conclusion having the given sources.
     */
    public Component getComponent(Collection<EntityBase> conclusionSources) throws ComponentResolverException {
        List<Component> sourceComponents = new ArrayList<Component>();
        // get the source components
        for (EntityBase sourceEntity: conclusionSources) {
            if (sourceEntity instanceof Fact) {
                sourceComponents.add(((Fact) sourceEntity).getComponent());
            } else if (sourceEntity instanceof Conclusion) {
                sourceComponents.add(((Conclusion) sourceEntity).getComponent());
            } else if (sourceEntity instanceof Component) {
                sourceComponents.add((Component) sourceEntity);
            }
        }

        return getComponent(sourceComponents);
    }

    /**
     * Resolve the component using the given source components and the component finder that we have
     */
    private Component getComponent(List<Component> sourceComponents) throws ComponentResolverException {
        if (sourceComponents.isEmpty()) {
            throw new ComponentResolverException("Got 0 source components, cannot resolve anything from that.. :) " + callerName);
        }

        if (componentFinder instanceof SimpleComponentFinder) { // SIMPLE component finder - make sure they're all the same and of the required type (if supplied)
            Component firstComp = sourceComponents.get(0);

            for (Component comp: sourceComponents) {
                if (!comp.getId().equals(firstComp.getId())) {
                    throw new ComponentResolverException("Simple component finder error: not all source components are the same, you could consider using a RelatedComponentFinder. Source components: " + sourceComponents +
                                                         ", " + callerName);
                }
            }

            if ((componentFinder.getComponentClass() != null) && !isComponentOfRequiredType(firstComp)) {
                throw new ComponentResolverException("Simple component finder error: one or more of the source components is not of the required type: " + getComponentClassType() +
                                                     ", " + callerName);
            }

            return firstComp;
        } else if (componentFinder instanceof RelatedComponentFinder) { // RELATED component finder
            RelatedComponentFinder relCompFinder = (RelatedComponentFinder) componentFinder;
            Set<Component> retSet = new HashSet<Component>();

            if (componentFinder.getComponentClass() == null) {
                throw new ComponentResolverException("Related component finder doesn't have component class defined!");
            }

            boolean first = true;
            for (Component comp: sourceComponents) { // resolve the related components
                if (first) {
                    retSet.addAll(getRelatedComponents(comp));
                    first = false;
                } else {
                    retSet.retainAll(getRelatedComponents(comp));
                }
            }

            if (retSet.isEmpty()) {
                throw new ComponentResolverException("Related component finder error: could not resolve the component for source components: " + sourceComponents +
                                                     ", " + callerName);
            }
            if (retSet.size() > 1) {
                throw new ComponentResolverException("Related component finder error: resolved more than one component (" + retSet + ") for source components: " + sourceComponents +
                                                    ", " + callerName);
            }
            return retSet.iterator().next();
        }

        throw new ComponentResolverException("Application logic error in ComponentResolver, " + callerName);
    }

    private Collection<Component> getRelatedComponents(Component component) {
        if (!(componentFinder instanceof RelatedComponentFinder)) {
            return Collections.EMPTY_SET;
        }
        Set<Component> retSet = new HashSet<Component>();
        if (componentFinder.getComponentClass().equals(component.getType())) {
            retSet.add(component); // add itself
        }
        Collection<ComponentLinkClassType> linkTypes = new ArrayList<ComponentLinkClassType>();
        for (ComponentLinkClass linkClass: ((RelatedComponentFinder) componentFinder).getLinkClasses()) {
            linkTypes.add(linkClass.getType());
        }
        for (ComponentLinkClassType linkType: linkTypes) {
            try {
                GComponent gComp = gOntologyDao.ejb().getGComponent(component);
                Collection<GComponent> relatedGComps = gComp.findRelatedGComponents(linkType, getGComponentClass());
                for (GComponent relatedGComp: relatedGComps) {
                    retSet.add(ontologyDao.ejb().getComponent(relatedGComp));
                }
            } catch(ComponentNotFoundException cnfEx) {
                logger.warn("ComponentResolver: Couldn't find a GComponent with name=" + component.getName() +
                           ", skipping this component...");
            }
        }
        return retSet;
    }

    /**
     * Checks if the given component is of required type.
     * @param component component to check.
     * @return true if the given component is of required type, false if not.
     */
    private boolean isComponentOfRequiredType(Component component) {
        GComponentClass gCompClass = gOntologyDao.ejb().getGComponentClass(component.getType().getType());
        return ((getGComponentClass().equals(gCompClass)) || gCompClass.getParentsRecursive().contains(getGComponentClass()));
    }

    /**
     * clean up - remove all components that are of different class than those specified in the component finder
     * if component finder doesn't have any component classes specified - return everything
     * @note this is not needed for relating component finder since all resolved components are already of the neccessary types (classes)
     */
    private Set<Component> filterComponents(Set<Component> components) {
        Set<Component> retSet = new HashSet<Component>();
        for (Component comp: components) {
            if (isComponentOfRequiredType(comp)) {
                retSet.add(comp);
            }
        }
        return retSet;
    }

    /**
     * Get component class types to filter the output components on.
     * @return component class types to filter the output components on.
     */
    protected ComponentClassType getComponentClassType() {
        if (componentClassType == null) {
            componentClassType = componentFinder.getComponentClass().getType();
        }
        return componentClassType;
    }

    /**
     * Get component class types as GComponentClass
     * @return component class types as GComponentClass
     */
    protected GComponentClass getGComponentClass() {
        if (gComponentClass == null) {
            gComponentClass = gOntologyDao.ejb().getGComponentClass(getComponentClassType());
        }
        return gComponentClass;
    }

}
