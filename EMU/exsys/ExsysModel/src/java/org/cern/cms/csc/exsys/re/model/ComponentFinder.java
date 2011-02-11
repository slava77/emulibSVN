//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.DiscriminatorColumn;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.OneToOne;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;
import jsf.bean.gui.annotation.Label;
import jsf.bean.gui.annotation.NoManualInput;
import jsf.bean.gui.annotation.UseInTitle;
import org.cern.cms.csc.dw.model.base.EntityBase;


/**
 * <p>Java class for componentFinderType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentFinderType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="rule" type="{http://www.cern.ch/cms/csc/exsys/re/model}ruleType" minOccurs="0"/>
 *         &lt;element name="componentClass" type="{http://www.cern.ch/cms/csc/dw/ontology}componentClassType"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "componentFinderType", propOrder = {
    "rule",
    "componentClass"
})
@XmlSeeAlso({
    RelatedComponentFinder.class,
    SimpleComponentFinder.class
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ComponentFinder")
@Table(name = "RE_COMPONENT_FINDERS")
@Inheritance(strategy = InheritanceType.SINGLE_TABLE)
@DiscriminatorColumn(name = "RCF_TYPE", length = 64)
@UseInTitle(order = 0)
public abstract class ComponentFinder
    extends EntityBase
    implements Serializable
{

    @NoManualInput
    @jsf.bean.gui.annotation.ImmutableReference
    protected org.cern.cms.csc.exsys.re.model.Rule rule;
    @XmlElement(required = true)
    @jsf.bean.gui.annotation.ImmutableReference
    @Label(description = "This tells the component finder what component type is to be resolved/accepted.", name = "Component Type")
    protected org.cern.cms.csc.dw.model.ontology.ComponentClass componentClass;
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the rule property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.exsys.re.model.Rule }
     *     
     */
    @OneToOne(targetEntity = org.cern.cms.csc.exsys.re.model.Rule.class, cascade = {
        CascadeType.ALL
    }, mappedBy = "componentFinder")
    public org.cern.cms.csc.exsys.re.model.Rule getRule() {
        return rule;
    }

    /**
     * Sets the value of the rule property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.exsys.re.model.Rule }
     *     
     */
    public void setRule(org.cern.cms.csc.exsys.re.model.Rule value) {
        this.rule = value;
    }

    @Transient
    public boolean isSetRule() {
        return (this.rule!= null);
    }

    /**
     * Gets the value of the componentClass property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.ontology.ComponentClass }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.ComponentClass.class)
    @JoinColumn(name = "RCF_COMPONENT_CLASS_ID")
    public org.cern.cms.csc.dw.model.ontology.ComponentClass getComponentClass() {
        return componentClass;
    }

    /**
     * Sets the value of the componentClass property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.ontology.ComponentClass }
     *     
     */
    public void setComponentClass(org.cern.cms.csc.dw.model.ontology.ComponentClass value) {
        this.componentClass = value;
    }

    @Transient
    public boolean isSetComponentClass() {
        return (this.componentClass!= null);
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
    @Column(name = "RCF_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
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

    @Override
    public String toString() {
        return getClass().getSimpleName() + " for " + getComponentClass();
    }

//--simple--preserve

}
