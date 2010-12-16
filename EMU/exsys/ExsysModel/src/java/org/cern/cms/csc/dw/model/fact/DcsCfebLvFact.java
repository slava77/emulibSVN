//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import java.math.BigInteger;
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
 * <p>Java class for DcsCfebLvFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DcsCfebLvFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="status" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="current_5v" type="{http://www.w3.org/2001/XMLSchema}double"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DcsCfebLvFactType", propOrder = {
    "status",
    "current5V"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DcsCfebLvFact")
@Table(name = "CDW_DCS_CFEB_LV_FACTS")
@FactAnn(limitComponents = {
    "CFEB"
}, title = "CFEB LV data")
public class DcsCfebLvFact
    extends Fact
    implements Serializable
{

    @XmlElement(required = true)
    protected BigInteger status;
    @XmlElement(name = "current_5v")
    protected double current5V;

    /**
     * Gets the value of the status property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_DATA_STATUS", precision = 10, scale = 0)
    public BigInteger getStatus() {
        return status;
    }

    /**
     * Sets the value of the status property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setStatus(BigInteger value) {
        this.status = value;
    }

    @Transient
    public boolean isSetStatus() {
        return (this.status!= null);
    }

    /**
     * Gets the value of the current5V property.
     * 
     */
    @Basic
    @Column(name = "FCT_CURRENT_5V", precision = 20, scale = 10)
    public double getCurrent5V() {
        return current5V;
    }

    /**
     * Sets the value of the current5V property.
     * 
     */
    public void setCurrent5V(double value) {
        this.current5V = value;
    }

    @Transient
    public boolean isSetCurrent5V() {
        return true;
    }

}
