/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.cern.cms.csc.exsys.re.conclusion.factory;

import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import com.espertech.esper.client.EventBean;
import com.espertech.esper.client.StatementAwareUpdateListener;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import jsf.bean.gui.log.Logger;
import org.cern.cms.csc.dw.log.ExsysLogger;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.exsys.re.RuleEngineManagerLocal;
import org.cern.cms.csc.exsys.re.conclusion.ConclusionComponentResolver;
import org.cern.cms.csc.exsys.re.exception.ComponentResolverException;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionTrigger;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource;
import org.cern.cms.csc.exsys.re.model.ConclusionTriggerType;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;
import org.cern.cms.csc.exsys.re.model.RuleType;

/**
 * @author Evaldas Juska
 * This class is abstract conclusion factory which is used to create conclusions out
 * of the results of the rules.
 */
public abstract class ConclusionFactory implements StatementAwareUpdateListener {

    private static Logger logger = ExsysLogger.getLogger(ConclusionFactory.class);
    private static final Pattern paramPattern = Pattern.compile("\\$(\\S+)");
    /** Conclusion type that this factory is supposed to use to create conclusions. */
    private ConclusionType conclType;
    /** Rule that this conclusion factory belongs to. */
    private Rule rule;
    /** Component resolver for the conclusion type in use by this conclusion factory. */
    private ConclusionComponentResolver componentResolver;
    /** Rule engine manager - used to throw conclusions back into rule engine. */
    RuleEngineManagerLocal reManager;
    
    /**
     * @param conclType conclusion type that this factory is supposed to use to create conclusions.
     * @param reManager Rule engine manager - used to throw conclusions back into rule engine.
     */
    public ConclusionFactory(RuleEngineManagerLocal reManager, Rule rule) {
        this.reManager = reManager;
        this.rule = rule;
        this.conclType = rule.getConclusionType();
        this.componentResolver = new ConclusionComponentResolver(rule.getComponentFinder());
    }

    /**
     * This method is called by Esper runtime whenever a rule is triggered
     */
    @Override
    public void update(EventBean[] newEvents, EventBean[] oldEvents,
            EPStatement statement, EPServiceProvider epService) {

        try {
            logger.debug("Conclusion factory: Got a trigger for statement \"{0}\"", statement.getName());
            if (logger.isTraceEnabled()) {
                if (newEvents != null) {
                    for (EventBean ev: newEvents) {
                        logger.trace("    Conclusion factory: new event: {0}", ev);
                    }
                }
                if (oldEvents != null) {
                    for (EventBean ev: oldEvents) {
                        logger.trace("    Conclusion factory: old event: {0}", ev);
                    }
                }
            }

            Conclusion concl = createConclusion(newEvents, oldEvents);

            concl = processConclusion(concl);
            if (concl != null) {
                reManager.postEventFromListener(concl);
            }
        } catch (Throwable ex) {
            logger.error("ConclusionFactory: error while constructing a conclusion for rule \"" + getRule().getName() + "\"", ex);
        }
    }

    /**
     * Creates a new conclusion out of data available here.
     * @param newEvents new events got in update(...)
     * @param oldEvents old events got in update(...)
     */
    protected Conclusion createConclusion(EventBean[] newEvents, EventBean[] oldEvents) throws ComponentResolverException {
        Collection<EntityBase> unpackedEventEntities = unpackEntitiesFromEvents(newEvents);
        //unpackedEventEntities.addAll(unpackEntitiesFromEvents(oldEvents));
        Collection<EventBean> unpackedNewEvents = unpackEvents(newEvents);

        Conclusion concl = new Conclusion();
        concl.setType(getConclusionType());
        concl.setTitle(substituteParams(getConclusionType().getTitle(), unpackedNewEvents));
        concl.setDescription(substituteParams(getConclusionType().getDescription(), unpackedNewEvents));
        concl.setSeverity(getConclusionType().getSeverity());
        concl.setTimestampItem(new Date());
        concl.setLastHitTimeItem(new Date());
        concl.setHitCount(BigInteger.ONE);
        concl.setClosed(isRuleClosing());
        if (concl.isClosed()) {
            concl.setTimeClosedItem(new Date());
        }
        concl.setComponent(getComponentResolver().getComponent(unpackedEventEntities));
        addTriggerToConclusion(concl, unpackedEventEntities);
        return concl;
    }

    private void addTriggerToConclusion(Conclusion conclusion, Collection<EntityBase> triggerSources) {
        ConclusionTrigger trigger = new ConclusionTrigger();
        if (isRuleClosing()) {
            trigger.setType(ConclusionTriggerType.CLOSE);
        } else {
            trigger.setType(ConclusionTriggerType.OPEN);
        }
        trigger.setConclusion(conclusion);
        trigger.setRule(getRule());
        trigger.setTimestampItem(new Date());

        for (EntityBase triggerSource: triggerSources) {
            if (triggerSource instanceof Fact) { // that's a fact here - add it as a child of this conclusion
                Fact fact = (Fact) triggerSource;
                if (logger.isDebugEnabled()) {
                    logger.debug("    Conclusion factory: trigger source (fact): {0}", fact);
                }
                ConclusionTriggerSource source = new ConclusionTriggerSource();
                source.setFact(fact);
                source.setTrigger(trigger);
                trigger.getSources().add(source);
            } else if (triggerSource instanceof Conclusion) { // that's a conclusion here - add it as a child of this conclusion
                Conclusion sourceConclusion = (Conclusion) triggerSource;
                if (logger.isDebugEnabled()) {
                    logger.debug("    Conclusion factory: trigger source (conclusion): {0}", sourceConclusion.debugPrint(false));
                }
                //childConclusion = (Conclusion) reManager.getRuleEngineDao().getEntityDao().refreshEntity(childConclusion);
                ConclusionTriggerSource source = new ConclusionTriggerSource();
                source.setConclusion(sourceConclusion);
                source.setTrigger(trigger);
                trigger.getSources().add(source);
                //sourceConclusion.getParents().add(source);
            }
        }

        conclusion.getTriggers().add(trigger);
    }

    /**
     * Recursive function which checks if the provided event is a simple one or
     * a composite one (contains a Map). In simple event case, it just returns
     * the same thing, in composite event case - it returns any events wrapped
     * inside it.
     * @param event event that you want to unpack.
     * @param unpackedEvents events, unpacked from the given event.
     */
    protected void unpackEvent(EventBean event, Collection<EventBean> unpackedEvents) {
        Object underlying = event.getUnderlying();
        if (underlying instanceof Map) { // a composite event
            Collection wrappedUnderlyingObjs = ((Map) underlying).values();
            boolean hasEntities = false;
            for (Object wrappedUnderlyingObj: wrappedUnderlyingObjs) {
                if (wrappedUnderlyingObj instanceof EventBean) {
                    unpackEvent((EventBean) wrappedUnderlyingObj, unpackedEvents);
                } else if (wrappedUnderlyingObj instanceof EventBean[]) {
                    for (EventBean e: (EventBean[]) wrappedUnderlyingObj) {
                        unpackEvent(e, unpackedEvents);
                    }
                } else if (wrappedUnderlyingObj instanceof EntityBase) {
                    hasEntities = true;
                }
            }
            if (hasEntities) { // if the map contains entities, also add the whole event as unpacked
                unpackedEvents.add(event);
            }
        } else { // not a composite event
            unpackedEvents.add(event);
        }
    }

    /**
     * Goes through all provided events and calls unpackEvent(...) on them.
     * @see unpackEvent(EventBean event, Collection<EventBean> unpackedEvents)
     */
    protected Collection<EventBean> unpackEvents(EventBean[] events) {
        Collection<EventBean> ret = new HashSet<EventBean>();
        if (events == null) {
            return ret;
        }
        for (EventBean event: events) {
            unpackEvent(event, ret);
        }
        return ret;
    }

    /** Recursive function which checks the event.underlying() and if it's an entity - returns it,
     *  if however that's a Map (which you get with join rules), then goes inside it, gets the wrapped
     *  events and calls the same method on all of them.
     * @param event event that you want to get the entities out from.
     * @param unpackedEntities any entities wrapped inside the given event are placed here - it's not in return because it's a recursive function and it's just more efficient not to create new collections all the time.
     */
    protected void unpackEntitiesFromEvent(EventBean event, Collection<EntityBase> unpackedEntities) {
        Object underlying = event.getUnderlying();
        if (underlying instanceof EntityBase) {
            unpackedEntities.add((EntityBase) underlying);
        } else if (underlying instanceof EntityBase[]) {
            unpackedEntities.addAll(Arrays.asList((EntityBase[]) underlying));
        }
        if (underlying instanceof Map) {
            Collection wrappedUnderlyingObjs = ((Map) underlying).values();
            for (Object wrappedUnderlyingObj: wrappedUnderlyingObjs) {
                if (wrappedUnderlyingObj instanceof EventBean) {
                    unpackEntitiesFromEvent((EventBean) wrappedUnderlyingObj, unpackedEntities);
                } else if (wrappedUnderlyingObj instanceof EntityBase) {
                    unpackedEntities.add((EntityBase) wrappedUnderlyingObj);
                } else if (wrappedUnderlyingObj instanceof EntityBase[]) {
                    unpackedEntities.addAll(Arrays.asList((EntityBase[]) wrappedUnderlyingObj));
                }
            }
        }
    }

    /**
     * Goes through all provided events and calls unpackEntitiesFromEvent(...) on them,
     * which unpacks any entities, hidden in those events.
     * @param events events that you want to unpack all entities from
     * @return a collection of entities unpacked from the given events
     */
    protected Collection<EntityBase> unpackEntitiesFromEvents(EventBean[] events) {
        if (events == null) {
            return Collections.EMPTY_SET;
        }
        Collection<EntityBase> ret = new HashSet<EntityBase>();
        for (EventBean event: events) {
            unpackEntitiesFromEvent(event, ret);
        }
        return ret;
    }

    /**
     * This method is called after the conclusion has been created by createConclusion(...)
     * This method must be overriden by the subclasses of ConclusionFactory
     * @param conclusion conclusion, created by createConclusion(...)
     * @return processed conclusion to be thrown back into the rules engine, if null is returned, nothing is thrown to rules engine
     */
    abstract protected Conclusion processConclusion(Conclusion conclusion);

    /**
     * Substitutes all parameters (having format of $paramName) in a given string with their actual values from the event
     * @param str string where the parameter values will be substituted
     * @param event event where the parameter values should be taken from
     * @return resulting string
     */
    protected String substituteParams(String str, Collection<EventBean> events) {
        StringBuffer ret = new StringBuffer();
        Matcher m = paramPattern.matcher(str);
        while (m.find()) {
            String param = m.group(1);
            Set<String> paramValues = new HashSet<String>();
            for (EventBean event : events) {
                if (!event.getEventType().isProperty(param)) {
                    continue;
                }
                if (event.get(param) != null) {
                    Object paramValue = event.get(param);
                    if (paramValue.getClass().isArray()) {
                        Object[] paramValueArray = (Object[]) paramValue;

                        StringBuilder paramValueStr = new StringBuilder();
                        paramValueStr.append('[');
                        boolean first = true;
                        for (Object pv : paramValueArray) {
                            if (!first) {
                                paramValueStr.append(", ");
                            }
                            paramValueStr.append(paramValueToString(pv));
                            first = false;
                        }
                        paramValueStr.append(']');
                        paramValues.add(paramValueStr.toString());

                    } else {
                        paramValues.add(paramValueToString(event.get(param)));
                    }
                } else {
                    paramValues.add("NULL");
                }
            }
            if (paramValues.isEmpty()) {
                paramValues.add("N/A");
            }
            StringBuilder paramValueStr = new StringBuilder();
            boolean first = true;
            for (String paramValue : paramValues) {
                if (!first) {
                    paramValueStr.append(", ");
                }
                paramValueStr.append(paramValue);
                first = false;
            }
            m.appendReplacement(ret, paramValueStr.toString());
        }
        m.appendTail(ret);
        return ret.toString();
    }

    /**
     * Get a string representation of an event parameter value.
     * Basically checks if this object by any chance is an EntityBase,
     * if so, then calls getEntityTitle() on it, if not then returns toString()
     */
    protected String paramValueToString(Object paramValue) {
        if (paramValue instanceof EntityBase) {
            try {
                return ((EntityBase) paramValue).getEntityTitle();
            } catch (Exception ex) {
                return paramValue.toString();
            }
        }
        return paramValue.toString();
    }

    /**
     * Get conclusion type that this factory is supposed to use to create conclusions.
     * @return conclusion type that this factory is supposed to use to create conclusions.
     */
    public ConclusionType getConclusionType() {
        return conclType;
    }

    /**
     * Get rule that this conclusion factory belongs to.
     * @return rule that this conclusion factory belongs to.
     */
    public Rule getRule() {
        return rule;
    }

    /**
     * Get a flag telling if the rule, associated to this conclusion factory is of closing type or not.
     * @return a flag telling if the rule, associated to this conclusion factory is of closing type or not.
     */
    public boolean isRuleClosing() {
        return rule.getType().equals(RuleType.CLOSING);
    }

    /**
     * Get component resolver for the conclusion type in use by this conclusion factory.
     * @return component resolver for the conclusion type in use by this conclusion factory.
     */
    public ConclusionComponentResolver getComponentResolver() {
        return componentResolver;
    }

}
