/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.model.FactCollection;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;

/**
 *
 * @author evka
 */
@WebService()
@Stateless()
public class FactCollectionInput {

    @PersistenceContext(name="CdwPU")
    private EntityManager em;

    /**
     * Web service operation
     */
    @WebMethod(operationName = "getFactCollection")
    public Integer getFactCollection(@WebParam(name = "factCollection")
    final FactCollection factCollection) {
        if (factCollection == null) {
            return 0;
        }
        try {
            em.persist(factCollection);
        } catch (Exception ex) {
            ex.printStackTrace();
            return 0;
        }
        return factCollection.getFact().size();
    }

}
