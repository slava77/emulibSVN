//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the org.cern.cms.csc.exsys.re.model package. 
 * <p>An ObjectFactory allows you to programatically 
 * construct new instances of the Java representation 
 * for XML content. The Java representation of XML 
 * content can consist of schema derived interfaces 
 * and classes representing the binding of schema 
 * type definitions, element declarations and model 
 * groups.  Factory methods for each of these are 
 * provided in this class.
 * 
 */
@XmlRegistry
public class ObjectFactory {

    private final static QName _ConclusionTrigger_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionTrigger");
    private final static QName _ConclusionTriggerSource_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionTriggerSource");
    private final static QName _Conclusion_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusion");
    private final static QName _RuleSet_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "ruleSet");
    private final static QName _ComponentFinder_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "componentFinder");
    private final static QName _ActionExecution_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "actionExecution");
    private final static QName _Rule_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "rule");
    private final static QName _Action_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "action");
    private final static QName _ConclusionType_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionType");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.exsys.re.model
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link SimpleComponentFinder }
     * 
     */
    public SimpleComponentFinder createSimpleComponentFinder() {
        return new SimpleComponentFinder();
    }

    /**
     * Create an instance of {@link ConclusionType }
     * 
     */
    public ConclusionType createConclusionType() {
        return new ConclusionType();
    }

    /**
     * Create an instance of {@link RuleSet }
     * 
     */
    public RuleSet createRuleSet() {
        return new RuleSet();
    }

    /**
     * Create an instance of {@link SmsAction }
     * 
     */
    public SmsAction createSmsAction() {
        return new SmsAction();
    }

    /**
     * Create an instance of {@link ActionExecution }
     * 
     */
    public ActionExecution createActionExecution() {
        return new ActionExecution();
    }

    /**
     * Create an instance of {@link Rule }
     * 
     */
    public Rule createRule() {
        return new Rule();
    }

    /**
     * Create an instance of {@link DcsCommandAction }
     * 
     */
    public DcsCommandAction createDcsCommandAction() {
        return new DcsCommandAction();
    }

    /**
     * Create an instance of {@link ConclusionTrigger }
     * 
     */
    public ConclusionTrigger createConclusionTrigger() {
        return new ConclusionTrigger();
    }

    /**
     * Create an instance of {@link RelatedComponentFinder }
     * 
     */
    public RelatedComponentFinder createRelatedComponentFinder() {
        return new RelatedComponentFinder();
    }

    /**
     * Create an instance of {@link EmailAction }
     * 
     */
    public EmailAction createEmailAction() {
        return new EmailAction();
    }

    /**
     * Create an instance of {@link Conclusion }
     * 
     */
    public Conclusion createConclusion() {
        return new Conclusion();
    }

    /**
     * Create an instance of {@link ConclusionTriggerSource }
     * 
     */
    public ConclusionTriggerSource createConclusionTriggerSource() {
        return new ConclusionTriggerSource();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ConclusionTrigger }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusionTrigger")
    public JAXBElement<ConclusionTrigger> createConclusionTrigger(ConclusionTrigger value) {
        return new JAXBElement<ConclusionTrigger>(_ConclusionTrigger_QNAME, ConclusionTrigger.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ConclusionTriggerSource }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusionTriggerSource")
    public JAXBElement<ConclusionTriggerSource> createConclusionTriggerSource(ConclusionTriggerSource value) {
        return new JAXBElement<ConclusionTriggerSource>(_ConclusionTriggerSource_QNAME, ConclusionTriggerSource.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Conclusion }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusion")
    public JAXBElement<Conclusion> createConclusion(Conclusion value) {
        return new JAXBElement<Conclusion>(_Conclusion_QNAME, Conclusion.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link RuleSet }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "ruleSet")
    public JAXBElement<RuleSet> createRuleSet(RuleSet value) {
        return new JAXBElement<RuleSet>(_RuleSet_QNAME, RuleSet.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ComponentFinder }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "componentFinder")
    public JAXBElement<ComponentFinder> createComponentFinder(ComponentFinder value) {
        return new JAXBElement<ComponentFinder>(_ComponentFinder_QNAME, ComponentFinder.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ActionExecution }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "actionExecution")
    public JAXBElement<ActionExecution> createActionExecution(ActionExecution value) {
        return new JAXBElement<ActionExecution>(_ActionExecution_QNAME, ActionExecution.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Rule }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "rule")
    public JAXBElement<Rule> createRule(Rule value) {
        return new JAXBElement<Rule>(_Rule_QNAME, Rule.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Action }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "action")
    public JAXBElement<Action> createAction(Action value) {
        return new JAXBElement<Action>(_Action_QNAME, Action.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ConclusionType }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusionType")
    public JAXBElement<ConclusionType> createConclusionType(ConclusionType value) {
        return new JAXBElement<ConclusionType>(_ConclusionType_QNAME, ConclusionType.class, null, value);
    }

}
