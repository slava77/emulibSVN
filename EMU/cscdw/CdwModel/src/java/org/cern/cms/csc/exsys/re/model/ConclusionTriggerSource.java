//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
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
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.base.EntityBase;


/**
 * <p>Java class for conclusionTriggerSourceType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionTriggerSourceType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="trigger" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionTriggerType"/>
 *         &lt;choice>
 *           &lt;element name="conclusion" type="{http://www.cern.ch/cms/csc/exsys/re/model}conclusionType"/>
 *           &lt;element name="fact" type="{http://www.cern.ch/cms/csc/dw/model}factType"/>
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
@XmlType(name = "conclusionTriggerSourceType", propOrder = {
    "trigger",
    "conclusion",
    "fact"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ConclusionTriggerSource")
@Table(name = "RE_CONCLUSION_TRIG_SOURCES")
@Inheritance(strategy = InheritanceType.JOINED)
public class ConclusionTriggerSource
    extends EntityBase
    implements Serializable
{

    @XmlElement(required = true)
    protected ConclusionTrigger trigger;
    protected org.cern.cms.csc.exsys.re.model.Conclusion conclusion;
    protected org.cern.cms.csc.dw.model.fact.Fact fact;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the trigger property.
     * 
     * @return
     *     possible object is
     *     {@link ConclusionTrigger }
     *     
     */
    @ManyToOne(targetEntity = ConclusionTrigger.class)
    @JoinColumn(name = "RECS_TRIGGER_ID", nullable = false)
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
     * Gets the value of the conclusion property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.exsys.re.model.Conclusion.class)
    @JoinColumn(name = "RECS_CONCLUSION_ID")
    public org.cern.cms.csc.exsys.re.model.Conclusion getConclusion() {
        return conclusion;
    }

    /**
     * Sets the value of the conclusion property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.Conclusion }
     *     
     */
    public void setConclusion(org.cern.cms.csc.exsys.re.model.Conclusion value) {
        this.conclusion = value;
    }

    @Transient
    public boolean isSetConclusion() {
        return (this.conclusion!= null);
    }

    /**
     * Gets the value of the fact property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.fact.Fact }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.fact.Fact.class)
    @JoinColumn(name = "RECS_FACT_ID")
    public org.cern.cms.csc.dw.model.fact.Fact getFact() {
        return fact;
    }

    /**
     * Sets the value of the fact property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.fact.Fact }
     *     
     */
    public void setFact(org.cern.cms.csc.dw.model.fact.Fact value) {
        this.fact = value;
    }

    @Transient
    public boolean isSetFact() {
        return (this.fact!= null);
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
    @Column(name = "RECS_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCL_TRIG_SOURCE_ID_SEQ", sequenceName = "RE_CONCL_TRIG_SOURCE_ID_SEQ")
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
