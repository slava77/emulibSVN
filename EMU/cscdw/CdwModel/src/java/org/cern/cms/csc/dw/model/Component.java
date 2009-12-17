//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2009.12.17 at 05:04:08 PM CET 
//


package org.cern.cms.csc.dw.model;

import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
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
import org.jvnet.hyperjaxb3.item.Item;
import org.jvnet.hyperjaxb3.item.ItemUtils;
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
 *         &lt;element name="type" type="{http://www.cern.ch/cms/csc/dw/model}componentClass"/>
 *         &lt;element name="id" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="synonym" type="{http://www.w3.org/2001/XMLSchema}string" maxOccurs="unbounded" minOccurs="0"/>
 *         &lt;element name="link" type="{http://www.cern.ch/cms/csc/dw/model}componentLinkType" maxOccurs="unbounded" minOccurs="0"/>
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
    "type",
    "id",
    "synonyms",
    "links"
})
@Entity(name = "org.cern.cms.csc.dw.model.Component")
@Table(name = "CDW_COMPONENTS")
@Inheritance(strategy = InheritanceType.JOINED)
public class Component
    implements Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected ComponentClass type;
    @XmlElement(required = true)
    protected String id;
    @XmlElement(name = "synonym")
    protected List<String> synonyms = new Vector<String>();
    @XmlElement(name = "link")
    protected List<ComponentLink> links = new Vector<ComponentLink>();
    protected transient List<Component.ComponentSynonymsItem> synonymsItems;

    /**
     * Gets the value of the type property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentClass }
     *     
     */
    @Basic
    @Column(name = "TYPE_", length = 255)
    @Enumerated(EnumType.STRING)
    public ComponentClass getType() {
        return type;
    }

    /**
     * Sets the value of the type property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentClass }
     *     
     */
    public void setType(ComponentClass value) {
        this.type = value;
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
    @Column(name = "ID")
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
     * {@link String }
     * 
     * 
     */
    @Transient
    public List<String> getSynonyms() {
        if (synonyms == null) {
            synonyms = new Vector<String>();
        }
        return this.synonyms;
    }

    /**
     * 
     * 
     */
    public void setSynonyms(List<String> synonyms) {
        this.synonyms = synonyms;
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
    })
    @JoinColumn(name = "LINKS_COMPONENT_ID")
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

    @OneToMany(targetEntity = org.cern.cms.csc.dw.model.Component.ComponentSynonymsItem.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "SYNONYMSITEMS_COMPONENT_ID")
    public List<Component.ComponentSynonymsItem> getSynonymsItems() {
        if (this.synonymsItems == null) {
            this.synonymsItems = new ArrayList<Component.ComponentSynonymsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.synonyms)) {
            this.synonyms = ItemUtils.wrap(this.synonyms, this.synonymsItems, Component.ComponentSynonymsItem.class);
        }
        return this.synonymsItems;
    }

    public void setSynonymsItems(List<Component.ComponentSynonymsItem> value) {
        this.synonyms = null;
        this.synonymsItems = null;
        this.synonymsItems = value;
        if (this.synonymsItems == null) {
            this.synonymsItems = new ArrayList<Component.ComponentSynonymsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.synonyms)) {
            this.synonyms = ItemUtils.wrap(this.synonyms, this.synonymsItems, Component.ComponentSynonymsItem.class);
        }
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
        equalsBuilder.append(this.getType(), that.getType());
        equalsBuilder.append(this.getId(), that.getId());
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
        hashCodeBuilder.append(this.getType());
        hashCodeBuilder.append(this.getId());
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
            ComponentClass theType;
            theType = this.getType();
            toStringBuilder.append("type", theType);
        }
        {
            String theId;
            theId = this.getId();
            toStringBuilder.append("id", theId);
        }
        {
            List<String> theSynonyms;
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

    @XmlAccessorType(XmlAccessType.FIELD)
    @Entity(name = "org.cern.cms.csc.dw.model.Component$ComponentSynonymsItem")
    @Table(name = "CDW_COMPONENT_SYNONYMS")
    @Inheritance(strategy = InheritanceType.JOINED)
    public static class ComponentSynonymsItem
        implements Item<String>
    {

        @XmlElement(name = "synonym", namespace = "http://www.cern.ch/cms/csc/dw/model")
        protected String item;
        @XmlAttribute(name = "Hjid")
        protected Long hjid;

        /**
         * Gets the value of the item property.
         * 
         * @return
         *     possible object is
         *     {@link String }
         *     
         */
        @Basic
        @Column(name = "ITEM", length = 255)
        public String getItem() {
            return item;
        }

        /**
         * Sets the value of the item property.
         * 
         * @param value
         *     allowed object is
         *     {@link String }
         *     
         */
        public void setItem(String value) {
            this.item = value;
        }

        /**
         * Gets the value of the hjid property.
         * 
         * @return
         *     possible object is
         *     {@link Long }
         *     
         */
        @Id
        @Column(name = "HJID")
        @GeneratedValue(strategy = GenerationType.AUTO)
        public Long getHjid() {
            return hjid;
        }

        /**
         * Sets the value of the hjid property.
         * 
         * @param value
         *     allowed object is
         *     {@link Long }
         *     
         */
        public void setHjid(Long value) {
            this.hjid = value;
        }

    }

}
