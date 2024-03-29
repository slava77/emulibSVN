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
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.FactAnn;


/**
 * <p>Java class for DqmCscAfebFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmCscAfebFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="CSC_ALCT_AFEB_NOISY" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="CSC_ALCT_NO_ANODE_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmCscAfebFactType", propOrder = {
    "online",
    "alctAfebNoisy",
    "alctNoAnodeData"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmCscAfebFact")
@Table(name = "CDW_DQM_CSC_AFEB_FACTS")
@FactAnn(limitComponents = {
    "AFEB"
}, title = "DQM AFEB")
public class DqmCscAfebFact
    extends Fact
    implements Serializable
{

    protected boolean online;
    @XmlElement(name = "CSC_ALCT_AFEB_NOISY")
    protected Long alctAfebNoisy;
    @XmlElement(name = "CSC_ALCT_NO_ANODE_DATA")
    protected Long alctNoAnodeData;

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
     * Gets the value of the alctAfebNoisy property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_ALCT_AFEB_NOISY", precision = 20, scale = 10)
    @org.cern.cms.csc.dw.model.annotation.OlapMeasure(aggregator = org.cern.cms.csc.dw.model.annotation.OlapMeasure.AggregatorType.SUM, name = "alctAfebNoisy")
    public Long getAlctAfebNoisy() {
        return alctAfebNoisy;
    }

    /**
     * Sets the value of the alctAfebNoisy property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setAlctAfebNoisy(Long value) {
        this.alctAfebNoisy = value;
    }

    @Transient
    public boolean isSetAlctAfebNoisy() {
        return (this.alctAfebNoisy!= null);
    }

    /**
     * Gets the value of the alctNoAnodeData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_ALCT_NO_ANODE_DATA", precision = 20, scale = 10)
    @org.cern.cms.csc.dw.model.annotation.OlapMeasure(aggregator = org.cern.cms.csc.dw.model.annotation.OlapMeasure.AggregatorType.SUM, name = "alctNoAnodeData")
    public Long getAlctNoAnodeData() {
        return alctNoAnodeData;
    }

    /**
     * Sets the value of the alctNoAnodeData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setAlctNoAnodeData(Long value) {
        this.alctNoAnodeData = value;
    }

    @Transient
    public boolean isSetAlctNoAnodeData() {
        return (this.alctNoAnodeData!= null);
    }

}
