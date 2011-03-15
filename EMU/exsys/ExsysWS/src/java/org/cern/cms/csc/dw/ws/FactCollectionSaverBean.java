
package org.cern.cms.csc.dw.ws;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;
import javax.annotation.Resource;
import javax.jms.Queue;
import javax.jms.QueueConnectionFactory;
import org.cern.cms.csc.dw.ws.exception.EmptyListReceivedException;
import java.util.HashSet;
import java.util.Set;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.xml.bind.JAXBElement;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.dw.exception.WrongComponentTypeException;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.model.fact.DcsFact;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.fact.FactCollection;
import org.cern.cms.csc.dw.service.ServiceInstructions;
import org.cern.cms.csc.dw.util.JmsWorker;

@Stateless
public class FactCollectionSaverBean implements FactCollectionSaverLocal {

    private static Logger logger = SimpleLogger.getLogger(FactCollectionSaverBean.class);


    @EJB
    private ComponentPropertyResolverLocal componentResolver;

    @EJB
    private EntityDaoLocal entityDao;

    // rule engine queue stuff
    @Resource(mappedName="jms/ruleEngineInputQueue")
    private Queue ruleEngineInputQueue;

    @Resource(mappedName="jms/ruleEngineInputQueueFactory")
    private QueueConnectionFactory ruleEngineInputQueueFactory;

    private final JmsWorker jmsWorker;

    public FactCollectionSaverBean() {
        this.jmsWorker = new JmsWorker() {

            @Override
            protected Queue getQueue() {
                return ruleEngineInputQueue;
            }

            @Override
            protected QueueConnectionFactory getQueueConnectionFactory() {
                return ruleEngineInputQueueFactory;
            }
        };
    }

    @Override
    public void saveFactCollection(FactCollection factCollection) throws Exception {
        
        logger.debug("FC Saver bean: at start, number of facts: " + factCollection.getFacts().size() + ", number of fis: " + factCollection.getFactsItems().size());

        Set<JAXBElement<? extends Fact>> toRemove = new HashSet<JAXBElement<? extends Fact>>();
        ServiceInstructions instructions = factCollection.getServiceInstructions();

        logger.debug("FC Saver bean: received fact collection with " + factCollection.getFacts().size() + " facts from " + factCollection.getSource());

        // Performing on receive operations
        factCollection.onReceive(entityDao);

        // Retrieving source gcomponent, checking its parents
        factCollection = (FactCollection) componentResolver.resolveComponentLinks(factCollection);

        // Loop over collection facts
        for (JAXBElement<? extends Fact> fi: factCollection.getFacts()) {

            // Get a fact
            Fact fact = fi.getValue();
            
            logger.debug("FC Saver bean: Processing fact: " + fact.toString());

            // Get ontology component object from fact component id or component.getId
            try {

                fact = (Fact) componentResolver.resolveComponentLinks(fact);

                if (!componentResolver.checkComponentType(
                        fact.getComponent(),
                        fact.getMetadata().getLimitComponents(),
                        fact.getMetadata().isLimitComponentsRecursive())) {
                    throw new WrongComponentTypeException(fact.getComponent(), fact.getClass(), "component");
                }

                fact.onReceive(entityDao);

                // This fact is OK so we add it to factItems
                //FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
                //fcfi.setItemValue(fact);
                //factCollection.getFactsItems().add(fcfi);

            } catch (Exception ex) {
                if (instructions.isStrict()) {
                    throw ex;
                } else {
                    if (! (fact instanceof DcsFact)) { // don't print the exception if it's a DCS fact, because it happens quite a lot and is normal due to DP addressing of unknown DPs
                        logger.error("Exception in FactCollectionSaverBean", ex);
                    }
                    toRemove.add(fi);
                }
            }

        }

        // Remove facts what need to be removed
        logger.debug("FC Saver bean: number of facts after processing but before removing: " + factCollection.getFacts().size());
        for (JAXBElement<? extends Fact> fi: toRemove) {
            factCollection.getFacts().remove(fi);
        }
        logger.debug("FC Saver bean: number of facts after processing and after removing: " + factCollection.getFacts().size());

        if (!factCollection.isSetFacts()) {
            throw new EmptyListReceivedException("factCollection", "Fact");
        }

        // Persist collection
        if (instructions.isPersist()) {
            logger.debug("FC Saver bean: serviceInstructions.isPersist() = true, so sending this fact collection to entity saver");
            entityDao.persist(factCollection);
        }

        Collection<Serializable> factsToSendToRE = new ArrayList<Serializable>();
        for (JAXBElement<? extends Fact> factEl: factCollection.getFacts()) {
            factsToSendToRE.add(factEl.getValue());
        }
        jmsWorker.sendMessages(factsToSendToRE);
        
    }

}