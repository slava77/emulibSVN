//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.11 at 03:49:07 PM CEST 
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
 * <p>Java class for DqmCscAfebFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmCscAfebFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="CSC_ALCT_AFEB_NOISY" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="CSC_ALCT_NO_ANODE_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmCscAfebFactType", propOrder = {
    "online",
    "alctAfebNoisy",
    "alctNoAnodeData"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmCscAfebFact")
@Table(name = "CDW_DQM_CSC_AFEB_FACTS")
public class DqmCscAfebFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    protected boolean online;
    @XmlElement(name = "CSC_ALCT_AFEB_NOISY")
    protected Long alctAfebNoisy;
    @XmlElement(name = "CSC_ALCT_NO_ANODE_DATA")
    protected Long alctNoAnodeData;

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
     * Gets the value of the alctAfebNoisy property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_ALCT_AFEB_NOISY", precision = 20, scale = 10)
    public Long getAlctAfebNoisy() {
        return alctAfebNoisy;
    }

    /**
     * Sets the value of the alctAfebNoisy property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setAlctAfebNoisy(Long value) {
        this.alctAfebNoisy = value;
    }

    @Transient
    public boolean isSetAlctAfebNoisy() {
        return (this.alctAfebNoisy!= null);
    }

    /**
     * Gets the value of the alctNoAnodeData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_ALCT_NO_ANODE_DATA", precision = 20, scale = 10)
    public Long getAlctNoAnodeData() {
        return alctNoAnodeData;
    }

    /**
     * Sets the value of the alctNoAnodeData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setAlctNoAnodeData(Long value) {
        this.alctNoAnodeData = value;
    }

    @Transient
    public boolean isSetAlctNoAnodeData() {
        return (this.alctNoAnodeData!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DqmCscAfebFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final DqmCscAfebFact that = ((DqmCscAfebFact) object);
        equalsBuilder.append(this.isOnline(), that.isOnline());
        equalsBuilder.append(this.getAlctAfebNoisy(), that.getAlctAfebNoisy());
        equalsBuilder.append(this.getAlctNoAnodeData(), that.getAlctNoAnodeData());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DqmCscAfebFact)) {
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
        hashCodeBuilder.append(this.getAlctAfebNoisy());
        hashCodeBuilder.append(this.getAlctNoAnodeData());
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
            Long theAlctAfebNoisy;
            theAlctAfebNoisy = this.getAlctAfebNoisy();
            toStringBuilder.append("alctAfebNoisy", theAlctAfebNoisy);
        }
        {
            Long theAlctNoAnodeData;
            theAlctNoAnodeData = this.getAlctNoAnodeData();
            toStringBuilder.append("alctNoAnodeData", theAlctNoAnodeData);
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
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.AFEB);
    }


//--simple--preserve

}
