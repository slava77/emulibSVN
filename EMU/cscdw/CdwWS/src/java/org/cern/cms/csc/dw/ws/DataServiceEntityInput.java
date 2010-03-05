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
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.exception.PersistException;
import org.cern.cms.csc.dw.model.dse.DataServiceEntity;

/**
 *
 * @author Evka
 */
@WebService(serviceName="cdw", name="dataservice")
@Stateless()
public class DataServiceEntityInput {

    private static Logger logger = Logger.getLogger(DataServiceEntityInput.class.getName());
    @EJB
    private PersistDaoLocal persistDao;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "getDataServiceEntity")
    public Boolean getDataServiceEntity(@WebParam(name = "dataServiceEntity") final DataServiceEntity dataServiceEntity) throws Exception {
        try {
            persistDao.persist(dataServiceEntity);
        } catch (PersistException ex) {
            logger.log(Level.SEVERE, "Error while saving DataServiceEntity", ex);
            throw ex;
        }
        return true;
    }

}
