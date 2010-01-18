/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.ws;

import javax.ejb.EJB;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import javax.jws.WebMethod;
import javax.jws.WebParam;
import javax.jws.WebService;
import javax.ejb.Stateless;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.dao.FactDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;

/**
 *
 * @author evka
 */
@WebService(serviceName="cdw", name="factcollection")
@Stateless()
public class FactCollectionInput {

    @EJB
    private OntologyDaoLocal ontologyDao;

    @EJB
    private FactDaoLocal factDao;

    /**
     * Receive and save fact collection
     * @param factCollection
     * @return
     * @throws java.lang.IllegalArgumentException
     */
    @WebMethod(operationName = "input")
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
                Component component = ontologyDao.getComponentById(fact.getComponentId());

                // If not found - throw exception
                if (component == null) {
                    throw new IllegalArgumentException("Component id [" + fact.getComponentId() + "] is not defined in detector ontology.");
                }

                // Set component and its id
                fact.setComponent(component);
                fact.setComponentId(component.getId());
                
            }

            // Persist collection
            factDao.saveFactCollection(factCollection);

        } catch (Exception ex) {

            ex.printStackTrace(System.err);
            throw ex;

        }
        
        return factCollection.getFacts().size();
        
    }

}
