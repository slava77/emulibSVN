//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.04 at 05:42:02 PM CEST 
//


package org.cern.cms.csc.dw.model.ontology;

import javax.xml.bind.annotation.XmlRegistry;


/**
 * This object contains factory methods for each 
 * Java content interface and Java element interface 
 * generated in the org.cern.cms.csc.dw.model.ontology package. 
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


    /**
     * Create a new ObjectFactory that can be used to create new instances of schema derived classes for package: org.cern.cms.csc.dw.model.ontology
     * 
     */
    public ObjectFactory() {
    }

    /**
     * Create an instance of {@link ComponentLinkClass }
     * 
     */
    public ComponentLinkClass createComponentLinkClass() {
        return new ComponentLinkClass();
    }

    /**
     * Create an instance of {@link ComponentClass }
     * 
     */
    public ComponentClass createComponentClass() {
        return new ComponentClass();
    }

    /**
     * Create an instance of {@link ComponentSynonym }
     * 
     */
    public ComponentSynonym createComponentSynonym() {
        return new ComponentSynonym();
    }

    /**
     * Create an instance of {@link ComponentGraphLink }
     * 
     */
    public ComponentGraphLink createComponentGraphLink() {
        return new ComponentGraphLink();
    }

    /**
     * Create an instance of {@link ComponentLink }
     * 
     */
    public ComponentLink createComponentLink() {
        return new ComponentLink();
    }

    /**
     * Create an instance of {@link Component }
     * 
     */
    public Component createComponent() {
        return new Component();
    }

}
