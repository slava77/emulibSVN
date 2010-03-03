//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.03 at 03:10:26 PM CET 
//


package org.cern.cms.csc.dw.model.ontology;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
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
 * <p>Java class for componentLinkType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentLinkType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="componentLinkClass" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassType"/>
 *         &lt;element name="component" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType"/>
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
@XmlType(name = "componentLinkType", propOrder = {
    "componentLinkClass",
    "component",
    "description"
})
@Entity(name = "org.cern.cms.csc.dw.model.ontology.ComponentLink")
@Table(name = "CDW_COMPONENT_LINKS")
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE, region = "")
public class ComponentLink
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected ComponentLinkClass componentLinkClass;
    @XmlElement(required = true)
    protected Component component;
    protected String description;
    @XmlAttribute(name = "Id")
    protected Long id;

    /**
     * Gets the value of the componentLinkClass property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClass }
     *     
     */
    @ManyToOne(targetEntity = ComponentLinkClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.EAGER)
    @JoinColumn(name = "LIN_LCL_ID")
    public ComponentLinkClass getComponentLinkClass() {
        return componentLinkClass;
    }

    /**
     * Sets the value of the componentLinkClass property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClass }
     *     
     */
    public void setComponentLinkClass(ComponentLinkClass value) {
        this.componentLinkClass = value;
    }

    @Transient
    public boolean isSetComponentLinkClass() {
        return (this.componentLinkClass!= null);
    }

    /**
     * Gets the value of the component property.
     * 
     * @return
     *     possible object is
     *     {@link Component }
     *     
     */
    @ManyToOne(targetEntity = Component.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.EAGER)
    @JoinColumn(name = "LIN_LINKED_CMP_ID")
    public Component getComponent() {
        return component;
    }

    /**
     * Sets the value of the component property.
     * 
     * @param value
     *     allowed object is
     *     {@link Component }
     *     
     */
    public void setComponent(Component value) {
        this.component = value;
    }

    @Transient
    public boolean isSetComponent() {
        return (this.component!= null);
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
    @Column(name = "LIN_DESCR", length = 2000)
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

    /**
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Id
    @Column(name = "LIN_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "CDW_LIN_ID_SEQ", sequenceName = "CDW_LIN_ID_SEQ")
    public Long getId() {
        return id;
    }

    /**
     * Sets the value of the id property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setId(Long value) {
        this.id = value;
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof ComponentLink)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final ComponentLink that = ((ComponentLink) object);
        equalsBuilder.append(this.getComponentLinkClass(), that.getComponentLinkClass());
        equalsBuilder.append(this.getComponent(), that.getComponent());
        equalsBuilder.append(this.getDescription(), that.getDescription());
    }

    public boolean equals(Object object) {
        if (!(object instanceof ComponentLink)) {
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
        hashCodeBuilder.append(this.getComponentLinkClass());
        hashCodeBuilder.append(this.getComponent());
        hashCodeBuilder.append(this.getDescription());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            ComponentLinkClass theComponentLinkClass;
            theComponentLinkClass = this.getComponentLinkClass();
            toStringBuilder.append("componentLinkClass", theComponentLinkClass);
        }
        {
            Component theComponent;
            theComponent = this.getComponent();
            toStringBuilder.append("component", theComponent);
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
