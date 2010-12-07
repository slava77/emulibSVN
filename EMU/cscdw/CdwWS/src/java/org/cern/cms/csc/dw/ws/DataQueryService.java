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
import org.apache.log4j.Logger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
@WebService(serviceName="cdw", name="dataqueryservice", targetNamespace="http://www.cern.ch/cms/csc/dw/ws/dataQueryService")
@Stateless()
public class DataQueryService {

    private static Logger logger = Logger.getLogger(DataQueryService.class);

    @EJB
    private EntityDaoLocal entityDao;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "entityExists")
    public Boolean entityExists(@WebParam(name = "className") final String className, @WebParam(name = "id") String id) throws InvalidEntityClassException {
        boolean ret = (entityDao.getEntityById(className, id) != null);
        logger.debug("Data query service: got an entityExists query for class " + className + " with ID=" + id + ". Answer:" + ret);
        return ret;
    }

}
