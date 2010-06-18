//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.service;

import java.io.Serializable;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for serviceInstructionsType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="serviceInstructionsType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;attribute name="async" type="{http://www.w3.org/2001/XMLSchema}boolean" default="true" />
 *       &lt;attribute name="strict" type="{http://www.w3.org/2001/XMLSchema}boolean" default="false" />
 *       &lt;attribute name="persist" type="{http://www.w3.org/2001/XMLSchema}boolean" default="true" />
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "serviceInstructionsType")
public class ServiceInstructions
    implements Serializable
{

    @XmlAttribute(name = "async")
    protected Boolean async;
    @XmlAttribute(name = "strict")
    protected Boolean strict;
    @XmlAttribute(name = "persist")
    protected Boolean persist;

    /**
     * 
     * Process the message asynchronous if true and synchonous otherwise. 
     *                         
     * 
     * @return
     *     possible object is
     *     {@link Boolean }
     *     
     */
    public boolean isAsync() {
        if (async == null) {
            return true;
        } else {
            return async;
        }
    }

    /**
     * Sets the value of the async property.
     * 
     * @param value
     *     allowed object is
     *     {@link Boolean }
     *     
     */
    public void setAsync(boolean value) {
        this.async = value;
    }

    public boolean isSetAsync() {
        return (this.async!= null);
    }

    public void unsetAsync() {
        this.async = null;
    }

    /**
     * 
     * Reject the whole message if at least one of its parts fails if true and process what is and if possible otherwise. Switch to true for debugging purposes.
     *                         
     * 
     * @return
     *     possible object is
     *     {@link Boolean }
     *     
     */
    public boolean isStrict() {
        if (strict == null) {
            return false;
        } else {
            return strict;
        }
    }

    /**
     * Sets the value of the strict property.
     * 
     * @param value
     *     allowed object is
     *     {@link Boolean }
     *     
     */
    public void setStrict(boolean value) {
        this.strict = value;
    }

    public boolean isSetStrict() {
        return (this.strict!= null);
    }

    public void unsetStrict() {
        this.strict = null;
    }

    /**
     * 
     * Persist (save) message if true and do not otherwise. Should be used for debugging purposes together with strict = true and async = false.
     *                         
     * 
     * @return
     *     possible object is
     *     {@link Boolean }
     *     
     */
    public boolean isPersist() {
        if (persist == null) {
            return true;
        } else {
            return persist;
        }
    }

    /**
     * Sets the value of the persist property.
     * 
     * @param value
     *     allowed object is
     *     {@link Boolean }
     *     
     */
    public void setPersist(boolean value) {
        this.persist = value;
    }

    public boolean isSetPersist() {
        return (this.persist!= null);
    }

    public void unsetPersist() {
        this.persist = null;
    }

}
