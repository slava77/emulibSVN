//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.util.List;
import java.util.Vector;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.DiscriminatorColumn;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;
import jsf.bean.gui.annotation.ImmutableReference;
import jsf.bean.gui.annotation.NoManualInput;
import jsf.bean.gui.annotation.UseInTitle;
import org.cern.cms.csc.dw.model.base.EntityBase;


/**
 * <p>Java class for actionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="actionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="name" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="conclusionTypes" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTypeType" maxOccurs="unbounded" minOccurs="0"/>
 *         &lt;element name="triggerType" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTriggerTypeType"/>
 *         &lt;element name="isDeleted" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="isEnabled" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="timeout" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="acknowledgeConclusionOnExecution" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="acknowledgeConclusionOnCreate" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "actionType", propOrder = {
    "name",
    "conclusionTypes",
    "triggerType",
    "deleted",
    "enabled",
    "timeout",
    "acknowledgeConclusionOnExecution",
    "acknowledgeConclusionOnCreate"
})
@XmlSeeAlso({
    NotificationAction.class
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.Action")
@Table(name = "RE_ACTIONS")
@Inheritance(strategy = InheritanceType.JOINED)
@DiscriminatorColumn(name = "REA_TYPE", length = 64)
public abstract class Action
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    @UseInTitle(order = 1)
    protected String name;
    @ImmutableReference
    protected List<org.cern.cms.csc.exsys.re.model.ConclusionType> conclusionTypes = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionType>();
    @XmlElement(required = true)
    protected ConclusionTriggerType triggerType;
    @XmlElement(name = "isDeleted")
    @NoManualInput(createDefaultValue = true)
    protected boolean deleted;
    @XmlElement(name = "isEnabled")
    protected boolean enabled;
    protected long timeout;
    protected boolean acknowledgeConclusionOnExecution;
    protected boolean acknowledgeConclusionOnCreate;
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
    @Column(name = "REA_NAME", nullable = false, length = 128)
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
     * Gets the value of the conclusionTypes property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the conclusionTypes property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getConclusionTypes().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link org.cern.cms.csc.exsys.re.model.ConclusionType }
     * 
     * 
     */
    @ManyToMany(targetEntity = org.cern.cms.csc.exsys.re.model.ConclusionType.class)
    @JoinTable(name = "RE_ACTIONS_CONCLUSION_TYPES", joinColumns = {
        @JoinColumn(name = "REAC_ACTION_ID")
    }, inverseJoinColumns = {
        @JoinColumn(name = "REAC_CONCLUSION_TYPE_ID")
    })
    public List<org.cern.cms.csc.exsys.re.model.ConclusionType> getConclusionTypes() {
        if (conclusionTypes == null) {
            conclusionTypes = new Vector<org.cern.cms.csc.exsys.re.model.ConclusionType>();
        }
        return this.conclusionTypes;
    }

    /**
     * 
     * 
     */
    public void setConclusionTypes(List<org.cern.cms.csc.exsys.re.model.ConclusionType> conclusionTypes) {
        this.conclusionTypes = conclusionTypes;
    }

    @Transient
    public boolean isSetConclusionTypes() {
        return ((this.conclusionTypes!= null)&&(!this.conclusionTypes.isEmpty()));
    }

    public void unsetConclusionTypes() {
        this.conclusionTypes = null;
    }

    /**
     * Gets the value of the triggerType property.
     * 
     * @return
     *     possible object is
     *     {@link ConclusionTriggerType }
     *     
     */
    @Transient
    public ConclusionTriggerType getTriggerType() {
        return triggerType;
    }

    /**
     * Sets the value of the triggerType property.
     * 
     * @param value
     *     allowed object is
     *     {@link ConclusionTriggerType }
     *     
     */
    public void setTriggerType(ConclusionTriggerType value) {
        this.triggerType = value;
    }

    @Transient
    public boolean isSetTriggerType() {
        return (this.triggerType!= null);
    }

    /**
     * Gets the value of the deleted property.
     * 
     */
    @Basic
    @Column(name = "REA_IS_DELETED", nullable = false)
    public boolean isDeleted() {
        return deleted;
    }

    /**
     * Sets the value of the deleted property.
     * 
     */
    public void setDeleted(boolean value) {
        this.deleted = value;
    }

    @Transient
    public boolean isSetDeleted() {
        return true;
    }

    /**
     * Gets the value of the enabled property.
     * 
     */
    @Basic
    @Column(name = "REA_IS_ENABLED", nullable = false)
    public boolean isEnabled() {
        return enabled;
    }

    /**
     * Sets the value of the enabled property.
     * 
     */
    public void setEnabled(boolean value) {
        this.enabled = value;
    }

    @Transient
    public boolean isSetEnabled() {
        return true;
    }

    /**
     * Gets the value of the timeout property.
     * 
     */
    @Basic
    @Column(name = "REA_TIMEOUT", nullable = false, precision = 20, scale = 10)
    public long getTimeout() {
        return timeout;
    }

    /**
     * Sets the value of the timeout property.
     * 
     */
    public void setTimeout(long value) {
        this.timeout = value;
    }

    @Transient
    public boolean isSetTimeout() {
        return true;
    }

    /**
     * Gets the value of the acknowledgeConclusionOnExecution property.
     * 
     */
    @Basic
    @Column(name = "REA_ACK_CONCL_ON_EXEC", nullable = false)
    public boolean isAcknowledgeConclusionOnExecution() {
        return acknowledgeConclusionOnExecution;
    }

    /**
     * Sets the value of the acknowledgeConclusionOnExecution property.
     * 
     */
    public void setAcknowledgeConclusionOnExecution(boolean value) {
        this.acknowledgeConclusionOnExecution = value;
    }

    @Transient
    public boolean isSetAcknowledgeConclusionOnExecution() {
        return true;
    }

    /**
     * Gets the value of the acknowledgeConclusionOnCreate property.
     * 
     */
    @Basic
    @Column(name = "REA_ACK_CONCL_ON_CREATE", nullable = false)
    public boolean isAcknowledgeConclusionOnCreate() {
        return acknowledgeConclusionOnCreate;
    }

    /**
     * Sets the value of the acknowledgeConclusionOnCreate property.
     * 
     */
    public void setAcknowledgeConclusionOnCreate(boolean value) {
        this.acknowledgeConclusionOnCreate = value;
    }

    @Transient
    public boolean isSetAcknowledgeConclusionOnCreate() {
        return true;
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
    @Column(name = "REA_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_ACTION_ID_SEQ", sequenceName = "RE_ACTION_ID_SEQ")
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
    @Column(name = "REA_TRIGGER_TYPE", nullable = false, length = 6)
    public String getTriggerTypeItem() {
        return ((this.getTriggerType() == null)?null:this.getTriggerType().value());
    }

    public void setTriggerTypeItem(String target) {
        setTriggerType(((target == null)?null:ConclusionTriggerType.fromValue(target)));
    }
    
//--simple--preserve

    @Override
    @Transient
    public String getEntityTitle() {
        return getName();
    }

//--simple--preserve

}
