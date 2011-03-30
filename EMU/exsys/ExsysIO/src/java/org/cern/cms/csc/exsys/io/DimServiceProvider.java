/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.io;

import dim.DimServer;
import dim.DimService;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import javax.annotation.PostConstruct;
import javax.ejb.Singleton;
import javax.ejb.Startup;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.dao.EntityDaoRemote;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.util.EjbLookup;
import org.cern.cms.csc.dw.util.ExsysRemoteEjbLookup;
import org.cern.cms.csc.dw.util.PropertiesProvider;
import org.cern.cms.csc.exsys.exception.DimException;
import org.cern.cms.csc.exsys.re.model.DimPublicationAction;
import org.cern.cms.csc.exsys.re.model.DimServiceDataType;

/**
 *
 * @author evka
 */
@Singleton
@Startup
public class DimServiceProvider implements DimServiceProviderRemote {

    private static final Logger logger = ExsysLogger.getLogger(DimServiceProvider.class);

    private static final String DIM_SERVER_NAME = "EXPERT-SYSTEM";

    private EjbLookup<EntityDaoRemote> entityDao = new ExsysRemoteEjbLookup<EntityDaoRemote>(EntityDaoRemote.class);

    private Map<String, DimService> services = new HashMap<String, DimService>();

    private boolean isInitialized = false;

    public DimServiceProvider() {
    }

    @PostConstruct
    private void init() {
        logger.info("Starting up DIM services...");

        List<DimPublicationAction> dimPubActions;
        try {
            dimPubActions = entityDao.ejbStrict().getAllEntitiesByClass(DimPublicationAction.class);
        } catch (Exception ex) {
            logger.error("Failed to initialize DIM services", ex);
            return;
        }

        Map<String, DimPublicationAction> serviceNameToDimPubAction = new HashMap<String, DimPublicationAction>();
        for (DimPublicationAction dimPubAction: dimPubActions) {
            if (!dimPubAction.isEnabled()) {
                continue;
            }
            if (!serviceNameToDimPubAction.containsKey(dimPubAction.getServiceName())) { // service doesn't exist yet
                services.put(dimPubAction.getServiceName(), createDimService(dimPubAction));
                serviceNameToDimPubAction.put(dimPubAction.getServiceName(), dimPubAction);
            } else { // service already exist - generally this means that we skip the creation, but we also check if the types match
                if (!serviceNameToDimPubAction.get(dimPubAction.getServiceName()).getDataType().equals(dimPubAction.getDataType())) {
                    throw new RuntimeException("Couldn't start DIM services, because found several DimPublicationActions with the same" +
                                               " service name but different data types. Action names: " +
                                               serviceNameToDimPubAction.get(dimPubAction.getServiceName()).getName() + " and " +
                                               dimPubAction.getName());
                }
            }
        }


        DimServer.setDnsNode(PropertiesProvider.getProperty("exsysIO.DIM.DNS"));
        DimServer.start(DIM_SERVER_NAME);

        isInitialized = true;
        logger.info("DIM services have been started up");
    }

    @Override
    public void publishString(DimPublicationAction dimPubAction, String value) throws DimException {
        getService(dimPubAction).updateService(value);
    }

    private DimService getService(DimPublicationAction dimPubAction) throws DimException {
        if (!isInitialized) {
            init();
        }
        if (!isInitialized) {
            throw new DimException("DIM services could not be initialized");
        }
        return services.get(dimPubAction.getServiceName());
    }

    private DimService createDimService(DimPublicationAction dimPubAction) {
        DimServiceDataType dataType = dimPubAction.getDataType();
        if (dataType.equals(DimServiceDataType.STRING)) {
            return new DimService(dimPubAction.getServiceName(), "");
        } else if (dataType.equals(DimServiceDataType.BOOLEAN)) {
            return new DimService(dimPubAction.getServiceName(), false);
        } else if (dataType.equals(DimServiceDataType.INT)) {
            int i = 0;
            return new DimService(dimPubAction.getServiceName(), i);
        } else if (dataType.equals(DimServiceDataType.LONG)) {
            long l = 0;
            return new DimService(dimPubAction.getServiceName(), l);
        } else if (dataType.equals(DimServiceDataType.FLOAT)) {
            float f = 0;
            return new DimService(dimPubAction.getServiceName(), f);
        } else if (dataType.equals(DimServiceDataType.FLOAT)) {
            double d = 0;
            return new DimService(dimPubAction.getServiceName(), d);
        }

        RuntimeException rex = new RuntimeException("Unsupported DIM service data type: " + dataType);
        logger.critical(rex);
        throw rex;
    }

}
