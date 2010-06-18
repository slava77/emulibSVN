//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

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
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;
import javax.xml.datatype.XMLGregorianCalendar;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XMLGregorianCalendarAsDateTime;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XmlAdapterUtils;


/**
 * <p>Java class for factType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="factType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="time" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="component_id" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="run" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="severity" type="{http://www.cern.ch/cms/csc/dw/model}severityType" minOccurs="0"/>
 *         &lt;element name="descr" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="component" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "factType", propOrder = {
    "time",
    "componentId",
    "run",
    "severity",
    "descr",
    "component"
})
@XmlSeeAlso({
    SlidingTmbTriggerCounterFact.class,
    DqmDduFact.class,
    DqmDduInputFact.class,
    DqmCscCfebFact.class,
    LocalDAQStatusFact.class,
    CumulativeTmbTriggerCounterFact.class,
    ApplicationStatusFact.class,
    DqmCscAfebFact.class,
    DqmCscHvSegmentFact.class,
    TmbCounterFact.class,
    DqmCscFact.class,
    DqmEmuFact.class
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.Fact")
@Table(name = "CDW_FACTS")
@Inheritance(strategy = InheritanceType.TABLE_PER_CLASS)
public abstract class Fact
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    @XmlSchemaType(name = "dateTime")
    protected XMLGregorianCalendar time;
    @XmlElement(name = "component_id", required = true)
    protected String componentId;
    protected Long run;
    protected SeverityType severity;
    protected String descr;
    protected org.cern.cms.csc.dw.model.ontology.Component component;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the time property.
     * 
     * @return
     *     possible object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    @Transient
    public XMLGregorianCalendar getTime() {
        return time;
    }

    /**
     * Sets the value of the time property.
     * 
     * @param value
     *     allowed object is
     *     {@link XMLGregorianCalendar }
     *     
     */
    public void setTime(XMLGregorianCalendar value) {
        this.time = value;
    }

    @Transient
    public boolean isSetTime() {
        return (this.time!= null);
    }

    /**
     * Gets the value of the componentId property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Transient
    public String getComponentId() {
        return componentId;
    }

    /**
     * Sets the value of the componentId property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setComponentId(String value) {
        this.componentId = value;
    }

    @Transient
    public boolean isSetComponentId() {
        return (this.componentId!= null);
    }

    /**
     * Gets the value of the run property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_RUN", precision = 20, scale = 10)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Run")
    public Long getRun() {
        return run;
    }

    /**
     * Sets the value of the run property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setRun(Long value) {
        this.run = value;
    }

    @Transient
    public boolean isSetRun() {
        return (this.run!= null);
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
     * Gets the value of the descr property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_DESCR", length = 255)
    public String getDescr() {
        return descr;
    }

    /**
     * Sets the value of the descr property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDescr(String value) {
        this.descr = value;
    }

    @Transient
    public boolean isSetDescr() {
        return (this.descr!= null);
    }

    /**
     * Gets the value of the component property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.ontology.Component }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.Component.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "FCT_CMP_ID")
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Component")
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
     * Gets the value of the id property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Id
    @Column(name = "FCT_ID")
    @GeneratedValue(generator = "CDW_FCT_ID_SEQ", strategy = GenerationType.SEQUENCE)
    @SequenceGenerator(name = "CDW_FCT_ID_SEQ", sequenceName = "CDW_FCT_ID_SEQ")
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
    @Column(name = "FCT_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Time")
    public Date getTimeItem() {
        return XmlAdapterUtils.unmarshall(XMLGregorianCalendarAsDateTime.class, this.getTime());
    }

    public void setTimeItem(Date target) {
        setTime(XmlAdapterUtils.marshall(XMLGregorianCalendarAsDateTime.class, target));
    }

    @Basic
    @Column(name = "FCT_SEVERITY", length = 10)
    @org.cern.cms.csc.dw.model.annotation.OlapDimension(name = "Severity")
    public String getSeverityItem() {
        return ((this.getSeverity() == null)?null:this.getSeverity().value());
    }

    public void setSeverityItem(String target) {
        setSeverity(((target == null)?null:SeverityType.fromValue(target)));
    }
    
//--simple--preserve

    @Transient
    public org.cern.cms.csc.dw.metadata.FactMd getMetadata() {
        org.cern.cms.csc.dw.metadata.MetadataManager mm = new org.cern.cms.csc.dw.metadata.MetadataManager();
        return mm.getFactMd(this.getClass());
    }

    @Override
    public void onSave(org.cern.cms.csc.dw.dao.EntityDaoLocal eDao, boolean queued) throws org.cern.cms.csc.dw.exception.OnSaveProcessingException {
        if (queued) {
            if (component != null) {
                try {
                    component = eDao.getEntityById(org.cern.cms.csc.dw.model.ontology.Component.class, component.getId());
                } catch (org.cern.cms.csc.dw.exception.InvalidEntityClassException iecEx) {
                    throw new org.cern.cms.csc.dw.exception.OnSaveProcessingException(iecEx);
                }
            } else {
                throw new org.cern.cms.csc.dw.exception.OnSaveProcessingException(new org.cern.cms.csc.dw.exception.ComponentNotFoundException(org.cern.cms.csc.dw.exception.OntologyException.OntologySource.RELATIONAL_DATABASE, componentId));
            }
        }
    }

//--simple--preserve

}
