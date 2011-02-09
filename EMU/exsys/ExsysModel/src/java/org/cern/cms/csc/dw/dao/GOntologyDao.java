package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import java.util.Set;
import java.util.TreeSet;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
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
import org.cern.cms.csc.dw.model.ontology.graph.GNode.InternalPropertyType;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GNodeFilter;

@Stateless
public class GOntologyDao implements GOntologyDaoLocal {

    private static final Logger logger = SimpleLogger.getLogger(GOntologyDao.class);

    @EJB
    private GraphServicesDao gdao;

    public GOntologyDao() {
    }

    @Override
    public GComponent getGComponent(Long id) throws ComponentNotFoundException {
        GServices gsvc = gdao.getServices();
        
        GComponent c = gsvc.getGNodeById(GComponent.class, id);
        
        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    @Override
    public GComponent getGComponent(String name) throws ComponentNotFoundException {
        GComponent c = getGComponentSilent(name);

        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, name);
        }

        return c;
    }

    @Override
    public GComponent getGComponent(Component component) throws ComponentNotFoundException {
        return getGComponent(component.getName());
    }

    @Override
    public GComponent getGComponentSilent(String name) {
        GServices gsvc = gdao.getServices();
        GComponent c = gsvc.getGNodeByProperty(GComponent.class, InternalPropertyType.NAME, name);

        if (c == null) {
            GComponentSynonym cs = gsvc.getGNodeByProperty(GComponentSynonym.class, InternalPropertyType.NAME, name);
            if (cs != null) {
                c = cs.getComponent();
            }
        }

        return c;
    }

    @Override
    public GComponentClass getGComponentClass(Long id) throws ComponentClassNotFoundException {
        GServices gsvc = gdao.getServices();
        
        GComponentClass c = gsvc.getGNodeById(GComponentClass.class, id);

        if (c == null) {
            throw new ComponentClassNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    @Override
    public Collection<GComponentClass> getGComponentClasses() {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentClasses();
    }

    @Override
    public GComponentLinkClass getGComponentLinkClass(Long id) throws ComponentLinkClassNotFoundException {
        GServices gsvc = gdao.getServices();
        GComponentLinkClass c = gsvc.getGNodeById(GComponentLinkClass.class, id);

        if (c == null) {
            throw new ComponentLinkClassNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    @Override
    public Collection<GComponentLinkClass> getGComponentLinkClasses() {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentLinkClasses();
    }

    @Override
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

    @Override
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
                InternalPropertyType.NAME,
                query.concat("*"),
                new GNodeFilter() {
            @Override
                    public boolean filter(GNode node) {
                        return (types.isEmpty() || types.contains(((GComponent) node).getType()));
                    }
                },
                numResults - res.size()));

        return res;
    }

    @Override
    public GComponentClass getGComponentClass(ComponentClassType type) {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentClass(type);
    }
 
}
