//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.util.Date;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
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
import org.cern.cms.csc.dw.model.annotation.gui.NoManualInput;
import org.cern.cms.csc.dw.model.annotation.gui.UseInTitle;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.SeverityType;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XMLGregorianCalendarAsDateTime;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XmlAdapterUtils;


/**
 * <p>Java class for conclusionTypeType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionTypeType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="name" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="title" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="intermediate" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="severity" type="{http://www.cern.ch/cms/csc/dw/model}severityType"/>
 *         &lt;element name="timeCreated" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "conclusionTypeType", propOrder = {
    "name",
    "title",
    "description",
    "intermediate",
    "severity",
    "timeCreated"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ConclusionType")
@Table(name = "RE_CONCLUSION_TYPES")
@Inheritance(strategy = InheritanceType.JOINED)
public class ConclusionType
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    @UseInTitle(order = 1)
    protected String name;
    @XmlElement(required = true)
    protected String title;
    @XmlElement(required = true)
    protected String description;
    protected boolean intermediate;
    @XmlElement(required = true)
    protected SeverityType severity;
    @XmlElement(required = true)
    @XmlSchemaType(name = "dateTime")
    @NoManualInput(createDefaultValue = true)
    protected XMLGregorianCalendar timeCreated;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RECT_NAME", nullable = false, length = 64)
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
     * Gets the value of the title property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RECT_TITLE", nullable = false, length = 512)
    public String getTitle() {
        return title;
    }

    /**
     * Sets the value of the title property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTitle(String value) {
        this.title = value;
    }

    @Transient
    public boolean isSetTitle() {
        return (this.title!= null);
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
    @Column(name = "RECT_DESCRIPTION", nullable = false, length = 2048)
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
     * Gets the value of the intermediate property.
     * 
     */
    @Basic
    @Column(name = "RECT_IS_INTERMEDIATE", nullable = false)
    public boolean isIntermediate() {
        return intermediate;
    }

    /**
     * Sets the value of the intermediate property.
     * 
     */
    public void setIntermediate(boolean value) {
        this.intermediate = value;
    }

    @Transient
    public boolean isSetIntermediate() {
        return true;
    }

    /**
     * Gets the value of the severity property.
     * 
     * @return
     *     possible object is
     *     {@link SeverityType }
     *     
     */
    @Transient
    public SeverityType getSeverity() {
        return severity;
    }

    /**
     * Sets the value of the severity property.
     * 
     * @param value
     *     allowed object is
     *     {@link SeverityType }
     *     
     */
    public void setSeverity(SeverityType value) {
        this.severity = value;
    }

    @Transient
    public boolean isSetSeverity() {
        return (this.severity!= null);
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
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Id
    @Column(name = "RECT_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCLUSION_TYPE_ID_SEQ", sequenceName = "RE_CONCLUSION_TYPE_ID_SEQ")
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
    @Column(name = "RECT_SEVERITY", nullable = false, length = 10)
    public String getSeverityItem() {
        return ((this.getSeverity() == null)?null:this.getSeverity().value());
    }

    public void setSeverityItem(String target) {
        setSeverity(((target == null)?null:SeverityType.fromValue(target)));
    }

    @Basic
    @Column(name = "RECT_TIME_CREATED", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimeCreatedItem() {
        return XmlAdapterUtils.unmarshall(XMLGregorianCalendarAsDateTime.class, this.getTimeCreated());
    }

    public void setTimeCreatedItem(Date target) {
        setTimeCreated(XmlAdapterUtils.marshall(XMLGregorianCalendarAsDateTime.class, target));
    }
    
//--simple--preserve

    @Override
    public boolean equals(Object obj) {
        if (obj == null) {
            return false;
        }
        if (getClass() != obj.getClass()) {
            return false;
        }
        final ConclusionType other = (ConclusionType) obj;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) {
            return false;
        }
        return true;
    }

    @Override
    public int hashCode() {
        int hash = 3;
        hash = 71 * hash + (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }

    @Override
    public String toString() {
        return "ConclusionType name=" + getName();
    }


//--simple--preserve

}
