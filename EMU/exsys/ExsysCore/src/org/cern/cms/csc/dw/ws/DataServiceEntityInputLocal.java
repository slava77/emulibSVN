/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.Local;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import org.cern.cms.csc.dw.model.dse.DataServiceEntityCollection;

/**
 *
 * @author evka
 */
@Local
public interface DataServiceEntityInputLocal {

    /**
     * Web service operation
     */
    @WebMethod(operationName = "getDataServiceEntityCollection")
    Integer getDataServiceEntityCollection(@WebParam(name = "dataServiceEntityCollection")
    final DataServiceEntityCollection dataServiceEntityCollection) throws Exception;

}
