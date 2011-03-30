package org.cern.cms.csc.exsys.re;

import com.espertech.esper.client.Configuration;
import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPServiceProviderManager;
import java.util.HashMap;
import java.util.Map;
import javax.ejb.Singleton;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;

@Singleton
public class EsperServiceProvider {

    private static Logger logger = ExsysLogger.getLogger(EsperServiceProvider.class);

    private Map<String, EPServiceProvider> epServices = new HashMap<String, EPServiceProvider>();

    public EsperServiceProvider() {}

    /**
     * Creates the default service on startup.
     */
    public void init() {
//        getEpService(RuleEngineManager.DEFAULT_EP_URI); // no way to do configuration...
    }

    /**
     * Checks if this class is already holding an instance of an Esper service for the the given URI.
     */
    public synchronized boolean epServiceExists(String uri) {
        return epServices.containsKey(uri);
    }

    /**
     * @return an instance of esper service for the given URI. If it doesn't exist yet - a new one is created by calling createEpService(uri).
     */
    public synchronized EPServiceProvider getEpService(String uri) {
        if (!epServices.containsKey(uri)) {
            epServices.put(uri, createEpService(uri));
        }
        return epServices.get(uri);
    }

    /**
     * Creates a new Esper service for the given URI.
     */
    private EPServiceProvider createEpService(String uri) {
        // check if the service exists in EPServiceProviderManager
        String[] uris = EPServiceProviderManager.getProviderURIs();
        boolean serviceExists = false;
        for (String currentUri: uris) {
            if (currentUri.equals(uri)) {
                serviceExists = true;
                break;
            }
        }

        EPServiceProvider epService;
        if (serviceExists) { // Esper service already exists so just and return it
            epService = EPServiceProviderManager.getProvider(uri);
        } else { // create and configure the Esper service
            logger.info("Initializing rules engine (Esper service URI=" + uri + ")");
            Configuration conf = new Configuration();
            conf.configure("org/cern/cms/csc/exsys/re/conf/esper.cfg.xml");
            epService = EPServiceProviderManager.getProvider(uri, conf);
            logger.info("Rules engine initialized (Esper service URI=" + uri + ")");
            //reManager.configure(epService);
        }

        return epService;
    }
}
