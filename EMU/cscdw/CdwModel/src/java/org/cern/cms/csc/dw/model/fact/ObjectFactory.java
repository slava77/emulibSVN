//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.15 at 07:35:42 PM CET 
//


package org.cern.cms.csc.dw.model.fact;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the org.cern.cms.csc.dw.model.fact package. 
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

    private final static QName _FactRequest_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "factRequest");
    private final static QName _FactCollection_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "factCollection");
    private final static QName _TmbCounterFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "tmbCounterFact");
    private final static QName _DqmCscFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmCscFact");
    private final static QName _DqmEmuFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmEmuFact");
    private final static QName _Fact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "fact");
    private final static QName _DqmCscHvSegmentFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmCscHvSegmentFact");
    private final static QName _DqmDduInputFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmDduInputFact");
    private final static QName _DqmCscCfebFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmCscCfebFact");
    private final static QName _LocalDAQStatusFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "localDAQStatusFact");
    private final static QName _CumulativeTmbTriggerCounterFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "cumulativeTmbTriggerCounterFact");
    private final static QName _ApplicationStatusFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "applicationStatusFact");
    private final static QName _DqmCscAfebFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmCscAfebFact");
    private final static QName _FactRequestCollection_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "factRequestCollection");
    private final static QName _SlidingTmbTriggerCounterFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "slidingTmbTriggerCounterFact");
    private final static QName _DqmDduFact_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model", "DqmDduFact");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.dw.model.fact
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link CumulativeTmbTriggerCounterFact }
     * 
     */
    public CumulativeTmbTriggerCounterFact createCumulativeTmbTriggerCounterFact() {
        return new CumulativeTmbTriggerCounterFact();
    }

    /**
     * Create an instance of {@link DqmDduFact }
     * 
     */
    public DqmDduFact createDqmDduFact() {
        return new DqmDduFact();
    }

    /**
     * Create an instance of {@link DqmCscHvSegmentFact }
     * 
     */
    public DqmCscHvSegmentFact createDqmCscHvSegmentFact() {
        return new DqmCscHvSegmentFact();
    }

    /**
     * Create an instance of {@link TmbCounterFact }
     * 
     */
    public TmbCounterFact createTmbCounterFact() {
        return new TmbCounterFact();
    }

    /**
     * Create an instance of {@link FactCollection }
     * 
     */
    public FactCollection createFactCollection() {
        return new FactCollection();
    }

    /**
     * Create an instance of {@link FactRequestCollection }
     * 
     */
    public FactRequestCollection createFactRequestCollection() {
        return new FactRequestCollection();
    }

    /**
     * Create an instance of {@link LocalDAQStatusFact }
     * 
     */
    public LocalDAQStatusFact createLocalDAQStatusFact() {
        return new LocalDAQStatusFact();
    }

    /**
     * Create an instance of {@link DqmCscAfebFact }
     * 
     */
    public DqmCscAfebFact createDqmCscAfebFact() {
        return new DqmCscAfebFact();
    }

    /**
     * Create an instance of {@link SlidingTmbTriggerCounterFact }
     * 
     */
    public SlidingTmbTriggerCounterFact createSlidingTmbTriggerCounterFact() {
        return new SlidingTmbTriggerCounterFact();
    }

    /**
     * Create an instance of {@link DqmCscFact }
     * 
     */
    public DqmCscFact createDqmCscFact() {
        return new DqmCscFact();
    }

    /**
     * Create an instance of {@link DqmDduInputFact }
     * 
     */
    public DqmDduInputFact createDqmDduInputFact() {
        return new DqmDduInputFact();
    }

    /**
     * Create an instance of {@link FactRequest }
     * 
     */
    public FactRequest createFactRequest() {
        return new FactRequest();
    }

    /**
     * Create an instance of {@link DqmCscCfebFact }
     * 
     */
    public DqmCscCfebFact createDqmCscCfebFact() {
        return new DqmCscCfebFact();
    }

    /**
     * Create an instance of {@link ApplicationStatusFact }
     * 
     */
    public ApplicationStatusFact createApplicationStatusFact() {
        return new ApplicationStatusFact();
    }

    /**
     * Create an instance of {@link DqmEmuFact }
     * 
     */
    public DqmEmuFact createDqmEmuFact() {
        return new DqmEmuFact();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link FactRequest }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "factRequest")
    public JAXBElement<FactRequest> createFactRequest(FactRequest value) {
        return new JAXBElement<FactRequest>(_FactRequest_QNAME, FactRequest.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link FactCollection }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "factCollection")
    public JAXBElement<FactCollection> createFactCollection(FactCollection value) {
        return new JAXBElement<FactCollection>(_FactCollection_QNAME, FactCollection.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link TmbCounterFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "tmbCounterFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<TmbCounterFact> createTmbCounterFact(TmbCounterFact value) {
        return new JAXBElement<TmbCounterFact>(_TmbCounterFact_QNAME, TmbCounterFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmCscFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmCscFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmCscFact> createDqmCscFact(DqmCscFact value) {
        return new JAXBElement<DqmCscFact>(_DqmCscFact_QNAME, DqmCscFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmEmuFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmEmuFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmEmuFact> createDqmEmuFact(DqmEmuFact value) {
        return new JAXBElement<DqmEmuFact>(_DqmEmuFact_QNAME, DqmEmuFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link Fact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "fact")
    public JAXBElement<Fact> createFact(Fact value) {
        return new JAXBElement<Fact>(_Fact_QNAME, Fact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmCscHvSegmentFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmCscHvSegmentFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmCscHvSegmentFact> createDqmCscHvSegmentFact(DqmCscHvSegmentFact value) {
        return new JAXBElement<DqmCscHvSegmentFact>(_DqmCscHvSegmentFact_QNAME, DqmCscHvSegmentFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmDduInputFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmDduInputFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmDduInputFact> createDqmDduInputFact(DqmDduInputFact value) {
        return new JAXBElement<DqmDduInputFact>(_DqmDduInputFact_QNAME, DqmDduInputFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmCscCfebFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmCscCfebFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmCscCfebFact> createDqmCscCfebFact(DqmCscCfebFact value) {
        return new JAXBElement<DqmCscCfebFact>(_DqmCscCfebFact_QNAME, DqmCscCfebFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link LocalDAQStatusFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "localDAQStatusFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<LocalDAQStatusFact> createLocalDAQStatusFact(LocalDAQStatusFact value) {
        return new JAXBElement<LocalDAQStatusFact>(_LocalDAQStatusFact_QNAME, LocalDAQStatusFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link CumulativeTmbTriggerCounterFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "cumulativeTmbTriggerCounterFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<CumulativeTmbTriggerCounterFact> createCumulativeTmbTriggerCounterFact(CumulativeTmbTriggerCounterFact value) {
        return new JAXBElement<CumulativeTmbTriggerCounterFact>(_CumulativeTmbTriggerCounterFact_QNAME, CumulativeTmbTriggerCounterFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ApplicationStatusFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "applicationStatusFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<ApplicationStatusFact> createApplicationStatusFact(ApplicationStatusFact value) {
        return new JAXBElement<ApplicationStatusFact>(_ApplicationStatusFact_QNAME, ApplicationStatusFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmCscAfebFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmCscAfebFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmCscAfebFact> createDqmCscAfebFact(DqmCscAfebFact value) {
        return new JAXBElement<DqmCscAfebFact>(_DqmCscAfebFact_QNAME, DqmCscAfebFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link FactRequestCollection }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "factRequestCollection")
    public JAXBElement<FactRequestCollection> createFactRequestCollection(FactRequestCollection value) {
        return new JAXBElement<FactRequestCollection>(_FactRequestCollection_QNAME, FactRequestCollection.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link SlidingTmbTriggerCounterFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "slidingTmbTriggerCounterFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<SlidingTmbTriggerCounterFact> createSlidingTmbTriggerCounterFact(SlidingTmbTriggerCounterFact value) {
        return new JAXBElement<SlidingTmbTriggerCounterFact>(_SlidingTmbTriggerCounterFact_QNAME, SlidingTmbTriggerCounterFact.class, null, value);
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link DqmDduFact }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model", name = "DqmDduFact", substitutionHeadNamespace = "http://www.cern.ch/cms/csc/dw/model", substitutionHeadName = "fact")
    public JAXBElement<DqmDduFact> createDqmDduFact(DqmDduFact value) {
        return new JAXBElement<DqmDduFact>(_DqmDduFact_QNAME, DqmDduFact.class, null, value);
    }

}
