package org.cern.cms.csc.dw.dao;

import java.util.Collection;
import java.util.LinkedList;
import java.util.Set;
import javax.annotation.Resource;
import org.cern.cms.csc.dw.exception.ComponentClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentLinkClassNotFoundException;
import org.cern.cms.csc.dw.exception.ComponentNotFoundException;
import org.cern.cms.csc.dw.exception.OntologyException.OntologySource;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentClassImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClass;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentLinkClassImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonym;
import org.cern.cms.csc.dw.model.ontology.graph.GComponentSynonymImpl;
import org.cern.cms.csc.dw.model.ontology.graph.GNode.PropertyType;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.neo4j.graphdb.Transaction;

public class GOntologyDao implements GOntologyDaoLocal {

    @Resource(name="OntologyGraphServices")
    private GraphServicesDao gdao;

    public GOntologyDao() {
    }

    public GComponent getGComponentById(Long id) throws ComponentNotFoundException {
        GServices gsvc = gdao.getServices();
        GComponent c = null;
        Transaction tx = gsvc.beginTx();
        try {
            c = new GComponentImpl(gsvc, gsvc.getDBSrv().getNodeById(id));
            tx.success();
        } finally {
            tx.finish();
        }

        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    public GComponent getGComponentByName(String name) throws ComponentNotFoundException {
        GComponent c = getGComponentByNameSilent(name);

        if (c == null) {
            throw new ComponentNotFoundException(OntologySource.GRAPH_DATABASE, name);
        }

        return c;
    }

    public GComponent getGComponentByNameSilent(String name) {
        GServices gsvc = gdao.getServices();
        GComponent c = gsvc.getGNodeByProperty(GComponent.class, GComponentImpl.class, PropertyType.NAME, name);

        if (c == null) {
            GComponentSynonym cs = gsvc.getGNodeByProperty(GComponentSynonym.class, GComponentSynonymImpl.class, PropertyType.NAME, name);
            if (cs != null) {
                c = cs.getComponent();
            }
        }

        return c;
    }

    public GComponentClass getGComponentClassById(Long id) throws ComponentClassNotFoundException {
        GServices gsvc = gdao.getServices();
        GComponentClass c = null;
        Transaction tx = gsvc.beginTx();
        try {
            c = new GComponentClassImpl(gsvc, gsvc.getDBSrv().getNodeById(id));
            tx.success();
        } finally {
            tx.finish();
        }

        if (c == null) {
            throw new ComponentClassNotFoundException(OntologySource.GRAPH_DATABASE, id);
        }

        return c;
    }

    public Collection<GComponentClass> getGComponentClasses() {
        GServices gsvc = gdao.getServices();
        return gsvc.getGComponentClasses();
    }

    public GComponentLinkClass getGComponentLinkClassById(Long id) throws ComponentLinkClassNotFoundException {
        GServices gsvc = gdao.getServices();
        GComponentLinkClass c = null;
        Transaction tx = gsvc.beginTx();
        try {
            c = new GComponentLinkClassImpl(gsvc, gsvc.getDBSrv().getNodeById(id));
            tx.success();
        } finally {
            tx.finish();
        }

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

        if (parents.isEmpty()) {
            return true;
        }
        
        if (recursive) {
            for (GComponentClass parent: toCheck.getParents()) {
                if (parents.contains(parent.getType())) {
                    return true;
                }
            }
            return false;
        }
        
        return parents.contains(toCheck.getType());

    }

    public Collection<GComponent> getGComponentsByNameMatches(String query, long numResults) {
        GServices gsvc = gdao.getServices();

        Collection<GComponent> res = new LinkedList<GComponent>();
        GComponent direct = getGComponentByNameSilent(query);
        if (direct != null) {
            res.add(direct);
        }

        return gsvc.getGNodesByPropertyFT(
                GComponent.class,
                GComponentImpl.class,
                PropertyType.NAME,
                query,
                numResults - res.size());
    }
 
}
