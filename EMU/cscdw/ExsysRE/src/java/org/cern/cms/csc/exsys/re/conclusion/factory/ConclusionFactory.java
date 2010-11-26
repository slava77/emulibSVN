/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.cern.cms.csc.exsys.re.conclusion.factory;

import com.espertech.esper.client.EPServiceProvider;
import com.espertech.esper.client.EPStatement;
import com.espertech.esper.client.EventBean;
import com.espertech.esper.client.EventPropertyDescriptor;
import com.espertech.esper.client.StatementAwareUpdateListener;
import com.espertech.esper.event.bean.BeanEventBean;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.exsys.re.conclusion.ComponentResolver;
import org.cern.cms.csc.exsys.re.model.Conclusion;
import org.cern.cms.csc.exsys.re.model.ConclusionSourceRelation;
import org.cern.cms.csc.exsys.re.model.ConclusionType;
import org.cern.cms.csc.exsys.re.model.Rule;

/**
 * @author Evaldas Juska
 * This class is abstract conclusion factory which is used to create conclusions out
 * of the results of the rules.
 */
public abstract class ConclusionFactory implements StatementAwareUpdateListener {

    private static Logger logger = Logger.getLogger(ConclusionFactory.class.getName());
    private static final Pattern paramPattern = Pattern.compile("\\$(\\S+)");
    /** Conclusion type that this factory is supposed to use to create conclusions. */
    private ConclusionType conclType;
    /** Rule that this conclusion factory belongs to. */
    private Rule rule;
    /** Component resolver for the conclusion type in use by this conclusion factory. */
    private ComponentResolver componentResolver;

    /**
     * @param conclType conclusion type that this factory is supposed to use to create conclusions.
     */
    public ConclusionFactory(Rule rule) {
        this.rule = rule;
        this.conclType = rule.getConclusionType();
        this.componentResolver = new ComponentResolver(rule.getComponentFinder());
    }

    /**
     * This method is called by Esper runtime whenever a rule is triggered
     */
    public void update(EventBean[] newEvents, EventBean[] oldEvents,
            EPStatement statement, EPServiceProvider epService) {

        logger.fine("Conclusion factory: Got a trigger for statement " +
                statement.getName() +
                ": " + statement.getText());

        Conclusion concl = createConclusion(newEvents, oldEvents);

        processConclusion(concl);
    }

    /**
     * Creates a new conclusion out of data available here.
     * @param newEvents new events got in update(...)
     * @param oldEvents old events got in update(...)
     */
    protected Conclusion createConclusion(EventBean[] newEvents, EventBean[] oldEvents) {
        Conclusion concl = new Conclusion();
        concl.setType(getConclusionType());
        concl.setRule(getRule());
        concl.setTitle(substituteParams(getConclusionType().getTitle(), newEvents));
        concl.setDescription(substituteParams(getConclusionType().getDescription(), newEvents));
        concl.setSeverity(getConclusionType().getSeverity());
        concl.setTimestampItem(new Date());
        concl.setLastHitTimeItem(new Date());
        concl.setHitCount(BigInteger.ZERO);
        concl.setIsClosed(false);
        addChildrenToConclusion(concl, newEvents);
        addChildrenToConclusion(concl, oldEvents);
        concl.setComponents(getComponentResolver().getComponents(concl));
        return concl;
    }

    private void addChildrenToConclusion(Conclusion conclusion, EventBean[] events) {
        if (events == null) {
            return;
        }
        List<ConclusionSourceRelation> relations = new ArrayList<ConclusionSourceRelation>();
        Set<Object> addedChildren = new HashSet<Object>();
        for (EventBean event: events) {
            Object child = event.getUnderlying();
            if (!addedChildren.contains(child)) {
                if (child instanceof Map) {
                    Collection smallerChildWrappers = ((Map) child).values();
                    for (Object smallerChildWrapper: smallerChildWrappers) {
                        if (smallerChildWrapper instanceof EventBean) {
                            Object smallerChild = ((EventBean) smallerChildWrapper).getUnderlying();
                            if (!addedChildren.contains(smallerChild)) {
                                addedChildren.add(smallerChild);
                                addConclusionSource(conclusion, smallerChild);
                            }
                        }
                    }
                } else {
                    addedChildren.add(child);
                    addConclusionSource(conclusion, child);
                }
            }
        }
    }

    private void addConclusionSource(Conclusion conclusion, Object child) {
        if (child instanceof Fact) { // that's a fact here - add it as a child of this conclusion
            Fact childFact = (Fact) child;
            ConclusionSourceRelation relation = new ConclusionSourceRelation();
            relation.setChildFact(childFact);
            relation.setParent(conclusion);
            relation.setTimestampItem(new Date());
            relation.setIsClosing(false); // just a default
            conclusion.getChildren().add(relation);
        } else if (child instanceof Conclusion) { // that's a conclusion here - add it as a child of this conclusion
            Conclusion childConclusion = (Conclusion) child;
            ConclusionSourceRelation relation = new ConclusionSourceRelation();
            relation.setChildConclusion(childConclusion);
            relation.setParent(conclusion);
            relation.setTimestampItem(new Date());
            relation.setIsClosing(false);
            childConclusion.getParents().add(relation);
            conclusion.getChildren().add(relation);
        }
    }

    /**
     * This method is called after the conclusion has been created by createConclusion(...)
     * This method must be overriden by the subclasses of ConclusionFactory
     * @param conclusion conclusion, created by createConclusion(...)
     */
    abstract protected void processConclusion(Conclusion conclusion);

    /**
     * Substitutes all parameters (having format of $paramName) in a given string with their actual values from the event
     * @param str string where the parameter values will be substituted
     * @param event event where the parameter values should be taken from
     * @return resulting string
     */
    protected String substituteParams(String str, EventBean[] events) {
        StringBuffer ret = new StringBuffer();
        Matcher m = paramPattern.matcher(str);
        while (m.find()) {
            String param = m.group(1);
            Set<String> paramValues = new HashSet<String>();
            for (EventBean event : events) {
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
     * Get component resolver for the conclusion type in use by this conclusion factory.
     * @return component resolver for the conclusion type in use by this conclusion factory.
     */
    public ComponentResolver getComponentResolver() {
        return componentResolver;
    }

}
