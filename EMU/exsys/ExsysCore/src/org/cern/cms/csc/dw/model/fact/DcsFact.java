//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.ComponentDcsId;


/**
 * <p>Java class for dcsFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="dcsFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="component_attribute" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *         &lt;element name="component_dcs_id" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "dcsFactType", propOrder = {
    "componentAttribute",
    "componentDcsId"
})
@XmlSeeAlso({
    DcsAlertFact.class,
    DcsDisableEnableFact.class,
    DcsHVOnVoltageFact.class
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DcsFact")
@Table(name = "CDW_DCS_FACTS")
public abstract class DcsFact
    extends Fact
    implements Serializable
{

    @XmlElement(name = "component_attribute")
    protected String componentAttribute;
    @XmlElement(name = "component_dcs_id")
    protected String componentDcsId;

    /**
     * Gets the value of the componentAttribute property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_CMP_ATTR", length = 255)
    public String getComponentAttribute() {
        return componentAttribute;
    }

    /**
     * Sets the value of the componentAttribute property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setComponentAttribute(String value) {
        this.componentAttribute = value;
    }

    @Transient
    public boolean isSetComponentAttribute() {
        return (this.componentAttribute!= null);
    }

    /**
     * Gets the value of the componentDcsId property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Transient
    @ComponentDcsId(componentAttributeProperty = "componentAttribute", componentProperty = "component", required = false)
    public String getComponentDcsId() {
        return componentDcsId;
    }

    /**
     * Sets the value of the componentDcsId property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setComponentDcsId(String value) {
        this.componentDcsId = value;
    }

    @Transient
    public boolean isSetComponentDcsId() {
        return (this.componentDcsId!= null);
    }

}