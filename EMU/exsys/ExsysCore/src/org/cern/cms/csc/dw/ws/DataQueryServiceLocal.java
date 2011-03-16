/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.Local;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import jsf.bean.gui.exception.InvalidEntityClassException;

/**
 *
 * @author evka
 */
@Local
public interface DataQueryServiceLocal {

    /**
     * Web service operation
     */
    @WebMethod(operationName = "entityExists")
    Boolean entityExists(@WebParam(name = "className")
    final String className, @WebParam(name = "id")
    String id) throws InvalidEntityClassException;

}
