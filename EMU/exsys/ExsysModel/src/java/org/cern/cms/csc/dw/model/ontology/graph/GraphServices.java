package org.cern.cms.csc.dw.model.ontology.graph;

import org.cern.cms.csc.dw.log.Logger;
import org.cern.cms.csc.dw.log.SimpleLogger;

public class GraphServices extends GServices {

    private static final Logger logger = SimpleLogger.getLogger(GraphServices.class);

    public GraphServices(String path) {
        super(path);
        try {
            
            getIdxSrv().enableCache(GNodeImpl.getPropertyKey(GComponent.class, GNode.PropertyType.NAME), 100000);
            //getFTQueryIdxSrv().enableCache(GNodeImpl.getPropertyKey(GComponent.class, GNode.PropertyType.NAME), 100000);

        } catch (ClassNotFoundException ex) {
            logger.error("Error while creating index cache", ex);
        }
    }

}
