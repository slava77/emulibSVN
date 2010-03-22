//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.19 at 06:37:09 PM CET 
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
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.cern.cms.csc.dw.model.base.ComponentBase;
import org.hibernate.annotations.Cache;
import org.hibernate.annotations.CacheConcurrencyStrategy;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for componentType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="componentClass" type="{http://www.cern.ch/cms/csc/dw/ontology}componentClassType"/>
 *         &lt;element name="id" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="synonym" type="{http://www.cern.ch/cms/csc/dw/ontology}componentSynonymType" maxOccurs="unbounded" minOccurs="0"/>
 *         &lt;element name="link" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkType" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "componentType", propOrder = {
    "componentClass",
    "id",
    "description",
    "synonyms",
    "links"
})
@Entity(name = "org.cern.cms.csc.dw.model.ontology.Component")
@Table(name = "CDW_COMPONENTS")
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE, region = "")
public class Component
    extends ComponentBase
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected ComponentClass componentClass;
    @XmlElement(required = true)
    protected String id;
    protected String description;
    @XmlElement(name = "synonym")
    protected List<ComponentSynonym> synonyms = new Vector<ComponentSynonym>();
    @XmlElement(name = "link")
    protected List<ComponentLink> links = new Vector<ComponentLink>();

    /**
     * Gets the value of the componentClass property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentClass }
     *     
     */
    @ManyToOne(targetEntity = ComponentClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.EAGER)
    @JoinColumn(name = "CMP_CCL_ID")
    public ComponentClass getComponentClass() {
        return componentClass;
    }

    /**
     * Sets the value of the componentClass property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentClass }
     *     
     */
    public void setComponentClass(ComponentClass value) {
        this.componentClass = value;
    }

    @Transient
    public boolean isSetComponentClass() {
        return (this.componentClass!= null);
    }

    /**
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Id
    @Column(name = "CMP_ID", length = 32)
    public String getId() {
        return id;
    }

    /**
     * Sets the value of the id property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setId(String value) {
        this.id = value;
    }

    @Transient
    public boolean isSetId() {
        return (this.id!= null);
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
    @Column(name = "CMP_DESCR", length = 2000)
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
     * Gets the value of the synonyms property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the synonyms property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getSynonyms().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link ComponentSynonym }
     * 
     * 
     */
    @OneToMany(targetEntity = ComponentSynonym.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "SYN_CMP_ID")
    public List<ComponentSynonym> getSynonyms() {
        if (synonyms == null) {
            synonyms = new Vector<ComponentSynonym>();
        }
        return this.synonyms;
    }

    /**
     * 
     * 
     */
    public void setSynonyms(List<ComponentSynonym> synonyms) {
        this.synonyms = synonyms;
    }

    @Transient
    public boolean isSetSynonyms() {
        return ((this.synonyms!= null)&&(!this.synonyms.isEmpty()));
    }

    public void unsetSynonyms() {
        this.synonyms = null;
    }

    /**
     * Gets the value of the links property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the links property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getLinks().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link ComponentLink }
     * 
     * 
     */
    @OneToMany(targetEntity = ComponentLink.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "LIN_CMP_ID")
    public List<ComponentLink> getLinks() {
        if (links == null) {
            links = new Vector<ComponentLink>();
        }
        return this.links;
    }

    /**
     * 
     * 
     */
    public void setLinks(List<ComponentLink> links) {
        this.links = links;
    }

    @Transient
    public boolean isSetLinks() {
        return ((this.links!= null)&&(!this.links.isEmpty()));
    }

    public void unsetLinks() {
        this.links = null;
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof Component)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final Component that = ((Component) object);
        equalsBuilder.append(this.getComponentClass(), that.getComponentClass());
        equalsBuilder.append(this.getId(), that.getId());
        equalsBuilder.append(this.getDescription(), that.getDescription());
        equalsBuilder.append(this.getSynonyms(), that.getSynonyms());
        equalsBuilder.append(this.getLinks(), that.getLinks());
    }

    public boolean equals(Object object) {
        if (!(object instanceof Component)) {
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
        hashCodeBuilder.append(this.getComponentClass());
        hashCodeBuilder.append(this.getId());
        hashCodeBuilder.append(this.getDescription());
        hashCodeBuilder.append(this.getSynonyms());
        hashCodeBuilder.append(this.getLinks());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            ComponentClass theComponentClass;
            theComponentClass = this.getComponentClass();
            toStringBuilder.append("componentClass", theComponentClass);
        }
        {
            String theId;
            theId = this.getId();
            toStringBuilder.append("id", theId);
        }
        {
            String theDescription;
            theDescription = this.getDescription();
            toStringBuilder.append("description", theDescription);
        }
        {
            List<ComponentSynonym> theSynonyms;
            theSynonyms = this.getSynonyms();
            toStringBuilder.append("synonyms", theSynonyms);
        }
        {
            List<ComponentLink> theLinks;
            theLinks = this.getLinks();
            toStringBuilder.append("links", theLinks);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
