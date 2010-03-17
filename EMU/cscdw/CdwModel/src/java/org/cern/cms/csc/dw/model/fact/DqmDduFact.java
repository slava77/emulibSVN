//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.15 at 07:35:42 PM CET 
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
 * <p>Java class for DqmDduFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmDduFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="DDU_WITH_FORMAT_ERRORS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_WITH_FORMAT_ERRORS_F" type="{http://www.w3.org/2001/XMLSchema}float" minOccurs="0"/>
 *         &lt;element name="DDU_WITH_TRAILER_ERRORS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_WITH_TRAILER_ERRORS_F" type="{http://www.w3.org/2001/XMLSchema}float" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmDduFactType", propOrder = {
    "online",
    "withFormatErrors",
    "withFormatErrorsFraction",
    "withTrailerErrors",
    "withTrailerErrorsFraction"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmDduFact")
@Table(name = "CDW_DQM_DDU_FACTS")
public class DqmDduFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    protected boolean online;
    @XmlElement(name = "DDU_WITH_FORMAT_ERRORS")
    protected Long withFormatErrors;
    @XmlElement(name = "DDU_WITH_FORMAT_ERRORS_F")
    protected Float withFormatErrorsFraction;
    @XmlElement(name = "DDU_WITH_TRAILER_ERRORS")
    protected Long withTrailerErrors;
    @XmlElement(name = "DDU_WITH_TRAILER_ERRORS_F")
    protected Float withTrailerErrorsFraction;

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
     * Gets the value of the withFormatErrors property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_FORMAT_ERRORS", precision = 20, scale = 10)
    public Long getWithFormatErrors() {
        return withFormatErrors;
    }

    /**
     * Sets the value of the withFormatErrors property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setWithFormatErrors(Long value) {
        this.withFormatErrors = value;
    }

    @Transient
    public boolean isSetWithFormatErrors() {
        return (this.withFormatErrors!= null);
    }

    /**
     * Gets the value of the withFormatErrorsFraction property.
     * 
     * @return
     *     possible object is
     *     {@link Float }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_FORMAT_ERRORS_F", precision = 20, scale = 10)
    public Float getWithFormatErrorsFraction() {
        return withFormatErrorsFraction;
    }

    /**
     * Sets the value of the withFormatErrorsFraction property.
     * 
     * @param value
     *     allowed object is
     *     {@link Float }
     *     
     */
    public void setWithFormatErrorsFraction(Float value) {
        this.withFormatErrorsFraction = value;
    }

    @Transient
    public boolean isSetWithFormatErrorsFraction() {
        return (this.withFormatErrorsFraction!= null);
    }

    /**
     * Gets the value of the withTrailerErrors property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_TRAILER_ERRORS", precision = 20, scale = 10)
    public Long getWithTrailerErrors() {
        return withTrailerErrors;
    }

    /**
     * Sets the value of the withTrailerErrors property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setWithTrailerErrors(Long value) {
        this.withTrailerErrors = value;
    }

    @Transient
    public boolean isSetWithTrailerErrors() {
        return (this.withTrailerErrors!= null);
    }

    /**
     * Gets the value of the withTrailerErrorsFraction property.
     * 
     * @return
     *     possible object is
     *     {@link Float }
     *     
     */
    @Basic
    @Column(name = "FCT_WITH_TRAILER_ERRORS_F", precision = 20, scale = 10)
    public Float getWithTrailerErrorsFraction() {
        return withTrailerErrorsFraction;
    }

    /**
     * Sets the value of the withTrailerErrorsFraction property.
     * 
     * @param value
     *     allowed object is
     *     {@link Float }
     *     
     */
    public void setWithTrailerErrorsFraction(Float value) {
        this.withTrailerErrorsFraction = value;
    }

    @Transient
    public boolean isSetWithTrailerErrorsFraction() {
        return (this.withTrailerErrorsFraction!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DqmDduFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final DqmDduFact that = ((DqmDduFact) object);
        equalsBuilder.append(this.isOnline(), that.isOnline());
        equalsBuilder.append(this.getWithFormatErrors(), that.getWithFormatErrors());
        equalsBuilder.append(this.getWithFormatErrorsFraction(), that.getWithFormatErrorsFraction());
        equalsBuilder.append(this.getWithTrailerErrors(), that.getWithTrailerErrors());
        equalsBuilder.append(this.getWithTrailerErrorsFraction(), that.getWithTrailerErrorsFraction());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DqmDduFact)) {
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
        hashCodeBuilder.append(this.getWithFormatErrors());
        hashCodeBuilder.append(this.getWithFormatErrorsFraction());
        hashCodeBuilder.append(this.getWithTrailerErrors());
        hashCodeBuilder.append(this.getWithTrailerErrorsFraction());
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
            Long theWithFormatErrors;
            theWithFormatErrors = this.getWithFormatErrors();
            toStringBuilder.append("withFormatErrors", theWithFormatErrors);
        }
        {
            Float theWithFormatErrorsFraction;
            theWithFormatErrorsFraction = this.getWithFormatErrorsFraction();
            toStringBuilder.append("withFormatErrorsFraction", theWithFormatErrorsFraction);
        }
        {
            Long theWithTrailerErrors;
            theWithTrailerErrors = this.getWithTrailerErrors();
            toStringBuilder.append("withTrailerErrors", theWithTrailerErrors);
        }
        {
            Float theWithTrailerErrorsFraction;
            theWithTrailerErrorsFraction = this.getWithTrailerErrorsFraction();
            toStringBuilder.append("withTrailerErrorsFraction", theWithTrailerErrorsFraction);
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
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.DDU);
    }


//--simple--preserve

}
