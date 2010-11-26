//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.ontology;

import java.io.Serializable;
import java.util.List;
import java.util.Vector;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.gui.UseInTitle;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.hibernate.annotations.Cache;
import org.hibernate.annotations.CacheConcurrencyStrategy;


/**
 * <p>Java class for componentClassType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentClassType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="id" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="type" type="{http://www.cern.ch/cms/csc/dw/ontology}componentClassIdType"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="parent" type="{http://www.cern.ch/cms/csc/dw/ontology}componentClassType" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "componentClassType", propOrder = {
    "id",
    "type",
    "description",
    "parents"
})
@Entity(name = "org.cern.cms.csc.dw.model.ontology.ComponentClass")
@Table(name = "CDW_COMPONENT_CLASSES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "CCL_NAME"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE)
public class ComponentClass
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true, type = java.lang.Long.class)
    protected Long id;
    @XmlElement(required = true)
    @UseInTitle(order = 1)
    protected ComponentClassType type;
    protected String description;
    @XmlElement(name = "parent")
    protected List<ComponentClass> parents = new Vector<ComponentClass>();

    /**
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link java.lang.Long }
     *     
     */
    @Id
    @Column(name = "CCL_ID", scale = 0)
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
     *     {@link ComponentClassType }
     *     
     */
    @Transient
    public ComponentClassType getType() {
        return type;
    }

    /**
     * Sets the value of the type property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentClassType }
     *     
     */
    public void setType(ComponentClassType value) {
        this.type = value;
    }

    @Transient
    public boolean isSetType() {
        return (this.type!= null);
    }

    /**
     * Gets the value of the description property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic(optional = true)
    @Column(name = "CCL_DESCR", length = 2000)
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
     * Gets the value of the parents property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the parents property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getParents().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link ComponentClass }
     * 
     * 
     */
    @ManyToMany(targetEntity = org.cern.cms.csc.dw.model.ontology.ComponentClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinTable(name = "CDW_COMPONENT_CLASS_PARENTS", joinColumns = {
        @JoinColumn(name = "CCP_PARENT_CCL_ID")
    }, inverseJoinColumns = {
        @JoinColumn(name = "CCP_CHILD_CCL_ID")
    }, uniqueConstraints = {
        @UniqueConstraint(columnNames = {
            "CCP_PARENT_CCL_ID",
            "CCP_CHILD_CCL_ID"
        })
    })
    public List<ComponentClass> getParents() {
        if (parents == null) {
            parents = new Vector<ComponentClass>();
        }
        return this.parents;
    }

    /**
     * 
     * 
     */
    public void setParents(List<ComponentClass> parents) {
        this.parents = parents;
    }

    @Transient
    public boolean isSetParents() {
        return ((this.parents!= null)&&(!this.parents.isEmpty()));
    }

    public void unsetParents() {
        this.parents = null;
    }

    @Basic(optional = false)
    @Column(name = "CCL_NAME", length = 255)
    public String getTypeItem() {
        return ((this.getType() == null)?null:this.getType().value());
    }

    public void setTypeItem(String target) {
        setType(((target == null)?null:ComponentClassType.fromValue(target)));
    }

}
