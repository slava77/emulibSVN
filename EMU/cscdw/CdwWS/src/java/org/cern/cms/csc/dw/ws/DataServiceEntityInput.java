/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.xml.bind.JAXBElement;
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

    private static Logger logger = Logger.getLogger(DataServiceEntityInput.class.getName());
    @EJB
    private PersistDaoLocal persistDao;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "getDataServiceEntityCollection")
    public Integer getDataServiceEntityCollection(@WebParam(name = "dataServiceEntityCollection") final DataServiceEntityCollection dataServiceEntityCollection) throws Exception {
        try {
            logger.fine("Data Service: got a DataServiceEntityCollection with " + dataServiceEntityCollection.getDataServiceEntities().size() + " DSEs");
            for (JAXBElement<? extends DataServiceEntity> dseEl: dataServiceEntityCollection.getDataServiceEntities()) {
                DataServiceEntity dse = dseEl.getValue();
                logger.finer("Data Service: forwarding DSE for persistance. dse.toString(): " + dse.toString());
                persistDao.persist(dse);
            }
            return dataServiceEntityCollection.getDataServiceEntities().size();
        } catch (PersistException ex) {
            logger.log(Level.SEVERE, "Error while saving DataServiceEntity", ex);
            throw ex;
        }
    }

}
