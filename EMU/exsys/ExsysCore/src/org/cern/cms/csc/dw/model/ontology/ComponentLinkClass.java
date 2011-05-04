//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.ontology;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
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
import jsf.bean.gui.annotation.UseInTitle;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.hibernate.annotations.Cache;
import org.hibernate.annotations.CacheConcurrencyStrategy;


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
 *         &lt;element name="type" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassIdType"/>
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
    "type",
    "inverse",
    "parent",
    "transitive",
    "description"
})
@Entity(name = "ComponentLinkClass")
@Table(name = "CDW_COMPONENT_LINK_CLASSES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "LCL_NAME"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class ComponentLinkClass
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true, type = java.lang.Long.class)
    protected Long id;
    @XmlElement(required = true)
    @UseInTitle(order = 1)
    protected ComponentLinkClassType type;
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
     * Gets the value of the type property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClassType }
     *     
     */
    @Basic(optional = false)
    @Column(name = "LCL_NAME", length = 255)
    @Enumerated(EnumType.STRING)
    public ComponentLinkClassType getType() {
        return type;
    }

    /**
     * Sets the value of the type property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClassType }
     *     
     */
    public void setType(ComponentLinkClassType value) {
        this.type = value;
    }

    @Transient
    public boolean isSetType() {
        return (this.type!= null);
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
    
//--simple--preserve

    @Override
    public String toString() {
        return getType().getValue();
    }

//--simple--preserve

}
