package org.cern.cms.csc.dw.dao;

import java.io.File;
import java.net.URL;
import javax.ejb.Singleton;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.ontology.graph.GServices;
import org.cern.cms.csc.dw.model.ontology.graph.GraphServices;
import org.cern.cms.csc.dw.util.ZipUtil;

@Singleton
public class GraphServicesDao {

    private static final Logger logger = ExsysLogger.getLogger(GraphServices.class);
    private static final String graphdbPath = "graphdb";
    
    private GraphServices graphServices;

    public GraphServicesDao() {

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
                URL zipUrl = this.getClass().getClassLoader().getResource("graphdb.zip");
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
