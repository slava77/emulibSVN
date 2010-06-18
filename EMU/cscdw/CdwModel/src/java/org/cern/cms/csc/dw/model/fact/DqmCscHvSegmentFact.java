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
 * <p>Java class for DqmCscHvSegmentFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmCscHvSegmentFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="CSC_NO_HV_SEGMENT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmCscHvSegmentFactType", propOrder = {
    "online",
    "noHvSegment"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmCscHvSegmentFact")
@Table(name = "CDW_DQM_CSC_HV_SEGMENT_FACTS")
@FactAnn(limitComponents = {
    "HVSegment"
}, title = "DQM HV Segment")
public class DqmCscHvSegmentFact
    extends Fact
    implements Serializable
{

    protected boolean online;
    @XmlElement(name = "CSC_NO_HV_SEGMENT")
    protected Long noHvSegment;

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
     * Gets the value of the noHvSegment property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_NO_HV_SEGMENT", precision = 20, scale = 10)
    public Long getNoHvSegment() {
        return noHvSegment;
    }

    /**
     * Sets the value of the noHvSegment property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setNoHvSegment(Long value) {
        this.noHvSegment = value;
    }

    @Transient
    public boolean isSetNoHvSegment() {
        return (this.noHvSegment!= null);
    }

}
