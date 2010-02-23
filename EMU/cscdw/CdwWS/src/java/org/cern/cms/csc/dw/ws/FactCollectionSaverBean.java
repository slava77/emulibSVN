
package org.cern.cms.csc.dw.ws;

import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.xml.bind.JAXBElement;
import org.cern.cms.csc.dw.ComponentTypeNotAllowedInFactException;
import org.cern.cms.csc.dw.dao.FactDaoLocal;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem;
import org.cern.cms.csc.dw.model.ontology.Component;
import org.cern.cms.csc.dw.service.ServiceInstructions;

@Stateless
public class FactCollectionSaverBean implements FactCollectionSaverLocal {

    private static Logger logger = Logger.getLogger(FactCollectionSaverBean.class.getName());

    @EJB
    private OntologyDaoLocal ontologyDao;

    @EJB
    private FactDaoLocal factDao;

    public void saveFactCollection(FactCollection factCollection) throws Exception {

        Set<JAXBElement<? extends Fact>> toRemove = new HashSet<JAXBElement<? extends Fact>>();
        ServiceInstructions instructions = factCollection.getServiceInstructions();

        // Clean factCollectionFactsItems
        factCollection.getFactsItems().clear();

        // Loop over collection facts
        for (JAXBElement<? extends Fact> fi: factCollection.getFacts()) {

            // Get a fact
            Fact fact = fi.getValue();

            // Get ontology component object from fact component id
            try {

                Component component = ontologyDao.getComponentById(fact.getComponentId());
                if (!fact.checkComponentClassType(component.getComponentClass().getId())) {
                    throw new ComponentTypeNotAllowedInFactException(component.getComponentClass().getId(), fact.getType());
                }
                fact.setComponent(component);
                fact.setComponentId(component.getId());

                // This fact is OK so we add it to factItems
                FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
                fcfi.setItemValue(fact);
                factCollection.getFactsItems().add(fcfi);

            } catch (Exception ex) {
                if (instructions.isStrict()) {
                    throw ex;
                } else {
                    logger.log(Level.SEVERE, "Exception in FactCollectionSaverBean", ex);
                    toRemove.add(fi);
                }
            }

        }

        // Remove facts what need to be removed
        for (JAXBElement<? extends Fact> fi: toRemove) {
            factCollection.getFacts().remove(fi);
        }

        if (!factCollection.isSetFacts()) {
            throw new EmptyListReceivedException("factCollection", "Fact");
        }

        // Persist collection
        if (instructions.isPersist()) {
            factDao.saveFactCollection(factCollection);
        }

    }

}
