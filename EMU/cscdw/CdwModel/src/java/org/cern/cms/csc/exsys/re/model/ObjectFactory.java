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

    private final static QName _ConclusionSourceRelation_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionSourceRelation");
    private final static QName _ConclusionFactory_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionFactory");
    private final static QName _Conclusion_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusion");
    private final static QName _Rule_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "rule");
    private final static QName _Action_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "action");
    private final static QName _ConclusionType_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "conclusionType");
    private final static QName _ActionFactory_QNAME = new QName("http://www.cern.ch/cms/csc/exsys/re/model", "actionFactory");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.exsys.re.model
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link ConclusionSourceRelation }
     * 
     */
    public ConclusionSourceRelation createConclusionSourceRelation() {
        return new ConclusionSourceRelation();
    }

    /**
     * Create an instance of {@link ActionFactory }
     * 
     */
    public ActionFactory createActionFactory() {
        return new ActionFactory();
    }

    /**
     * Create an instance of {@link Conclusion }
     * 
     */
    public Conclusion createConclusion() {
        return new Conclusion();
    }

    /**
     * Create an instance of {@link ConclusionType }
     * 
     */
    public ConclusionType createConclusionType() {
        return new ConclusionType();
    }

    /**
     * Create an instance of {@link Action }
     * 
     */
    public Action createAction() {
        return new Action();
    }

    /**
     * Create an instance of {@link Rule }
     * 
     */
    public Rule createRule() {
        return new Rule();
    }

    /**
     * Create an instance of {@link ConclusionFactory }
     * 
     */
    public ConclusionFactory createConclusionFactory() {
        return new ConclusionFactory();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ConclusionSourceRelation }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusionSourceRelation")
    public JAXBElement<ConclusionSourceRelation> createConclusionSourceRelation(ConclusionSourceRelation value) {
        return new JAXBElement<ConclusionSourceRelation>(_ConclusionSourceRelation_QNAME, ConclusionSourceRelation.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ConclusionFactory }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "conclusionFactory")
    public JAXBElement<ConclusionFactory> createConclusionFactory(ConclusionFactory value) {
        return new JAXBElement<ConclusionFactory>(_ConclusionFactory_QNAME, ConclusionFactory.class, null, value);
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

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ActionFactory }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/exsys/re/model", name = "actionFactory")
    public JAXBElement<ActionFactory> createActionFactory(ActionFactory value) {
        return new JAXBElement<ActionFactory>(_ActionFactory_QNAME, ActionFactory.class, null, value);
    }

}
