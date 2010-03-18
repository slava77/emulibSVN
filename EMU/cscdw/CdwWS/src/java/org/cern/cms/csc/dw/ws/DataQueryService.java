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
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.InvalidEntityClassException;

/**
 *
 * @author Evka
 */
@WebService()
@Stateless()
public class DataQueryService {

    private static Logger logger = Logger.getLogger(DataQueryService.class.getName());

    @EJB
    private EntityDaoLocal entityDao;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "entityExists")
    public Boolean entityExists(@WebParam(name = "className") final String className, @WebParam(name = "id") String id) throws InvalidEntityClassException {
        return (entityDao.getEntityById(className, id) != null);
    }

}
