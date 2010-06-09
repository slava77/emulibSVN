//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.21 at 05:10:42 PM CEST 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.math.BigInteger;
import java.util.Date;
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
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSchemaType;
import javax.xml.bind.annotation.XmlType;
import javax.xml.datatype.XMLGregorianCalendar;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XMLGregorianCalendarAsDateTime;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XmlAdapterUtils;


/**
 * <p>Java class for ruleType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="ruleType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="version" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="name" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="timeCreated" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="ruleDefinition" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="conclusionType" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTypeType"/>
 *         &lt;element name="conclusionFactory" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionFactoryType"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "ruleType", propOrder = {
    "version",
    "name",
    "timeCreated",
    "ruleDefinition",
    "description",
    "conclusionType",
    "conclusionFactory"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.Rule")
@Table(name = "RE_RULES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "RER_NAME",
        "RER_VERSION"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
public class Rule
    implements Serializable
{

    @XmlElement(required = true)
    protected BigInteger version;
    @XmlElement(required = true)
    protected String name;
    @XmlElement(required = true)
    @XmlSchemaType(name = "dateTime")
    protected XMLGregorianCalendar timeCreated;
    @XmlElement(required = true)
    protected String ruleDefinition;
    protected String description;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.ConclusionType conclusionType;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.ConclusionFactory conclusionFactory;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the version property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "RER_VERSION", nullable = false, precision = 20, scale = 10)
    public BigInteger getVersion() {
        return version;
    }

    /**
     * Sets the value of the version property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setVersion(BigInteger value) {
        this.version = value;
    }

    @Transient
    public boolean isSetVersion() {
        return (this.version!= null);
    }

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RER_NAME", nullable = false, length = 128)
    public String getName() {
        return name;
    }

    /**
     * Sets the value of the name property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setName(String value) {
        this.name = value;
    }

    @Transient
    public boolean isSetName() {
        return (this.name!= null);
    }

    /**
     * Gets the value of the timeCreated property.
     * 
     * @return
     *     possible object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    @Transient
    public XMLGregorianCalendar getTimeCreated() {
        return timeCreated;
    }

    /**
     * Sets the value of the timeCreated property.
     * 
     * @param value
     *     allowed object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    public void setTimeCreated(XMLGregorianCalendar value) {
        this.timeCreated = value;
    }

    @Transient
    public boolean isSetTimeCreated() {
        return (this.timeCreated!= null);
    }

    /**
     * Gets the value of the ruleDefinition property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RER_RULE_DEF", nullable = false, length = 1024)
    public String getRuleDefinition() {
        return ruleDefinition;
    }

    /**
     * Sets the value of the ruleDefinition property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setRuleDefinition(String value) {
        this.ruleDefinition = value;
    }

    @Transient
    public boolean isSetRuleDefinition() {
        return (this.ruleDefinition!= null);
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
    @Column(name = "RER_DESCRIPTION", length = 1024)
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
     * Gets the value of the conclusionType property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionType }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionType.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "RER_CONCLUSION_TYPE_ID", nullable = false)
    public org.cern.cms.csc.exsys.re.model.ConclusionType getConclusionType() {
        return conclusionType;
    }

    /**
     * Sets the value of the conclusionType property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionType }
     *     
     */
    public void setConclusionType(org.cern.cms.csc.exsys.re.model.ConclusionType value) {
        this.conclusionType = value;
    }

    @Transient
    public boolean isSetConclusionType() {
        return (this.conclusionType!= null);
    }

    /**
     * Gets the value of the conclusionFactory property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionFactory }
     *     
     */
    @OneToOne(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionFactory.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "RER_CONCLUSION_FACTORY_ID", nullable = false)
    public org.cern.cms.csc.exsys.re.model.ConclusionFactory getConclusionFactory() {
        return conclusionFactory;
    }

    /**
     * Sets the value of the conclusionFactory property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionFactory }
     *     
     */
    public void setConclusionFactory(org.cern.cms.csc.exsys.re.model.ConclusionFactory value) {
        this.conclusionFactory = value;
    }

    @Transient
    public boolean isSetConclusionFactory() {
        return (this.conclusionFactory!= null);
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
    @Column(name = "RER_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_RULE_ID_SEQ", sequenceName = "RE_RULE_ID_SEQ")
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
    @Column(name = "RER_TIME_CREATED", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimeCreatedItem() {
        return XmlAdapterUtils.unmarshall(XMLGregorianCalendarAsDateTime.class, this.getTimeCreated());
    }

    public void setTimeCreatedItem(Date target) {
        setTimeCreated(XmlAdapterUtils.marshall(XMLGregorianCalendarAsDateTime.class, target));
    }

}
