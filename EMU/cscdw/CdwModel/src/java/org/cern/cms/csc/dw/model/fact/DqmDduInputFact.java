//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.21 at 05:10:42 PM CEST 
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
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.FactAnn;


/**
 * <p>Java class for DqmDduInputFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmDduInputFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="DDU_INPUT_IN_ERROR_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_INPUT_IN_WARNING_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="DDU_NO_INPUT_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmDduInputFactType", propOrder = {
    "online",
    "inputInErrorState",
    "inputInWarningState",
    "inputNoData"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmDduInputFact")
@Table(name = "CDW_DQM_DDU_INPUT_FACTS")
@FactAnn(limitComponents = {
    "DDUInput"
})
public class DqmDduInputFact
    extends Fact
    implements Serializable
{

    protected boolean online;
    @XmlElement(name = "DDU_INPUT_IN_ERROR_STATE")
    protected Long inputInErrorState;
    @XmlElement(name = "DDU_INPUT_IN_WARNING_STATE")
    protected Long inputInWarningState;
    @XmlElement(name = "DDU_NO_INPUT_DATA")
    protected Long inputNoData;

    /**
     * Gets the value of the online property.
     * 
     */
    @Basic
    @Column(name = "FCT_ONLINE")
    public boolean isOnline() {
        return online;
    }

    /**
     * Sets the value of the online property.
     * 
     */
    public void setOnline(boolean value) {
        this.online = value;
    }

    @Transient
    public boolean isSetOnline() {
        return true;
    }

    /**
     * Gets the value of the inputInErrorState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_INPUT_IN_ERROR_STATE", precision = 20, scale = 10)
    public Long getInputInErrorState() {
        return inputInErrorState;
    }

    /**
     * Sets the value of the inputInErrorState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputInErrorState(Long value) {
        this.inputInErrorState = value;
    }

    @Transient
    public boolean isSetInputInErrorState() {
        return (this.inputInErrorState!= null);
    }

    /**
     * Gets the value of the inputInWarningState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_INPUT_IN_WARN_STATE", precision = 20, scale = 10)
    public Long getInputInWarningState() {
        return inputInWarningState;
    }

    /**
     * Sets the value of the inputInWarningState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputInWarningState(Long value) {
        this.inputInWarningState = value;
    }

    @Transient
    public boolean isSetInputInWarningState() {
        return (this.inputInWarningState!= null);
    }

    /**
     * Gets the value of the inputNoData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_NO_INPUT_DATA", precision = 20, scale = 10)
    public Long getInputNoData() {
        return inputNoData;
    }

    /**
     * Sets the value of the inputNoData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setInputNoData(Long value) {
        this.inputNoData = value;
    }

    @Transient
    public boolean isSetInputNoData() {
        return (this.inputNoData!= null);
    }

}
