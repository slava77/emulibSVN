//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.04.29 at 09:33:27 PM CEST 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.math.BigInteger;
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
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.SeverityType;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for conclusionFactoryType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionFactoryType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="conclusionName" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="conclusionTitle" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="conclusionDescription" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="conclusionSeverity" type="{http://www.cern.ch/cms/csc/dw/model}severityType"/>
 *         &lt;element name="conclusionCertainty" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="actionFactories" type="{http://www.cern.ch/cms/csc/exsys/re/model}actionFactoryType" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "conclusionFactoryType", propOrder = {
    "conclusionName",
    "conclusionTitle",
    "description",
    "conclusionSeverity",
    "conclusionCertainty",
    "actionFactories"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ConclusionFactory")
@Table(name = "RE_CONCLUSION_FACTORIES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "RECF_CONCL_NAME"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
public class ConclusionFactory
    extends EntityBase
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected String conclusionName;
    @XmlElement(required = true)
    protected String conclusionTitle;
    @XmlElement(name = "conclusionDescription", required = true)
    protected String description;
    @XmlElement(required = true)
    protected SeverityType conclusionSeverity;
    @XmlElement(required = true)
    protected BigInteger conclusionCertainty;
    protected List<ActionFactory> actionFactories = new Vector<ActionFactory>();
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the conclusionName property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RECF_CONCL_NAME", length = 64)
    public String getConclusionName() {
        return conclusionName;
    }

    /**
     * Sets the value of the conclusionName property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setConclusionName(String value) {
        this.conclusionName = value;
    }

    @Transient
    public boolean isSetConclusionName() {
        return (this.conclusionName!= null);
    }

    /**
     * Gets the value of the conclusionTitle property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RECF_CONCL_TITLE", length = 512)
    public String getConclusionTitle() {
        return conclusionTitle;
    }

    /**
     * Sets the value of the conclusionTitle property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setConclusionTitle(String value) {
        this.conclusionTitle = value;
    }

    @Transient
    public boolean isSetConclusionTitle() {
        return (this.conclusionTitle!= null);
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
    @Column(name = "RECF_CONCL_DESCRIPTION", length = 2048)
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
     * Gets the value of the conclusionSeverity property.
     * 
     * @return
     *     possible object is
     *     {@link SeverityType }
     *     
     */
    @Transient
    public SeverityType getConclusionSeverity() {
        return conclusionSeverity;
    }

    /**
     * Sets the value of the conclusionSeverity property.
     * 
     * @param value
     *     allowed object is
     *     {@link SeverityType }
     *     
     */
    public void setConclusionSeverity(SeverityType value) {
        this.conclusionSeverity = value;
    }

    @Transient
    public boolean isSetConclusionSeverity() {
        return (this.conclusionSeverity!= null);
    }

    /**
     * Gets the value of the conclusionCertainty property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "RECF_CONCL_CERTAINTY", precision = 3, scale = 0)
    public BigInteger getConclusionCertainty() {
        return conclusionCertainty;
    }

    /**
     * Sets the value of the conclusionCertainty property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setConclusionCertainty(BigInteger value) {
        this.conclusionCertainty = value;
    }

    @Transient
    public boolean isSetConclusionCertainty() {
        return (this.conclusionCertainty!= null);
    }

    /**
     * Gets the value of the actionFactories property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the actionFactories property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getActionFactories().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link ActionFactory }
     * 
     * 
     */
    @OneToMany(targetEntity = ActionFactory.class, cascade = {
        CascadeType.ALL
    }, mappedBy = "conclusionFactory")
    @JoinColumn(name = "ACTIONFACTORIES_CONCLUSIONFA_0")
    public List<ActionFactory> getActionFactories() {
        if (actionFactories == null) {
            actionFactories = new Vector<ActionFactory>();
        }
        return this.actionFactories;
    }

    /**
     * 
     * 
     */
    public void setActionFactories(List<ActionFactory> actionFactories) {
        this.actionFactories = actionFactories;
    }

    @Transient
    public boolean isSetActionFactories() {
        return ((this.actionFactories!= null)&&(!this.actionFactories.isEmpty()));
    }

    public void unsetActionFactories() {
        this.actionFactories = null;
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
    @Column(name = "RECF_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCL_FACTORY_ID_SEQ", sequenceName = "RE_CONCL_FACTORY_ID_SEQ")
    public Long getid() {
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
    public void setid(Long value) {
        this.id = value;
    }

    @Basic
    @Column(name = "REC_CONCL_SEVERITY", length = 10)
    public String getConclusionSeverityItem() {
        return ((this.getConclusionSeverity() == null)?null:this.getConclusionSeverity().value());
    }

    public void setConclusionSeverityItem(String target) {
        setConclusionSeverity(((target == null)?null:SeverityType.fromValue(target)));
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof ConclusionFactory)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final ConclusionFactory that = ((ConclusionFactory) object);
        equalsBuilder.append(this.getConclusionName(), that.getConclusionName());
        equalsBuilder.append(this.getConclusionTitle(), that.getConclusionTitle());
        equalsBuilder.append(this.getDescription(), that.getDescription());
        equalsBuilder.append(this.getConclusionSeverity(), that.getConclusionSeverity());
        equalsBuilder.append(this.getConclusionCertainty(), that.getConclusionCertainty());
        equalsBuilder.append(this.getActionFactories(), that.getActionFactories());
    }

    public boolean equals(Object object) {
        if (!(object instanceof ConclusionFactory)) {
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
        hashCodeBuilder.append(this.getConclusionName());
        hashCodeBuilder.append(this.getConclusionTitle());
        hashCodeBuilder.append(this.getDescription());
        hashCodeBuilder.append(this.getConclusionSeverity());
        hashCodeBuilder.append(this.getConclusionCertainty());
        hashCodeBuilder.append(this.getActionFactories());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            String theConclusionName;
            theConclusionName = this.getConclusionName();
            toStringBuilder.append("conclusionName", theConclusionName);
        }
        {
            String theConclusionTitle;
            theConclusionTitle = this.getConclusionTitle();
            toStringBuilder.append("conclusionTitle", theConclusionTitle);
        }
        {
            String theDescription;
            theDescription = this.getDescription();
            toStringBuilder.append("description", theDescription);
        }
        {
            SeverityType theConclusionSeverity;
            theConclusionSeverity = this.getConclusionSeverity();
            toStringBuilder.append("conclusionSeverity", theConclusionSeverity);
        }
        {
            BigInteger theConclusionCertainty;
            theConclusionCertainty = this.getConclusionCertainty();
            toStringBuilder.append("conclusionCertainty", theConclusionCertainty);
        }
        {
            List<ActionFactory> theActionFactories;
            theActionFactories = this.getActionFactories();
            toStringBuilder.append("actionFactories", theActionFactories);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
