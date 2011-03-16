package org.cern.cms.csc.dw.model.ontology.graph;

import java.lang.reflect.Constructor;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.neo4j.graphdb.Relationship;

public class GLinkImpl extends GBase implements GLink {

    private static final Logger logger = SimpleLogger.getLogger(GLinkImpl.class);
    
    protected Relationship relationship;

    public GLinkImpl(GServices gservices, Relationship relationship) {
        super(gservices);
        this.relationship = relationship;
    }

    public Long getId() {
        return relationship.getId();
    }

    public Relationship getRelationship() {
        return relationship;
    }

    protected <T extends GLink> Collection<T> wrap(Class<T> ifClass, Class elClass, Iterator<Relationship> it) {
        return wrap(gservices, ifClass, elClass, it);
    }

    @SuppressWarnings("unchecked")
    protected static <T extends GLink> Collection<T> wrap(GServices gservices, Class<T> ifClass, Class elClass, Iterator<Relationship> it) {
        try {
            Constructor constr = elClass.getConstructor(GServices.class, Relationship.class);
            Collection<T>  ret = new LinkedList<T>();
            while (it.hasNext()) {
                ret.add((T) constr.newInstance(gservices, it.next()));
            }
            return ret;
        } catch (Exception e) {
            logger.error(null, e);
            return null;
        }
    }

}
