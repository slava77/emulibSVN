package org.cern.cms.csc.dw.dao;

import java.io.File;
import java.net.URL;
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GraphServices;
import org.cern.cms.csc.dw.util.ZipUtil;

/**
 * A Custom Resource has to be created!
 * Edit Custom Resource
 * JNDI Name: OntologyGraphServices
 * Resource Type: org.cern.cms.csc.dw.dao.GraphServicesDao
 * Factory Class: org.cern.cms.csc.dw.util.GenericImmutableSingletonFactory
 * Description: Ontology Graph Services Dao
 * Status: Enabled
 * @author valdo
 */

public class GraphServicesDao {

    private static final Logger logger = Logger.getLogger(GraphServices.class);
    private static final String graphdbPath = "graphdb";
    
    private GraphServices graphServices;

    public GraphServicesDao() {
        graphServices = null;
    }

    public void init() {

        try {

            logger.info("Cleaning graphdb");
            {
                File root = new File(graphdbPath);
                if (root.exists()) {
                    logger.info("graphdb found in [" + root.getAbsolutePath() + "]. Removing...");
                    ZipUtil.deleteRecursive(root);
                }
            }

            logger.info("Unpacking graphdb");
            {
                File root = new File(graphdbPath);
                root.mkdir();
                URL zipUrl = this.getClass().getClassLoader().getResource("org/cern/cms/csc/dw/model/graphdb.zip");
                logger.info("Unpacking graphdb from [" + zipUrl.getFile() + "] to [" + root.getAbsolutePath() + "]");
                ZipUtil.unzip(new File(zipUrl.getFile()), root);
            }

            graphServices = new GraphServices(graphdbPath);

        } catch (Exception ex) {
            logger.error(ex.getMessage(), ex);
        }

    }

    public GServices getServices() {
        return graphServices;
    }

}