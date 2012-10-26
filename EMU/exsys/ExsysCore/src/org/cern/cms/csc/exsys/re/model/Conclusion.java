//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.math.BigInteger;
import java.util.Date;
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
import javax.persistence.ManyToOne;
import javax.persistence.OneToMany;
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
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;
import org.cern.cms.csc.dw.model.annotation.OlapDimensionSetter;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.SeverityType;
import org.w3._2001.xmlschema.Adapter1;


/**
 * <p>Java class for conclusionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="component" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType"/>
 *         &lt;element name="title" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="description" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="severity" type="{http://www.cern.ch/cms/csc/dw/model}severityType"/>
 *         &lt;element name="timestamp" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="lastHitTime" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="hitCount" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="isClosed" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isAcknowledged" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="timeClosed" type="{http://www.w3.org/2001/XMLSchema}dateTime" minOccurs="0"/>
 *         &lt;element name="type" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTypeType"/>
 *         &lt;element name="triggers" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTriggerType" maxOccurs="unbounded" minOccurs="0"/>
 *         &lt;element name="parents" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTriggerSourceType" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "conclusionType", propOrder = {
    "component",
    "title",
    "description",
    "severity",
    "timestamp",
    "lastHitTime",
    "hitCount",
    "closed",
    "acknowledged",
    "timeClosed",
    "type",
    "triggers",
    "parents"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.Conclusion")
@Table(name = "RE_CONCLUSIONS")
@Inheritance(strategy = InheritanceType.JOINED)
@OlapDimensionSetter(name = "Component Location", propertyName = "component", sharedTable = "CDW_OLAP$D_COMPS_BY_LOCATION")
public class Conclusion
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    protected org.cern.cms.csc.dw.model.ontology.Component component;
    @XmlElement(required = true)
    protected String title;
    @XmlElement(required = true)
    protected String description;
    @XmlElement(required = true)
    protected SeverityType severity;
    @XmlElement(required = true, type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date timestamp;
    @XmlElement(required = true, type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date lastHitTime;
    @XmlElement(required = true)
    protected BigInteger hitCount;
    @XmlElement(name = "isClosed")
    protected boolean closed;
    @XmlElement(name = "isAcknowledged")
    protected boolean acknowledged;
    @XmlElement(type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date timeClosed;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.ConclusionType type;
    protected List<org.cern.cms.csc.exsys.re.model.ConclusionTrigger> triggers = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionTrigger>();
    protected List<org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource> parents = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource>();
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the component property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.ontology.Component }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.Component.class)
    @JoinColumn(name = "REC_COMPONENT")
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Component Type", sharedTable = "CDW_OLAP$D_COMPS_BY_TYPE")
    public org.cern.cms.csc.dw.model.ontology.Component getComponent() {
        return component;
    }

    /**
     * Sets the value of the component property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.ontology.Component }
     *     
     */
    public void setComponent(org.cern.cms.csc.dw.model.ontology.Component value) {
        this.component = value;
    }

    @Transient
    public boolean isSetComponent() {
        return (this.component!= null);
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
    @Column(name = "REC_TITLE", nullable = false, length = 512)
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
    @Column(name = "REC_DESCRIPTION", nullable = false, length = 2048)
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
     * Gets the value of the severity property.
     * 
     * @return
     *     possible object is
     *     {@link SeverityType }
     *     
     */
    @Basic
    @Column(name = "REC_SEVERITY", nullable = false, length = 10)
    @Enumerated(EnumType.STRING)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Severity")
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
     * Gets the value of the timestamp property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REC_TIMESTAMP", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Timestamp")
    public Date getTimestamp() {
        return timestamp;
    }

    /**
     * Sets the value of the timestamp property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTimestamp(Date value) {
        this.timestamp = value;
    }

    @Transient
    public boolean isSetTimestamp() {
        return (this.timestamp!= null);
    }

    /**
     * Gets the value of the lastHitTime property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REC_LAST_HIT_TIME", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    public Date getLastHitTime() {
        return lastHitTime;
    }

    /**
     * Sets the value of the lastHitTime property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setLastHitTime(Date value) {
        this.lastHitTime = value;
    }

    @Transient
    public boolean isSetLastHitTime() {
        return (this.lastHitTime!= null);
    }

    /**
     * Gets the value of the hitCount property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "REC_HIT_COUNT", nullable = false, precision = 20, scale = 10)
    public BigInteger getHitCount() {
        return hitCount;
    }

    /**
     * Sets the value of the hitCount property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setHitCount(BigInteger value) {
        this.hitCount = value;
    }

    @Transient
    public boolean isSetHitCount() {
        return (this.hitCount!= null);
    }

    /**
     * Gets the value of the closed property.
     * 
     */
    @Basic
    @Column(name = "REC_IS_CLOSED", nullable = false)
    public boolean isClosed() {
        return closed;
    }

    /**
     * Sets the value of the closed property.
     * 
     */
    public void setClosed(boolean value) {
        this.closed = value;
    }

    @Transient
    public boolean isSetClosed() {
        return true;
    }

    /**
     * Gets the value of the acknowledged property.
     * 
     */
    @Basic
    @Column(name = "REC_IS_ACKNOWLEDGED", nullable = false)
    public boolean isAcknowledged() {
        return acknowledged;
    }

    /**
     * Sets the value of the acknowledged property.
     * 
     */
    public void setAcknowledged(boolean value) {
        this.acknowledged = value;
    }

    @Transient
    public boolean isSetAcknowledged() {
        return true;
    }

    /**
     * Gets the value of the timeClosed property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REC_TIME_CLOSED")
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimeClosed() {
        return timeClosed;
    }

    /**
     * Sets the value of the timeClosed property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTimeClosed(Date value) {
        this.timeClosed = value;
    }

    @Transient
    public boolean isSetTimeClosed() {
        return (this.timeClosed!= null);
    }

    /**
     * Gets the value of the type property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionType }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionType.class)
    @JoinColumn(name = "REC_CONCLUSION_TYPE_ID", nullable = false)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Conclusion Type", sharedTable = "CDW_OLAP$D_CONCLUSION_TYPES")
    public org.cern.cms.csc.exsys.re.model.ConclusionType getType() {
        return type;
    }

    /**
     * Sets the value of the type property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionType }
     *     
     */
    public void setType(org.cern.cms.csc.exsys.re.model.ConclusionType value) {
        this.type = value;
    }

    @Transient
    public boolean isSetType() {
        return (this.type!= null);
    }

    /**
     * Gets the value of the triggers property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the triggers property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getTriggers().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link org.cern.cms.csc.exsys.re.model.ConclusionTrigger }
     * 
     * 
     */
    @OneToMany(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionTrigger.class, cascade = {
        CascadeType.REMOVE,
        CascadeType.PERSIST
    }, mappedBy = "conclusion")
    public List<org.cern.cms.csc.exsys.re.model.ConclusionTrigger> getTriggers() {
        if (triggers == null) {
            triggers = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionTrigger>();
        }
        return this.triggers;
    }

    /**
     * 
     * 
     */
    public void setTriggers(List<org.cern.cms.csc.exsys.re.model.ConclusionTrigger> triggers) {
        this.triggers = triggers;
    }

    @Transient
    public boolean isSetTriggers() {
        return ((this.triggers!= null)&&(!this.triggers.isEmpty()));
    }

    public void unsetTriggers() {
        this.triggers = null;
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
     * {@link org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource }
     * 
     * 
     */
    @OneToMany(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource.class, mappedBy = "conclusion")
    public List<org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource> getParents() {
        if (parents == null) {
            parents = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource>();
        }
        return this.parents;
    }

    /**
     * 
     * 
     */
    public void setParents(List<org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource> parents) {
        this.parents = parents;
    }

    @Transient
    public boolean isSetParents() {
        return ((this.parents!= null)&&(!this.parents.isEmpty()));
    }

    public void unsetParents() {
        this.parents = null;
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
    @Column(name = "REC_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCLUSION_ID_SEQ", sequenceName = "RE_CONCLUSION_ID_SEQ")
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
    
//--simple--preserve

    public String debugPrint() {
        return debugPrint(true);
    }

    public String debugPrint(boolean printTriggers) {
        StringBuilder ret = new StringBuilder();
        ret.append("Conclusion ID=");
        ret.append(getid());
        ret.append(", type={");
        ret.append(getType().toString());
        ret.append("}, title=");
        ret.append(getTitle());
        ret.append("}, component=");
        ret.append(getComponent().getName());

        if (printTriggers) {
            ret.append(", number of parents=");
            ret.append(getParents().size());
            ret.append(", number of triggers=");
            ret.append(getTriggers().size());
            ret.append("\n");

            for (ConclusionTrigger trig: getTriggers()) {
                ret.append("   Trigger: " + trig.toString());
            }
        }

        return ret.toString();
    }

    @Override
    public String toString() {
        return getTitle();
    }

    public String toStringDetailed() {
        StringBuilder ret = new StringBuilder();
        ret.append("Conclusion details:");
        ret.append("\n");
        ret.append("    - State: ");
        if (isClosed()) {
            ret.append("CLOSED");
        } else {
            ret.append("OPEN");
        }
        if (isAcknowledged()) {
            ret.append(" (acknowledged)");
        } else {
            ret.append(" (not acknowledged)");
        }
        ret.append("\n");
        ret.append("    - Type: ");
        ret.append(getType().getName());
        ret.append("\n");
        ret.append("    - Component: ");
        ret.append(getComponent().getName());
        ret.append("\n");
        ret.append("    - Severity: ");
        ret.append(getSeverity());
        ret.append("\n");
        ret.append("    - Title: ");
        ret.append(getTitle());
        ret.append("\n");
        ret.append("    - Description: ");
        ret.append(getTitle());
        ret.append("\n");
        ret.append("    - Time created: ");
        ret.append(getTimestamp());
        ret.append("\n");
        ret.append("    - Time of last update: ");
        ret.append(getLastHitTime());
        ret.append("\n");
        ret.append("    - Number of updates: ");
        ret.append(getHitCount());

        return ret.toString();
    }

//--simple--preserve

}
