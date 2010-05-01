//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.04.29 at 09:33:27 PM CEST 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
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
 * <p>Java class for localDAQStatusFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="localDAQStatusFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="daqState" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="isBuildingEvents" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isSupervised" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="runType" type="{http://www.cern.ch/cms/csc/dw/model}runTypeType"/>
 *         &lt;element name="state" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "localDAQStatusFactType", propOrder = {
    "daqState",
    "isBuildingEvents",
    "isSupervised",
    "runType",
    "state"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.LocalDAQStatusFact")
@Table(name = "CDW_LOCAL_DAQ_STATUS_FACTS")
public class LocalDAQStatusFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected String daqState;
    protected boolean isBuildingEvents;
    protected boolean isSupervised;
    @XmlElement(required = true)
    protected RunType runType;
    @XmlElement(required = true)
    protected String state;

    /**
     * Gets the value of the daqState property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_DAQ_STATE", length = 16)
    public String getDaqState() {
        return daqState;
    }

    /**
     * Sets the value of the daqState property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDaqState(String value) {
        this.daqState = value;
    }

    @Transient
    public boolean isSetDaqState() {
        return (this.daqState!= null);
    }

    /**
     * Gets the value of the isBuildingEvents property.
     * 
     */
    @Basic
    @Column(name = "FCT_IS_BUILDING_EVENTS")
    public boolean isIsBuildingEvents() {
        return isBuildingEvents;
    }

    /**
     * Sets the value of the isBuildingEvents property.
     * 
     */
    public void setIsBuildingEvents(boolean value) {
        this.isBuildingEvents = value;
    }

    @Transient
    public boolean isSetIsBuildingEvents() {
        return true;
    }

    /**
     * Gets the value of the isSupervised property.
     * 
     */
    @Basic
    @Column(name = "FCT_IS_SUPERVISED")
    public boolean isIsSupervised() {
        return isSupervised;
    }

    /**
     * Sets the value of the isSupervised property.
     * 
     */
    public void setIsSupervised(boolean value) {
        this.isSupervised = value;
    }

    @Transient
    public boolean isSetIsSupervised() {
        return true;
    }

    /**
     * Gets the value of the runType property.
     * 
     * @return
     *     possible object is
     *     {@link RunType }
     *     
     */
    @Basic
    @Column(name = "FCT_RUN_TYPE", length = 32)
    @Enumerated(EnumType.STRING)
    public RunType getRunType() {
        return runType;
    }

    /**
     * Sets the value of the runType property.
     * 
     * @param value
     *     allowed object is
     *     {@link RunType }
     *     
     */
    public void setRunType(RunType value) {
        this.runType = value;
    }

    @Transient
    public boolean isSetRunType() {
        return (this.runType!= null);
    }

    /**
     * Gets the value of the state property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_STATE", length = 16)
    public String getState() {
        return state;
    }

    /**
     * Sets the value of the state property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setState(String value) {
        this.state = value;
    }

    @Transient
    public boolean isSetState() {
        return (this.state!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof LocalDAQStatusFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final LocalDAQStatusFact that = ((LocalDAQStatusFact) object);
        equalsBuilder.append(this.getDaqState(), that.getDaqState());
        equalsBuilder.append(this.isIsBuildingEvents(), that.isIsBuildingEvents());
        equalsBuilder.append(this.isIsSupervised(), that.isIsSupervised());
        equalsBuilder.append(this.getRunType(), that.getRunType());
        equalsBuilder.append(this.getState(), that.getState());
    }

    public boolean equals(Object object) {
        if (!(object instanceof LocalDAQStatusFact)) {
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
        hashCodeBuilder.append(this.getDaqState());
        hashCodeBuilder.append(this.isIsBuildingEvents());
        hashCodeBuilder.append(this.isIsSupervised());
        hashCodeBuilder.append(this.getRunType());
        hashCodeBuilder.append(this.getState());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        super.toString(toStringBuilder);
        {
            String theDaqState;
            theDaqState = this.getDaqState();
            toStringBuilder.append("daqState", theDaqState);
        }
        {
            boolean theIsBuildingEvents;
            theIsBuildingEvents = this.isIsBuildingEvents();
            toStringBuilder.append("isBuildingEvents", theIsBuildingEvents);
        }
        {
            boolean theIsSupervised;
            theIsSupervised = this.isIsSupervised();
            toStringBuilder.append("isSupervised", theIsSupervised);
        }
        {
            RunType theRunType;
            theRunType = this.getRunType();
            toStringBuilder.append("runType", theRunType);
        }
        {
            String theState;
            theState = this.getState();
            toStringBuilder.append("state", theState);
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
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.SOFTWARE);
    }

//--simple--preserve

}
