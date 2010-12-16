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
 * <p>Java class for cumulativeTmbTriggerCounterFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="cumulativeTmbTriggerCounterFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="alctCount" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="clctCount" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="lctCount" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="l1aCount" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "cumulativeTmbTriggerCounterFactType", propOrder = {
    "alctCount",
    "clctCount",
    "lctCount",
    "l1ACount"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.CumulativeTmbTriggerCounterFact")
@Table(name = "CDW_TMB_TRG_CNT_CUMUL_FACTS")
@FactAnn(limitComponents = {
    "TMB"
}, title = "Cumulative TMB Trigger Counters")
public class CumulativeTmbTriggerCounterFact
    extends Fact
    implements Serializable
{

    @XmlElement(required = true)
    protected BigInteger alctCount;
    @XmlElement(required = true)
    protected BigInteger clctCount;
    @XmlElement(required = true)
    protected BigInteger lctCount;
    @XmlElement(name = "l1aCount", required = true)
    protected BigInteger l1ACount;

    /**
     * Gets the value of the alctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_ALCT_COUNT", precision = 10, scale = 0)
    public BigInteger getAlctCount() {
        return alctCount;
    }

    /**
     * Sets the value of the alctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setAlctCount(BigInteger value) {
        this.alctCount = value;
    }

    @Transient
    public boolean isSetAlctCount() {
        return (this.alctCount!= null);
    }

    /**
     * Gets the value of the clctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_CLCT_COUNT", precision = 10, scale = 0)
    public BigInteger getClctCount() {
        return clctCount;
    }

    /**
     * Sets the value of the clctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setClctCount(BigInteger value) {
        this.clctCount = value;
    }

    @Transient
    public boolean isSetClctCount() {
        return (this.clctCount!= null);
    }

    /**
     * Gets the value of the lctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_LCT_COUNT", precision = 10, scale = 0)
    public BigInteger getLctCount() {
        return lctCount;
    }

    /**
     * Sets the value of the lctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setLctCount(BigInteger value) {
        this.lctCount = value;
    }

    @Transient
    public boolean isSetLctCount() {
        return (this.lctCount!= null);
    }

    /**
     * Gets the value of the l1ACount property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_L1A_COUNT", precision = 10, scale = 0)
    public BigInteger getL1ACount() {
        return l1ACount;
    }

    /**
     * Sets the value of the l1ACount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setL1ACount(BigInteger value) {
        this.l1ACount = value;
    }

    @Transient
    public boolean isSetL1ACount() {
        return (this.l1ACount!= null);
    }

}
