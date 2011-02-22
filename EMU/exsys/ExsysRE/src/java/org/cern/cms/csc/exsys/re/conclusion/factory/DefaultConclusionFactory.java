/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.conclusion.factory;

import java.io.Serializable;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.logging.Level;
import javax.jms.Connection;
import javax.jms.ConnectionFactory;
import javax.jms.Destination;
import javax.jms.JMSException;
import javax.jms.Message;
import javax.jms.MessageProducer;
import javax.jms.ObjectMessage;
import javax.jms.Session;
import javax.naming.Context;
import javax.naming.InitialContext;
import javax.naming.NamingException;
import jsf.bean.gui.log.Logger;
import jsf.bean.gui.log.SimpleLogger;
import org.cern.cms.csc.dw.dao.EntityDaoLocal;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionCacheServiceLocal;
import org.cern.cms.csc.exsys.re.dao.RuleEngineDaoLocal;
import org.cern.cms.csc.exsys.re.model.Action;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTrigger;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerType;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 *
 * @author Evka
 */
public class DefaultConclusionFactory extends ConclusionFactory {

    private static Logger logger = SimpleLogger.getLogger(ConclusionFactory.class);

    private RuleEngineDaoLocal reDao;
    private ConclusionCacheServiceLocal conclusionCacheService;

    public DefaultConclusionFactory(RuleEngineManagerLocal reManager, Rule rule, ConclusionCacheServiceLocal conclusionCacheService) {
        super(reManager, rule);
        this.reDao = reManager.getRuleEngineDao();
        this.conclusionCacheService = conclusionCacheService;
    }

    @Override
    protected synchronized Conclusion processConclusion(Conclusion conclusion) {
        try {
            // check if it already exists in cache
            Conclusion existingConclusion = getConclusionCacheService().checkCache(conclusion);
            if (existingConclusion == null) {
                if (conclusion.isClosed()) {
                    return null;
                }

                logger.info("Default conclusion factory: Got new conclusion: " + conclusion.getTitle());
                logger.info("Saving new conclusion: " + conclusion);
                EntityDaoLocal entityDao = reDao.getEntityDao();
                entityDao.persist(conclusion);
                entityDao.flush();
                getConclusionCacheService().addToCache(conclusion);
                executeActions(getConclusionType().getActions(), conclusion.getTriggers());
                return conclusion;
            } else {
                logger.debug("Default conclusion factory: Got a duplicate conclusion (updating existing one): " +
                        existingConclusion.getTitle() + " (id=" + existingConclusion.getid() + ")");

                //existingConclusion = (Conclusion) reDao.getEntityDao().refreshEntity(existingConclusion, false);
                updateExistingConclusion(existingConclusion, conclusion);
                logger.trace("Existing conclusion ID before merge: " + existingConclusion.getid());
                EntityDaoLocal entityDao = reDao.getEntityDao();
                existingConclusion = (Conclusion) entityDao.merge(existingConclusion);
                entityDao.flush();
                logger.trace("Existing conclusion ID after merge: " + existingConclusion.getid());
                if (!existingConclusion.isClosed()) {
                    logger.debug("Saving existing conclusion: " + existingConclusion);
                    getConclusionCacheService().addToCache(existingConclusion); // update the conclusion in the cache
                } else {
                    logger.info("Closing conclusion: " + existingConclusion);
                    getConclusionCacheService().removeFromCache(existingConclusion); // remove the conclusion from the cache
                    return existingConclusion;
                }
                return null; // don't throw the conclusion back into esper if it's just an update
            }
        } catch (Throwable pex) {
            logger.error("Exception while saving a conclusion", pex);
            return null;
        }
    }

    protected void updateExistingConclusion(Conclusion existingConclusion, Conclusion conclusion) throws Exception {
        existingConclusion.setHitCount(existingConclusion.getHitCount().add(BigInteger.ONE));
        existingConclusion.setLastHitTimeItem(conclusion.getTimestampItem());
        existingConclusion.setTitle(conclusion.getTitle()); // update with more up to date title
        existingConclusion.setDescription(conclusion.getDescription()); // update with more up to date description
        existingConclusion.setClosed(conclusion.isClosed());

        // add new triggers
        for (ConclusionTrigger trigger: conclusion.getTriggers()) {
            // check for loops - remove any sources that are pointing back to the same conclusion
            List<ConclusionTriggerSource> sourcesToRemove = new ArrayList<ConclusionTriggerSource>();
            for (ConclusionTriggerSource source: trigger.getSources()) {
                if (existingConclusion.equals(source.getConclusion())) {
                    sourcesToRemove.add(source);
                }
            }
            trigger.getSources().removeAll(sourcesToRemove);

            // if the trigger still has sources left - update and save it
            if (trigger.getSources().size() > 0) {
                trigger.setConclusion(existingConclusion);
                if (trigger.getType().equals(ConclusionTriggerType.OPEN)) { // don't change "CLOSE" to "UPDATE"
                    trigger.setType(ConclusionTriggerType.UPDATE);
                }
                reDao.getEntityDao().persist(trigger);
                executeActions(getConclusionType().getActions(), trigger);
            }
//            existingConclusion.getTriggers().add(trigger);
        }
    }

    protected ConclusionCacheServiceLocal getConclusionCacheService() {
        return conclusionCacheService;
    }

    protected void executeAction(Action action, ConclusionTrigger trigger) {
        try {
            logger.info("action execution disabled for now - should have executed action " + action.getName());
//            ActionExecution exec = new ActionExecution();
//            exec.setAction(action);
//            exec.setTrigger(trigger);
//            exec.setTimestampItem(new Date());
//            sendJMSMessageToRuleEngineActionQueue(exec);
        } catch (Exception ex) {
            logger.error("Exception while sending actions to the ruleEngineActionQueue for execution", ex);
        }
    }

    protected void executeActions(Collection<Action> actions, ConclusionTrigger trigger) {
        for (Action action: actions) {
            executeAction(action, trigger);
        }
    }

    protected void executeActions(Collection<Action> actions, Collection<ConclusionTrigger> triggers) {
        for (ConclusionTrigger trigger: triggers) {
            executeActions(actions, trigger);
        }
    }

    private Message createJMSMessageForjmsRuleEngineActionQueue(Session session, Serializable messageData) throws JMSException {
        // TODO create and populate message to send
        ObjectMessage om = session.createObjectMessage();
        om.setObject(messageData);
        return om;
    }

    private void sendJMSMessageToRuleEngineActionQueue(Serializable messageData) throws NamingException, JMSException {
        Context c = new InitialContext();
        ConnectionFactory cf = (ConnectionFactory) c.lookup("java:comp/env/jms/ruleEngineActionQueueFactory");
        Connection conn = null;
        Session s = null;
        try {
            conn = cf.createConnection();
            s = conn.createSession(false, s.AUTO_ACKNOWLEDGE);
            Destination destination = (Destination) c.lookup("java:comp/env/jms/ruleEngineActionQueue");
            MessageProducer mp = s.createProducer(destination);
            mp.send(createJMSMessageForjmsRuleEngineActionQueue(s, messageData));
        } finally {
            if (s != null) {
                try {
                    s.close();
                } catch (JMSException e) {
                    java.util.logging.Logger.getLogger(this.getClass().getName()).log(Level.WARNING, "Cannot close session", e);
                }
            }
            if (conn != null) {
                conn.close();
            }
        }
    }

}
