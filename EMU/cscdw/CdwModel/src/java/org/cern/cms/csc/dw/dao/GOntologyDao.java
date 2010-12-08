package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import java.util.Set;
import java.util.TreeSet;
import javax.annotation.Resource;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.exception.OntologyException.OntologySource;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonym;
import org.cern.cms.csc.dw.model.ontology.graph.GNode;
import org.cern.cms.csc.dw.model.ontology.graph.GNode.PropertyType;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GNodeFilter;

public class GOntologyDao implements GOntologyDaoLocal {

    @Resource(name="OntologyGraphServices")
    private GraphServicesDao gdao;

    public GOntologyDao() {
    }

    public GComponent getGComponent(Long id) throws ComponentNotFoundException {
        GServices gsvc = gdao.getServices();
        
        GComponent c = gsvc.getGNodeById(GComponent.class, id);
        
        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    public GComponent getGComponent(String name) throws ComponentNotFoundException {
        GComponent c = getGComponentSilent(name);

        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, name);
        }

        return c;
    }

    public GComponent getGComponent(Component component) throws ComponentNotFoundException {
        return getGComponent(component.getName());
    }

    public GComponent getGComponentSilent(String name) {
        GServices gsvc = gdao.getServices();
        GComponent c = gsvc.getGNodeByProperty(GComponent.class, PropertyType.NAME, name);

        if (c == null) {
            GComponentSynonym cs = gsvc.getGNodeByProperty(GComponentSynonym.class, PropertyType.NAME, name);
            if (cs != null) {
                c = cs.getComponent();
            }
        }

        return c;
    }

    public GComponentClass getGComponentClass(Long id) throws ComponentClassNotFoundException {
        GServices gsvc = gdao.getServices();
        
        GComponentClass c = gsvc.getGNodeById(GComponentClass.class, id);

        if (c == null) {
            throw new ComponentClassNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    public Collection<GComponentClass> getGComponentClasses() {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentClasses();
    }

    public GComponentLinkClass getGComponentLinkClass(Long id) throws ComponentLinkClassNotFoundException {
        GServices gsvc = gdao.getServices();
        GComponentLinkClass c = gsvc.getGNodeById(GComponentLinkClass.class, id);

        if (c == null) {
            throw new ComponentLinkClassNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    public Collection<GComponentLinkClass> getGComponentLinkClasses() {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentLinkClasses();
    }

    public boolean isGComponentClassParent(Set<ComponentClassType> parents, GComponentClass toCheck, boolean recursive) {

        if (parents.isEmpty() || parents.contains(toCheck.getType())) {
            return true;
        }

        if (recursive) {
            for (GComponentClass parent: toCheck.getParentsRecursive()) {
                if (parents.contains(parent.getType())) {
                    return true;
                }
            }
        }
        
        return false;

    }

    public Collection<GComponent> getGComponentsByNameMatches(String query, final Collection<GComponentClass> types, long numResults) {
        GServices gsvc = gdao.getServices();
        Collection<GComponent> res = new TreeSet<GComponent>();
        
        GComponent direct = getGComponentSilent(query);
        if (direct != null) {
            if (types.isEmpty() || types.contains(direct.getType())) {
                res.add(direct);
            }
        }

        res.addAll(
            gsvc.getGNodesByPropertyFT(
                GComponent.class,
                PropertyType.NAME,
                query.concat("*"),
                new GNodeFilter() {
                    public boolean filter(GNode node) {
                        return (types.isEmpty() || types.contains(((GComponent) node).getType()));
                    }
                },
                numResults - res.size()));

        return res;
    }

    public GComponentClass getGComponentClass(ComponentClassType type) {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentClass(type);
    }
 
}
