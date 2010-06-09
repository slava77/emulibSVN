//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.21 at 05:10:42 PM CEST 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
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
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;
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
 * <p>Java class for conclusionSourceRelationType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionSourceRelationType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="isClosing" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="timestamp" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="parent" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionType"/>
 *         &lt;choice>
 *           &lt;element name="childConclusion" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionType"/>
 *           &lt;element name="childFact" type="{http://www.cern.ch/cms/csc/dw/model}factType"/>
 *         &lt;/choice>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "conclusionSourceRelationType", propOrder = {
    "isClosing",
    "timestamp",
    "parent",
    "childConclusion",
    "childFact"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ConclusionSourceRelation")
@Table(name = "RE_CONCLUSION_SOURCE_RELATIONS")
@Inheritance(strategy = InheritanceType.JOINED)
public class ConclusionSourceRelation
    implements Serializable
{

    protected boolean isClosing;
    @XmlElement(required = true)
    @XmlSchemaType(name = "dateTime")
    protected XMLGregorianCalendar timestamp;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.Conclusion parent;
    protected org.cern.cms.csc.exsys.re.model.Conclusion childConclusion;
    protected org.cern.cms.csc.dw.model.fact.Fact childFact;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the isClosing property.
     * 
     */
    @Basic
    @Column(name = "REC_IS_CLOSING", nullable = false)
    public boolean isIsClosing() {
        return isClosing;
    }

    /**
     * Sets the value of the isClosing property.
     * 
     */
    public void setIsClosing(boolean value) {
        this.isClosing = value;
    }

    @Transient
    public boolean isSetIsClosing() {
        return true;
    }

    /**
     * Gets the value of the timestamp property.
     * 
     * @return
     *     possible object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    @Transient
    public XMLGregorianCalendar getTimestamp() {
        return timestamp;
    }

    /**
     * Sets the value of the timestamp property.
     * 
     * @param value
     *     allowed object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    public void setTimestamp(XMLGregorianCalendar value) {
        this.timestamp = value;
    }

    @Transient
    public boolean isSetTimestamp() {
        return (this.timestamp!= null);
    }

    /**
     * Gets the value of the parent property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.Conclusion.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "RECR_PARENT_ID", nullable = false)
    public org.cern.cms.csc.exsys.re.model.Conclusion getParent() {
        return parent;
    }

    /**
     * Sets the value of the parent property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    public void setParent(org.cern.cms.csc.exsys.re.model.Conclusion value) {
        this.parent = value;
    }

    @Transient
    public boolean isSetParent() {
        return (this.parent!= null);
    }

    /**
     * Gets the value of the childConclusion property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.Conclusion.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "RECR_CHILD_CONCLUSION_ID")
    public org.cern.cms.csc.exsys.re.model.Conclusion getChildConclusion() {
        return childConclusion;
    }

    /**
     * Sets the value of the childConclusion property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    public void setChildConclusion(org.cern.cms.csc.exsys.re.model.Conclusion value) {
        this.childConclusion = value;
    }

    @Transient
    public boolean isSetChildConclusion() {
        return (this.childConclusion!= null);
    }

    /**
     * Gets the value of the childFact property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.fact.Fact }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.fact.Fact.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "RECR_CHILD_FACT_ID")
    public org.cern.cms.csc.dw.model.fact.Fact getChildFact() {
        return childFact;
    }

    /**
     * Sets the value of the childFact property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.fact.Fact }
     *     
     */
    public void setChildFact(org.cern.cms.csc.dw.model.fact.Fact value) {
        this.childFact = value;
    }

    @Transient
    public boolean isSetChildFact() {
        return (this.childFact!= null);
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
    @Column(name = "RECR_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCL_SOURCE_REL_ID_SEQ", sequenceName = "RE_CONCL_SOURCE_REL_ID_SEQ")
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
    @Column(name = "RECR_TIMESTAMP", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimestampItem() {
        return XmlAdapterUtils.unmarshall(XMLGregorianCalendarAsDateTime.class, this.getTimestamp());
    }

    public void setTimestampItem(Date target) {
        setTimestamp(XmlAdapterUtils.marshall(XMLGregorianCalendarAsDateTime.class, target));
    }

}
