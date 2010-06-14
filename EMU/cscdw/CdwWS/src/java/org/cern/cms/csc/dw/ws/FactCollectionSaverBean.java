
package org.cern.cms.csc.dw.ws;

import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.xml.bind.JAXBElement;
import org.apache.commons.lang.NullArgumentException;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.ComponentTypeNotAllowedInFactException;
import org.cern.cms.csc.dw.dao.OntologyDaoLocal;
import org.cern.cms.csc.dw.dao.PersistDaoLocal;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.model.ontology.graph.GComponent;
import org.cern.cms.csc.dw.service.ServiceInstructions;

@Stateless
public class FactCollectionSaverBean implements FactCollectionSaverLocal {

    private static Logger logger = Logger.getLogger(FactCollectionSaverBean.class.getName());

    @EJB
    private PersistDaoLocal persistDao;

    @EJB
    private OntologyDaoLocal ontologyDao;

    @EJB
    private EntityDaoLocal entityDao;

    public void saveFactCollection(FactCollection factCollection) throws Exception {
        logger.finest("FC Saver bean: at start, number of facts: " + factCollection.getFacts().size() + ", number of fis: " + factCollection.getFactsItems().size());

        Set<JAXBElement<? extends Fact>> toRemove = new HashSet<JAXBElement<? extends Fact>>();
        ServiceInstructions instructions = factCollection.getServiceInstructions();

        logger.info("FC Saver bean: received: " + factCollection.toString());

        // Performing on receive operations
        factCollection.onReceive(entityDao);

        // Loop over collection facts
        for (JAXBElement<? extends Fact> fi: factCollection.getFacts()) {

            // Get a fact
            Fact fact = fi.getValue();
            
            logger.info("FC Saver bean: Processing fact: " + fact.toString());

            // Get ontology component object from fact component id or component.getId
            try {

                GComponent gcomponent = null;
                if (fact.getComponent() != null) {
                    gcomponent = ontologyDao.getGComponentById(fact.getComponent().getId());
                } else if (fact.getComponentId() != null) {
                    gcomponent = ontologyDao.getGComponentByName(fact.getComponentId());
                }

                if (gcomponent == null) {
                    throw new NullArgumentException("Valid component id and/or component is not provided for Fact!");
                }
                logger.info("FC Saver bean: found gcomponent: " + gcomponent.getName());

                if (!ontologyDao.isGComponentClassParent(
                        fact.getMetadata().getLimitComponents(),
                        gcomponent.getType(),
                        fact.getMetadata().isLimitComponentsRecursive())) {
                    throw new ComponentTypeNotAllowedInFactException(gcomponent.getType().getType(), fact.getClass());
                }

                fact.setComponent(ontologyDao.getComponentById(gcomponent.getId()));
                fact.setComponentId(gcomponent.getName());

                fact.onReceive(entityDao);

                // This fact is OK so we add it to factItems
                //FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
                //fcfi.setItemValue(fact);
                //factCollection.getFactsItems().add(fcfi);

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
        logger.finest("FC Saver bean: number of facts after processing but before removing: " + factCollection.getFacts().size());
        for (JAXBElement<? extends Fact> fi: toRemove) {
            factCollection.getFacts().remove(fi);
        }
        logger.finest("FC Saver bean: number of facts after processing and after removing: " + factCollection.getFacts().size());

        if (!factCollection.isSetFacts()) {
            throw new EmptyListReceivedException("factCollection", "Fact");
        }

        // Persist collection
        if (instructions.isPersist()) {
            logger.finest("FC Saver bean: serviceInstructions.isPersist() = true, so sending this fact collection to entity saver");
            persistDao.persist(factCollection);
        }

    }

}
