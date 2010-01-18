//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.01.13 at 06:44:15 PM CET 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlElementRef;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.jvnet.hyperjaxb3.item.ItemUtils;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for factCollectionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="factCollectionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="source" type="{http://www.cern.ch/cms/csc/dw/model}sourceType"/>
 *         &lt;element name="requestId" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element ref="{http://www.cern.ch/cms/csc/dw/model}fact" maxOccurs="unbounded"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "factCollectionType", propOrder = {
    "source",
    "request",
    "facts"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.FactCollection")
@Table(name = "CDW_FACT_COLLECTIONS")
@Inheritance(strategy = InheritanceType.JOINED)
public class FactCollection
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected SourceType source;
    @XmlElement(name = "requestId")
    protected Long request;
    @XmlElementRef(name = "fact", namespace = "http://www.cern.ch/cms/csc/dw/model", type = JAXBElement.class)
    protected List<JAXBElement<? extends Fact>> facts = new Vector<JAXBElement<? extends Fact>>();
    @XmlAttribute(name = "Id")
    protected Long id;
    protected transient List<FactCollectionFactsItem> factsItems;

    /**
     * Gets the value of the source property.
     * 
     * @return
     *     possible object is
     *     {@link SourceType }
     *     
     */
    @Transient
    public SourceType getSource() {
        return source;
    }

    /**
     * Sets the value of the source property.
     * 
     * @param value
     *     allowed object is
     *     {@link SourceType }
     *     
     */
    public void setSource(SourceType value) {
        this.source = value;
    }

    @Transient
    public boolean isSetSource() {
        return (this.source!= null);
    }

    /**
     * Gets the value of the request property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCO_REQUEST_ID", precision = 20, scale = 10)
    public Long getRequest() {
        return request;
    }

    /**
     * Sets the value of the request property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setRequest(Long value) {
        this.request = value;
    }

    @Transient
    public boolean isSetRequest() {
        return (this.request!= null);
    }

    /**
     * Gets the value of the facts property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the facts property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getFacts().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link JAXBElement }{@code <}{@link CumulativeTmbTriggerCounterFact }{@code >}
     * {@link JAXBElement }{@code <}{@link SlidingTmbTriggerCounterFact }{@code >}
     * {@link JAXBElement }{@code <}{@link Fact }{@code >}
     * 
     * 
     */
    @Transient
    public List<JAXBElement<? extends Fact>> getFacts() {
        if (facts == null) {
            facts = new Vector<JAXBElement<? extends Fact>>();
        }
        return this.facts;
    }

    /**
     * 
     * 
     */
    public void setFacts(List<JAXBElement<? extends Fact>> facts) {
        this.facts = facts;
    }

    @Transient
    public boolean isSetFacts() {
        return ((this.facts!= null)&&(!this.facts.isEmpty()));
    }

    public void unsetFacts() {
        this.facts = null;
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
    @Column(name = "FCO_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "CDW_FCO_ID_SEQ", sequenceName = "CDW_FCO_ID_SEQ", initialValue = 1)
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

    @Basic
    @Column(name = "FCO_SOURCE", length = 255)
    public String getSourceItem() {
        return ((this.getSource() == null)?null:this.getSource().value());
    }

    public void setSourceItem(String target) {
        setSource(((target == null)?null:SourceType.fromValue(target)));
    }

    @OneToMany(targetEntity = FactCollectionFactsItem.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "FIT_FCO_ID")
    public List<FactCollectionFactsItem> getFactsItems() {
        if (this.factsItems == null) {
            this.factsItems = new ArrayList<FactCollectionFactsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.facts)) {
            this.facts = ItemUtils.wrap(this.facts, this.factsItems, FactCollectionFactsItem.class);
        }
        return this.factsItems;
    }

    public void setFactsItems(List<FactCollectionFactsItem> value) {
        this.facts = null;
        this.factsItems = null;
        this.factsItems = value;
        if (this.factsItems == null) {
            this.factsItems = new ArrayList<FactCollectionFactsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.facts)) {
            this.facts = ItemUtils.wrap(this.facts, this.factsItems, FactCollectionFactsItem.class);
        }
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof FactCollection)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final FactCollection that = ((FactCollection) object);
        equalsBuilder.append(this.getSource(), that.getSource());
        equalsBuilder.append(this.getRequest(), that.getRequest());
        equalsBuilder.append(this.getFacts(), that.getFacts());
    }

    public boolean equals(Object object) {
        if (!(object instanceof FactCollection)) {
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
        hashCodeBuilder.append(this.getSource());
        hashCodeBuilder.append(this.getRequest());
        hashCodeBuilder.append(this.getFacts());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            SourceType theSource;
            theSource = this.getSource();
            toStringBuilder.append("source", theSource);
        }
        {
            Long theRequest;
            theRequest = this.getRequest();
            toStringBuilder.append("request", theRequest);
        }
        {
            List<JAXBElement<? extends Fact>> theFacts;
            theFacts = this.getFacts();
            toStringBuilder.append("facts", theFacts);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
