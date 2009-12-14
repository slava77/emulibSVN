//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2009.12.14 at 12:26:53 AM CET 
//


package ch.cern.cms.csc.cdw.data;

import java.math.BigDecimal;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;
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
 *     &lt;extension base="{http://www.cern.ch/cms/csc/cdw/data}factType">
 *       &lt;sequence>
 *         &lt;element name="alctCount" type="{http://www.w3.org/2001/XMLSchema}decimal"/>
 *         &lt;element name="clctCount" type="{http://www.w3.org/2001/XMLSchema}decimal"/>
 *         &lt;element name="lctCount" type="{http://www.w3.org/2001/XMLSchema}decimal"/>
 *         &lt;element name="l1aCount" type="{http://www.w3.org/2001/XMLSchema}decimal"/>
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
@Entity(name = "ch.cern.cms.csc.cdw.data.CumulativeTmbTriggerCounterFactType")
@Table(name = "CUMULATIVETMBTRIGGERCOUNTERF_0")
public class CumulativeTmbTriggerCounterFactType
    extends FactType
    implements Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected BigDecimal alctCount;
    @XmlElement(required = true)
    protected BigDecimal clctCount;
    @XmlElement(required = true)
    protected BigDecimal lctCount;
    @XmlElement(name = "l1aCount", required = true)
    protected BigDecimal l1ACount;

    /**
     * Gets the value of the alctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigDecimal }
     *     
     */
    @Basic
    @Column(name = "ALCTCOUNT", precision = 20, scale = 10)
    public BigDecimal getAlctCount() {
        return alctCount;
    }

    /**
     * Sets the value of the alctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigDecimal }
     *     
     */
    public void setAlctCount(BigDecimal value) {
        this.alctCount = value;
    }

    /**
     * Gets the value of the clctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigDecimal }
     *     
     */
    @Basic
    @Column(name = "CLCTCOUNT", precision = 20, scale = 10)
    public BigDecimal getClctCount() {
        return clctCount;
    }

    /**
     * Sets the value of the clctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigDecimal }
     *     
     */
    public void setClctCount(BigDecimal value) {
        this.clctCount = value;
    }

    /**
     * Gets the value of the lctCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigDecimal }
     *     
     */
    @Basic
    @Column(name = "LCTCOUNT", precision = 20, scale = 10)
    public BigDecimal getLctCount() {
        return lctCount;
    }

    /**
     * Sets the value of the lctCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigDecimal }
     *     
     */
    public void setLctCount(BigDecimal value) {
        this.lctCount = value;
    }

    /**
     * Gets the value of the l1ACount property.
     * 
     * @return
     *     possible object is
     *     {@link BigDecimal }
     *     
     */
    @Basic
    @Column(name = "L1ACOUNT", precision = 20, scale = 10)
    public BigDecimal getL1ACount() {
        return l1ACount;
    }

    /**
     * Sets the value of the l1ACount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigDecimal }
     *     
     */
    public void setL1ACount(BigDecimal value) {
        this.l1ACount = value;
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof CumulativeTmbTriggerCounterFactType)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final CumulativeTmbTriggerCounterFactType that = ((CumulativeTmbTriggerCounterFactType) object);
        equalsBuilder.append(this.getAlctCount(), that.getAlctCount());
        equalsBuilder.append(this.getClctCount(), that.getClctCount());
        equalsBuilder.append(this.getLctCount(), that.getLctCount());
        equalsBuilder.append(this.getL1ACount(), that.getL1ACount());
    }

    public boolean equals(Object object) {
        if (!(object instanceof CumulativeTmbTriggerCounterFactType)) {
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
            BigDecimal theAlctCount;
            theAlctCount = this.getAlctCount();
            toStringBuilder.append("alctCount", theAlctCount);
        }
        {
            BigDecimal theClctCount;
            theClctCount = this.getClctCount();
            toStringBuilder.append("clctCount", theClctCount);
        }
        {
            BigDecimal theLctCount;
            theLctCount = this.getLctCount();
            toStringBuilder.append("lctCount", theLctCount);
        }
        {
            BigDecimal theL1ACount;
            theL1ACount = this.getL1ACount();
            toStringBuilder.append("l1ACount", theL1ACount);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
