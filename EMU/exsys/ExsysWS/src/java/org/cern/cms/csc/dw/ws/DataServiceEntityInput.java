/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.EJB;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.xml.bind.JAXBElement;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.exception.PersistException;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.model.dse.DataServiceEntity;
import org.cern.cms.csc.dw.model.dse.DataServiceEntityCollection;

/**
 *
 * @author Evka
 */
@WebService(serviceName="cdw", name="dataservice", targetNamespace="http://www.cern.ch/cms/csc/dw/ws/dataServiceEntityInput")
@Stateless()
public class DataServiceEntityInput implements DataServiceEntityInputLocal {

    private static Logger logger = SimpleLogger.getLogger(DataServiceEntityInput.class);
    
    @EJB
    private EntityDaoLocal entityDao;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "getDataServiceEntityCollection")
    public Integer getDataServiceEntityCollection(@WebParam(name = "dataServiceEntityCollection") final DataServiceEntityCollection dataServiceEntityCollection) throws Exception {
        try {
            logger.debug("Data Service: got a DataServiceEntityCollection with " + dataServiceEntityCollection.getDataServiceEntities().size() + " DSEs");
            for (JAXBElement<? extends DataServiceEntity> dseEl: dataServiceEntityCollection.getDataServiceEntities()) {
                DataServiceEntity dse = dseEl.getValue();
                logger.debug("Data Service: forwarding DSE for persistance. dse.toString(): " + dse.toString());
                entityDao.persist(dse);
            }
            return dataServiceEntityCollection.getDataServiceEntities().size();
        } catch (PersistException ex) {
            logger.error("Error while saving DataServiceEntity", ex);
            throw ex;
        }
    }

}
