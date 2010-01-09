/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.model.fact.FactCollection;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.model.ontology.Component;

/**
 *
 * @author evka
 */
@WebService(serviceName="factcollection",name="input")
@Stateless()
public class FactCollectionInput {

    @PersistenceContext(name="CdwPU")
    private EntityManager em;

    /**
     * Receive and save fact collection
     * @param factCollection
     * @return
     * @throws java.lang.IllegalArgumentException
     */
    @WebMethod(operationName = "set")
    public Integer getFactCollection(@WebParam(name = "factCollection")
    final FactCollection factCollection) throws Exception {

        // If collection is empty - return
        if (factCollection == null) {
            return 0;
        }

        try {

            // Loop over collection facts
            for (FactCollectionFactsItem fi: factCollection.getFactsItems()) {
                Fact fact = fi.getItem().getValue();

                // Get ontology component object from fact component id
                Component component = Component.getComponentById(em, fact.getComponentId());

                // If not found - throw exception
                if (component == null) {
                    throw new IllegalArgumentException("Component id [" + fact.getComponentId() + "] is not defined in detector ontology.");
                }

                // Set component and its id
                fact.setComponent(component);
                fact.setComponentId(component.getId());
                
            }

            // Persist collection
            em.persist(factCollection);

        } catch (Exception ex) {

            ex.printStackTrace(System.err);
            throw ex;

        }
        
        return factCollection.getFacts().size();
        
    }

}
