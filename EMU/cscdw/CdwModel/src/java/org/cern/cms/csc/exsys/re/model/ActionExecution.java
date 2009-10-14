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
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XMLGregorianCalendarAsDateTime;
import org.jvnet.hyperjaxb3.xml.bind.annotation.adapters.XmlAdapterUtils;


/**
 * <p>Java class for actionExecutionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="actionExecutionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="timestamp" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="action" type="{http://www.cern.ch/cms/csc/exsys/re/model}actionType"/>
 *         &lt;element name="trigger" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTriggerType"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "actionExecutionType", propOrder = {
    "timestamp",
    "action",
    "trigger"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ActionExecution")
@Table(name = "RE_ACTION_EXECUTIONS")
@Inheritance(strategy = InheritanceType.JOINED)
public class ActionExecution
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    @XmlSchemaType(name = "dateTime")
    protected XMLGregorianCalendar timestamp;
    @XmlElement(required = true)
    protected Action action;
    @XmlElement(required = true)
    protected ConclusionTrigger trigger;
    @XmlAttribute(name = "id")
    protected Long id;

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
     * Gets the value of the action property.
     * 
     * @return
     *     possible object is
     *     {@link Action }
     *     
     */
    @ManyToOne(targetEntity = Action.class)
    @JoinColumn(name = "REAE_ACTION_ID", nullable = false)
    public Action getAction() {
        return action;
    }

    /**
     * Sets the value of the action property.
     * 
     * @param value
     *     allowed object is
     *     {@link Action }
     *     
     */
    public void setAction(Action value) {
        this.action = value;
    }

    @Transient
    public boolean isSetAction() {
        return (this.action!= null);
    }

    /**
     * Gets the value of the trigger property.
     * 
     * @return
     *     possible object is
     *     {@link ConclusionTrigger }
     *     
     */
    @ManyToOne(targetEntity = ConclusionTrigger.class)
    @JoinColumn(name = "REAE_TRIGGER_ID", nullable = false)
    public ConclusionTrigger getTrigger() {
        return trigger;
    }

    /**
     * Sets the value of the trigger property.
     * 
     * @param value
     *     allowed object is
     *     {@link ConclusionTrigger }
     *     
     */
    public void setTrigger(ConclusionTrigger value) {
        this.trigger = value;
    }

    @Transient
    public boolean isSetTrigger() {
        return (this.trigger!= null);
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
    @Column(name = "REAE_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_ACTION_EXECUTION_ID_SEQ", sequenceName = "RE_ACTION_EXECUTION_ID_SEQ")
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
    @Column(name = "REAE_TIMESTAMP")
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimestampItem() {
        return XmlAdapterUtils.unmarshall(XMLGregorianCalendarAsDateTime.class, this.getTimestamp());
    }

    public void setTimestampItem(Date target) {
        setTimestamp(XmlAdapterUtils.marshall(XMLGregorianCalendarAsDateTime.class, target));
    }

}