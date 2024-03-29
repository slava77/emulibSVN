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
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.dse.DataServiceEntity;
import org.cern.cms.csc.dw.model.dse.DataServiceEntityCollection;

/**
 *
 * @author Evka
 */
@WebService(serviceName="cdw", name="dataservice", targetNamespace="http://www.cern.ch/cms/csc/dw/ws/dataServiceEntityInput")
@Stateless()
public class DataServiceEntityInput {

    private static Logger logger = Logger.getLogger(DataServiceEntityInput.class);
    
    @EJB
    private PersistDaoLocal persistDao;

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
                persistDao.persist(dse);
            }
            return dataServiceEntityCollection.getDataServiceEntities().size();
        } catch (PersistException ex) {
            logger.error("Error while saving DataServiceEntity", ex);
            throw ex;
        }
    }

}
