//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.04 at 07:39:31 PM CET 
//


package org.cern.cms.csc.dw.model.dse;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the org.cern.cms.csc.dw.model.dse package. 
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

    private final static QName _TmbCounterDescriptionDSE_QNAME = new QName("http://www.cern.ch/cms/csc/dw/model/dse", "tmbCounterDescriptionDSE");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.dw.model.dse
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link DataServiceEntity }
     * 
     */
    public DataServiceEntity createDataServiceEntity() {
        return new DataServiceEntity();
    }

    /**
     * Create an instance of {@link TmbCounterDescriptionDSE }
     * 
     */
    public TmbCounterDescriptionDSE createTmbCounterDescriptionDSE() {
        return new TmbCounterDescriptionDSE();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link TmbCounterDescriptionDSE }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/model/dse", name = "tmbCounterDescriptionDSE")
    public JAXBElement<TmbCounterDescriptionDSE> createTmbCounterDescriptionDSE(TmbCounterDescriptionDSE value) {
        return new JAXBElement<TmbCounterDescriptionDSE>(_TmbCounterDescriptionDSE_QNAME, TmbCounterDescriptionDSE.class, null, value);
    }

}
