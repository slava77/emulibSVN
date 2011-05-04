//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.FactAnn;


/**
 * <p>Java class for DcsAlertFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DcsAlertFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}dcsFactType">
 *       &lt;sequence>
 *         &lt;element name="isOn" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isAcknowledged" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="alertText" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="sourceNumericValue" type="{http://www.w3.org/2001/XMLSchema}double" minOccurs="0"/>
 *         &lt;element name="sourceStringValue" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DcsAlertFactType", propOrder = {
    "isOn",
    "acknowledged",
    "alertText",
    "sourceNumericValue",
    "sourceStringValue"
})
@Entity(name = "DcsAlertFact")
@Table(name = "CDW_DCS_ALERT_FACTS")
@FactAnn(title = "DCS Alert")
public class DcsAlertFact
    extends DcsFact
    implements Serializable
{

    protected boolean isOn;
    @XmlElement(name = "isAcknowledged")
    protected boolean acknowledged;
    @XmlElement(required = true)
    protected String alertText;
    protected Double sourceNumericValue;
    protected String sourceStringValue;

    /**
     * Gets the value of the isOn property.
     * 
     */
    @Basic
    @Column(name = "FCT_IS_ALERT_ON", nullable = false)
    public boolean isIsOn() {
        return isOn;
    }

    /**
     * Sets the value of the isOn property.
     * 
     */
    public void setIsOn(boolean value) {
        this.isOn = value;
    }

    @Transient
    public boolean isSetIsOn() {
        return true;
    }

    /**
     * Gets the value of the acknowledged property.
     * 
     */
    @Basic
    @Column(name = "FCT_IS_ALERT_ACKNOWLEDGED", nullable = false)
    public boolean isAcknowledged() {
        return acknowledged;
    }

    /**
     * Sets the value of the acknowledged property.
     * 
     */
    public void setAcknowledged(boolean value) {
        this.acknowledged = value;
    }

    @Transient
    public boolean isSetAcknowledged() {
        return true;
    }

    /**
     * Gets the value of the alertText property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_ALERT_TEXT", length = 255)
    public String getAlertText() {
        return alertText;
    }

    /**
     * Sets the value of the alertText property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setAlertText(String value) {
        this.alertText = value;
    }

    @Transient
    public boolean isSetAlertText() {
        return (this.alertText!= null);
    }

    /**
     * Gets the value of the sourceNumericValue property.
     * 
     * @return
     *     possible object is
     *     {@link Double }
     *     
     */
    @Basic
    @Column(name = "FCT_SOURCE_NUMERIC_VALUE", precision = 20, scale = 10)
    public Double getSourceNumericValue() {
        return sourceNumericValue;
    }

    /**
     * Sets the value of the sourceNumericValue property.
     * 
     * @param value
     *     allowed object is
     *     {@link Double }
     *     
     */
    public void setSourceNumericValue(Double value) {
        this.sourceNumericValue = value;
    }

    @Transient
    public boolean isSetSourceNumericValue() {
        return (this.sourceNumericValue!= null);
    }

    /**
     * Gets the value of the sourceStringValue property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_SOURCE_STRING_VALUE", length = 255)
    public String getSourceStringValue() {
        return sourceStringValue;
    }

    /**
     * Sets the value of the sourceStringValue property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSourceStringValue(String value) {
        this.sourceStringValue = value;
    }

    @Transient
    public boolean isSetSourceStringValue() {
        return (this.sourceStringValue!= null);
    }

}
