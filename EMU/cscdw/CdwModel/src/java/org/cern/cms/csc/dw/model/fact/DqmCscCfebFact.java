//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.05 at 03:09:15 PM CET 
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
 * <p>Java class for DqmCscCfebFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmCscCfebFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="CSC_WITH_LOW_CFEB_DAV_EFF" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="CSC_WITH_LOW_CFEB_DAV_EFF_F" type="{http://www.w3.org/2001/XMLSchema}float" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmCscCfebFactType", propOrder = {
    "online",
    "withLowCfebDavEfficiency",
    "withLowCfebDavEfficiencyFraction"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmCscCfebFact")
@Table(name = "CDW_DQM_CSC_CFEB_FACTS")
public class DqmCscCfebFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    protected boolean online;
    @XmlElement(name = "CSC_WITH_LOW_CFEB_DAV_EFF")
    protected Long withLowCfebDavEfficiency;
    @XmlElement(name = "CSC_WITH_LOW_CFEB_DAV_EFF_F")
    protected Float withLowCfebDavEfficiencyFraction;

    /**
     * Gets the value of the online property.
     * 
     */
    @Basic
    @Column(name = "FCT_ONLINE")
    public boolean isOnline() {
        return online;
    }

    /**
     * Sets the value of the online property.
     * 
     */
    public void setOnline(boolean value) {
        this.online = value;
    }

    @Transient
    public boolean isSetOnline() {
        return true;
    }

    /**
     * Gets the value of the withLowCfebDavEfficiency property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_LOW_CFEB_DAV_EFF", precision = 20, scale = 10)
    public Long getWithLowCfebDavEfficiency() {
        return withLowCfebDavEfficiency;
    }

    /**
     * Sets the value of the withLowCfebDavEfficiency property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setWithLowCfebDavEfficiency(Long value) {
        this.withLowCfebDavEfficiency = value;
    }

    @Transient
    public boolean isSetWithLowCfebDavEfficiency() {
        return (this.withLowCfebDavEfficiency!= null);
    }

    /**
     * Gets the value of the withLowCfebDavEfficiencyFraction property.
     * 
     * @return
     *     possible object is
     *     {@link Float }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_LOW_CFEB_DAV_EFF_F", precision = 20, scale = 10)
    public Float getWithLowCfebDavEfficiencyFraction() {
        return withLowCfebDavEfficiencyFraction;
    }

    /**
     * Sets the value of the withLowCfebDavEfficiencyFraction property.
     * 
     * @param value
     *     allowed object is
     *     {@link Float }
     *     
     */
    public void setWithLowCfebDavEfficiencyFraction(Float value) {
        this.withLowCfebDavEfficiencyFraction = value;
    }

    @Transient
    public boolean isSetWithLowCfebDavEfficiencyFraction() {
        return (this.withLowCfebDavEfficiencyFraction!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DqmCscCfebFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final DqmCscCfebFact that = ((DqmCscCfebFact) object);
        equalsBuilder.append(this.isOnline(), that.isOnline());
        equalsBuilder.append(this.getWithLowCfebDavEfficiency(), that.getWithLowCfebDavEfficiency());
        equalsBuilder.append(this.getWithLowCfebDavEfficiencyFraction(), that.getWithLowCfebDavEfficiencyFraction());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DqmCscCfebFact)) {
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
        hashCodeBuilder.append(this.isOnline());
        hashCodeBuilder.append(this.getWithLowCfebDavEfficiency());
        hashCodeBuilder.append(this.getWithLowCfebDavEfficiencyFraction());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        super.toString(toStringBuilder);
        {
            boolean theOnline;
            theOnline = this.isOnline();
            toStringBuilder.append("online", theOnline);
        }
        {
            Long theWithLowCfebDavEfficiency;
            theWithLowCfebDavEfficiency = this.getWithLowCfebDavEfficiency();
            toStringBuilder.append("withLowCfebDavEfficiency", theWithLowCfebDavEfficiency);
        }
        {
            Float theWithLowCfebDavEfficiencyFraction;
            theWithLowCfebDavEfficiencyFraction = this.getWithLowCfebDavEfficiencyFraction();
            toStringBuilder.append("withLowCfebDavEfficiencyFraction", theWithLowCfebDavEfficiencyFraction);
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
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.CFEB);
    }


//--simple--preserve

}
