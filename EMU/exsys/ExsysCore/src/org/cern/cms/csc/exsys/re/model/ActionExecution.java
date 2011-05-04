//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
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
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.w3._2001.xmlschema.Adapter1;


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
 *         &lt;element name="timeCreated" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="timeExecuted" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="timeClosed" type="{http://www.w3.org/2001/XMLSchema}dateTime"/>
 *         &lt;element name="error" type="{http://www.w3.org/2001/XMLSchema}string"/>
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
    "timeCreated",
    "timeExecuted",
    "timeClosed",
    "error",
    "action",
    "trigger"
})
@Entity(name = "ActionExecution")
@Table(name = "RE_ACTION_EXECUTIONS")
@Inheritance(strategy = InheritanceType.JOINED)
public class ActionExecution
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true, type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date timeCreated;
    @XmlElement(required = true, type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date timeExecuted;
    @XmlElement(required = true, type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date timeClosed;
    @XmlElement(required = true)
    protected String error;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.Action action;
    @XmlElement(required = true)
    protected org.cern.cms.csc.exsys.re.model.ConclusionTrigger trigger;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the timeCreated property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REAE_TIME_CREATED", nullable = false)
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimeCreated() {
        return timeCreated;
    }

    /**
     * Sets the value of the timeCreated property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTimeCreated(Date value) {
        this.timeCreated = value;
    }

    @Transient
    public boolean isSetTimeCreated() {
        return (this.timeCreated!= null);
    }

    /**
     * Gets the value of the timeExecuted property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REAE_TIME_EXECUTED")
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTimeExecuted() {
        return timeExecuted;
    }

    /**
     * Sets the value of the timeExecuted property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTimeExecuted(Date value) {
        this.timeExecuted = value;
    }

    @Transient
    public boolean isSetTimeExecuted() {
        return (this.timeExecuted!= null);
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
    @Column(name = "REAE_TIME_CLOSED")
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
     * Gets the value of the error property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REAE_ERROR", length = 102400)
    public String getError() {
        return error;
    }

    /**
     * Sets the value of the error property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setError(String value) {
        this.error = value;
    }

    @Transient
    public boolean isSetError() {
        return (this.error!= null);
    }

    /**
     * Gets the value of the action property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.Action }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.Action.class)
    @JoinColumn(name = "REAE_ACTION_ID", nullable = false)
    public org.cern.cms.csc.exsys.re.model.Action getAction() {
        return action;
    }

    /**
     * Sets the value of the action property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.Action }
     *     
     */
    public void setAction(org.cern.cms.csc.exsys.re.model.Action value) {
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
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionTrigger }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionTrigger.class)
    @JoinColumn(name = "REAE_TRIGGER_ID", nullable = false)
    public org.cern.cms.csc.exsys.re.model.ConclusionTrigger getTrigger() {
        return trigger;
    }

    /**
     * Sets the value of the trigger property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.ConclusionTrigger }
     *     
     */
    public void setTrigger(org.cern.cms.csc.exsys.re.model.ConclusionTrigger value) {
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

}
