//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.02 at 10:10:34 AM CET 
//


package org.cern.cms.csc.dw.service;

import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlElementDecl;
import javax.xml.bind.annotation.XmlRegistry;
import javax.xml.namespace.QName;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the org.cern.cms.csc.dw.service package. 
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

    private final static QName _ServiceInstructions_QNAME = new QName("http://www.cern.ch/cms/csc/dw/service", "serviceInstructions");

    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.dw.service
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link ServiceInstructions }
     * 
     */
    public ServiceInstructions createServiceInstructions() {
        return new ServiceInstructions();
    }

    /**
     * Create an instance of {@link JAXBElement }{@code <}{@link ServiceInstructions }{@code >}}
     * 
     */
    @XmlElementDecl(namespace = "http://www.cern.ch/cms/csc/dw/service", name = "serviceInstructions")
    public JAXBElement<ServiceInstructions> createServiceInstructions(ServiceInstructions value) {
        return new JAXBElement<ServiceInstructions>(_ServiceInstructions_QNAME, ServiceInstructions.class, null, value);
    }

}
