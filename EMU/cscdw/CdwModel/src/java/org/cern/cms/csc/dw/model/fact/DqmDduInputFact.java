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
 * <p>Java class for DqmDduInputFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmDduInputFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="DDU_INPUT_IN_ERROR_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_INPUT_IN_WARNING_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_NO_INPUT_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmDduInputFactType", propOrder = {
    "online",
    "inputInErrorState",
    "inputInWarningState",
    "inputNoData"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmDduInputFact")
@Table(name = "CDW_DQM_DDU_INPUT_FACTS")
public class DqmDduInputFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    protected boolean online;
    @XmlElement(name = "DDU_INPUT_IN_ERROR_STATE")
    protected Long inputInErrorState;
    @XmlElement(name = "DDU_INPUT_IN_WARNING_STATE")
    protected Long inputInWarningState;
    @XmlElement(name = "DDU_NO_INPUT_DATA")
    protected Long inputNoData;

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
     * Gets the value of the inputInErrorState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_INPUT_IN_ERROR_STATE", precision = 20, scale = 10)
    public Long getInputInErrorState() {
        return inputInErrorState;
    }

    /**
     * Sets the value of the inputInErrorState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputInErrorState(Long value) {
        this.inputInErrorState = value;
    }

    @Transient
    public boolean isSetInputInErrorState() {
        return (this.inputInErrorState!= null);
    }

    /**
     * Gets the value of the inputInWarningState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_INPUT_IN_WARN_STATE", precision = 20, scale = 10)
    public Long getInputInWarningState() {
        return inputInWarningState;
    }

    /**
     * Sets the value of the inputInWarningState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputInWarningState(Long value) {
        this.inputInWarningState = value;
    }

    @Transient
    public boolean isSetInputInWarningState() {
        return (this.inputInWarningState!= null);
    }

    /**
     * Gets the value of the inputNoData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_NO_INPUT_DATA", precision = 20, scale = 10)
    public Long getInputNoData() {
        return inputNoData;
    }

    /**
     * Sets the value of the inputNoData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputNoData(Long value) {
        this.inputNoData = value;
    }

    @Transient
    public boolean isSetInputNoData() {
        return (this.inputNoData!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DqmDduInputFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final DqmDduInputFact that = ((DqmDduInputFact) object);
        equalsBuilder.append(this.isOnline(), that.isOnline());
        equalsBuilder.append(this.getInputInErrorState(), that.getInputInErrorState());
        equalsBuilder.append(this.getInputInWarningState(), that.getInputInWarningState());
        equalsBuilder.append(this.getInputNoData(), that.getInputNoData());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DqmDduInputFact)) {
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
        hashCodeBuilder.append(this.getInputInErrorState());
        hashCodeBuilder.append(this.getInputInWarningState());
        hashCodeBuilder.append(this.getInputNoData());
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
            Long theInputInErrorState;
            theInputInErrorState = this.getInputInErrorState();
            toStringBuilder.append("inputInErrorState", theInputInErrorState);
        }
        {
            Long theInputInWarningState;
            theInputInWarningState = this.getInputInWarningState();
            toStringBuilder.append("inputInWarningState", theInputInWarningState);
        }
        {
            Long theInputNoData;
            theInputNoData = this.getInputNoData();
            toStringBuilder.append("inputNoData", theInputNoData);
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
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.DDU_INPUT);
    }


//--simple--preserve

}
