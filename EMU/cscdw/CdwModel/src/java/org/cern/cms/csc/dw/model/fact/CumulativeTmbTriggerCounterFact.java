//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.05 at 07:17:06 PM CET 
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
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


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
public class CumulativeTmbTriggerCounterFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
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

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof CumulativeTmbTriggerCounterFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final CumulativeTmbTriggerCounterFact that = ((CumulativeTmbTriggerCounterFact) object);
        equalsBuilder.append(this.getAlctCount(), that.getAlctCount());
        equalsBuilder.append(this.getClctCount(), that.getClctCount());
        equalsBuilder.append(this.getLctCount(), that.getLctCount());
        equalsBuilder.append(this.getL1ACount(), that.getL1ACount());
    }

    public boolean equals(Object object) {
        if (!(object instanceof CumulativeTmbTriggerCounterFact)) {
            return false;
        }
        if (this == object) {
            return true;
        }
        final EqualsBuilder equalsBuilder = new JAXBEqualsBuilder();
        equals(object, equalsBuilder);
        return equalsBuilder.isEquals();
    }

    public void hashCode(HashCodeBuilder hashCodeBuilder) {
        super.hashCode(hashCodeBuilder);
        hashCodeBuilder.append(this.getAlctCount());
        hashCodeBuilder.append(this.getClctCount());
        hashCodeBuilder.append(this.getLctCount());
        hashCodeBuilder.append(this.getL1ACount());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        super.toString(toStringBuilder);
        {
            BigInteger theAlctCount;
            theAlctCount = this.getAlctCount();
            toStringBuilder.append("alctCount", theAlctCount);
        }
        {
            BigInteger theClctCount;
            theClctCount = this.getClctCount();
            toStringBuilder.append("clctCount", theClctCount);
        }
        {
            BigInteger theLctCount;
            theLctCount = this.getLctCount();
            toStringBuilder.append("lctCount", theLctCount);
        }
        {
            BigInteger theL1ACount;
            theL1ACount = this.getL1ACount();
            toStringBuilder.append("l1ACount", theL1ACount);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }
    
//--simple--preserve

    @Override
    public boolean checkComponentClassType(org.cern.cms.csc.dw.model.ontology.ComponentClassType componentClassType) {
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.TMB);
    }


//--simple--preserve

}
