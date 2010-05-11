//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.11 at 03:49:07 PM CEST 
//


package org.cern.cms.csc.dw.model.ontology;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.hibernate.annotations.Cache;
import org.hibernate.annotations.CacheConcurrencyStrategy;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for componentLinkClassType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentLinkClassType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="id" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="name" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassIdType"/>
 *         &lt;element name="inverse" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassType" minOccurs="0"/>
 *         &lt;element name="parent" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassType" minOccurs="0"/>
 *         &lt;element name="transitive" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "componentLinkClassType", propOrder = {
    "id",
    "name",
    "inverse",
    "parent",
    "transitive",
    "description"
})
@Entity(name = "org.cern.cms.csc.dw.model.ontology.ComponentLinkClass")
@Table(name = "CDW_COMPONENT_LINK_CLASSES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "LCL_NAME"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class ComponentLinkClass
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true, type = java.lang.Long.class)
    protected Long id;
    @XmlElement(required = true)
    protected ComponentLinkClassType name;
    protected ComponentLinkClass inverse;
    protected ComponentLinkClass parent;
    protected boolean transitive;
    protected String description;

    /**
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link java.lang.Long }
     *     
     */
    @Id
    @Column(name = "LCL_ID", scale = 0)
    public Long getId() {
        return id;
    }

    /**
     * Sets the value of the id property.
     * 
     * @param value
     *     allowed object is
     *     {@link java.lang.Long }
     *     
     */
    public void setId(Long value) {
        this.id = value;
    }

    @Transient
    public boolean isSetId() {
        return (this.id!= null);
    }

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClassType }
     *     
     */
    @Transient
    public ComponentLinkClassType getName() {
        return name;
    }

    /**
     * Sets the value of the name property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClassType }
     *     
     */
    public void setName(ComponentLinkClassType value) {
        this.name = value;
    }

    @Transient
    public boolean isSetName() {
        return (this.name!= null);
    }

    /**
     * Gets the value of the inverse property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClass }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.ComponentLinkClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "LCL_INVERSE_LCL_ID")
    public ComponentLinkClass getInverse() {
        return inverse;
    }

    /**
     * Sets the value of the inverse property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClass }
     *     
     */
    public void setInverse(ComponentLinkClass value) {
        this.inverse = value;
    }

    @Transient
    public boolean isSetInverse() {
        return (this.inverse!= null);
    }

    /**
     * Gets the value of the parent property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClass }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.ComponentLinkClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "LCL_PARENT_LCL_ID")
    public ComponentLinkClass getParent() {
        return parent;
    }

    /**
     * Sets the value of the parent property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClass }
     *     
     */
    public void setParent(ComponentLinkClass value) {
        this.parent = value;
    }

    @Transient
    public boolean isSetParent() {
        return (this.parent!= null);
    }

    /**
     * Gets the value of the transitive property.
     * 
     */
    @Basic
    @Column(name = "LCL_TRANSITIVE")
    public boolean isTransitive() {
        return transitive;
    }

    /**
     * Sets the value of the transitive property.
     * 
     */
    public void setTransitive(boolean value) {
        this.transitive = value;
    }

    @Transient
    public boolean isSetTransitive() {
        return true;
    }

    /**
     * Gets the value of the description property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "LCL_DESCR", length = 2000)
    public String getDescription() {
        return description;
    }

    /**
     * Sets the value of the description property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDescription(String value) {
        this.description = value;
    }

    @Transient
    public boolean isSetDescription() {
        return (this.description!= null);
    }

    @Basic(optional = false)
    @Column(name = "LCL_NAME", length = 255)
    public String getNameItem() {
        return ((this.getName() == null)?null:this.getName().value());
    }

    public void setNameItem(String target) {
        setName(((target == null)?null:ComponentLinkClassType.fromValue(target)));
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof ComponentLinkClass)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final ComponentLinkClass that = ((ComponentLinkClass) object);
        equalsBuilder.append(this.getId(), that.getId());
        equalsBuilder.append(this.getName(), that.getName());
        equalsBuilder.append(this.getInverse(), that.getInverse());
        equalsBuilder.append(this.getParent(), that.getParent());
        equalsBuilder.append(this.isTransitive(), that.isTransitive());
        equalsBuilder.append(this.getDescription(), that.getDescription());
    }

    public boolean equals(Object object) {
        if (!(object instanceof ComponentLinkClass)) {
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
        hashCodeBuilder.append(this.getId());
        hashCodeBuilder.append(this.getName());
        hashCodeBuilder.append(this.getInverse());
        hashCodeBuilder.append(this.getParent());
        hashCodeBuilder.append(this.isTransitive());
        hashCodeBuilder.append(this.getDescription());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            Long theId;
            theId = this.getId();
            toStringBuilder.append("id", theId);
        }
        {
            ComponentLinkClassType theName;
            theName = this.getName();
            toStringBuilder.append("name", theName);
        }
        {
            ComponentLinkClass theInverse;
            theInverse = this.getInverse();
            toStringBuilder.append("inverse", theInverse);
        }
        {
            ComponentLinkClass theParent;
            theParent = this.getParent();
            toStringBuilder.append("parent", theParent);
        }
        {
            boolean theTransitive;
            theTransitive = this.isTransitive();
            toStringBuilder.append("transitive", theTransitive);
        }
        {
            String theDescription;
            theDescription = this.getDescription();
            toStringBuilder.append("description", theDescription);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
