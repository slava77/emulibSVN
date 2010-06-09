package org.cern.cms.csc.dw.model.ontology.graph;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.annotation.Resource;

@Resource
public class GraphServices extends GServices {

    private static final Logger log = Logger.getLogger(GraphServices.class.getName());

    public GraphServices(String path) {
        super(path);
        try {
            
            getIdxSrv().enableCache(GNodeImpl.getPropertyKey(GComponent.class, GNode.PropertyType.NAME), 100000);
            //getFTQueryIdxSrv().enableCache(GNodeImpl.getPropertyKey(GComponent.class, GNode.PropertyType.NAME), 100000);

        } catch (ClassNotFoundException ex) {
            log.log(Level.SEVERE, "Error while creating index cache", ex);
        }
    }

}
