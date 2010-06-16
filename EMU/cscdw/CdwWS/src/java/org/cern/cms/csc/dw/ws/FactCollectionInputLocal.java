/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.Local;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import org.cern.cms.csc.dw.model.fact.FactCollection;

/**
 *
 * @author Evka
 */
@Local
public interface FactCollectionInputLocal {

    /**
     * Receive and save fact collection
     * @param factCollection
     * @return number of facts saved
     * @throws java.lang.IllegalArgumentException
     */
    @WebMethod(operationName = "input")
    Integer getFactCollection(@WebParam(name = "factCollection")
    FactCollection factCollection) throws Exception;

}
