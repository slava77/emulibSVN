//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import java.math.BigInteger;
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
 * <p>Java class for EmuMonitorFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="EmuMonitorFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="state" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="stateChangeTime" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="dqmEvents" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="dqmRate" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="cscRate" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element name="cscDetected" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="cscUnpacked" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "EmuMonitorFactType", propOrder = {
    "state",
    "stateChangeTime",
    "dqmEvents",
    "dqmRate",
    "cscRate",
    "cscDetected",
    "cscUnpacked"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.EmuMonitorFact")
@Table(name = "CDW_EMU_MONITOR_FACTS")
@FactAnn(limitComponents = {
    "EmuMonitor"
}, title = "Emu Monitor fact")
public class EmuMonitorFact
    extends Fact
    implements Serializable
{

    @XmlElement(required = true)
    protected String state;
    @XmlElement(required = true)
    protected String stateChangeTime;
    protected long dqmEvents;
    protected long dqmRate;
    protected long cscRate;
    @XmlElement(required = true)
    protected BigInteger cscDetected;
    protected long cscUnpacked;

    /**
     * Gets the value of the state property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_STATE", length = 12)
    public String getState() {
        return state;
    }

    /**
     * Sets the value of the state property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setState(String value) {
        this.state = value;
    }

    @Transient
    public boolean isSetState() {
        return (this.state!= null);
    }

    /**
     * Gets the value of the stateChangeTime property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_STATE_CHANGE_TIME", length = 25)
    public String getStateChangeTime() {
        return stateChangeTime;
    }

    /**
     * Sets the value of the stateChangeTime property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setStateChangeTime(String value) {
        this.stateChangeTime = value;
    }

    @Transient
    public boolean isSetStateChangeTime() {
        return (this.stateChangeTime!= null);
    }

    /**
     * Gets the value of the dqmEvents property.
     * 
     */
    @Basic
    @Column(name = "FCT_DQM_EVENTS", precision = 20, scale = 10)
    public long getDqmEvents() {
        return dqmEvents;
    }

    /**
     * Sets the value of the dqmEvents property.
     * 
     */
    public void setDqmEvents(long value) {
        this.dqmEvents = value;
    }

    @Transient
    public boolean isSetDqmEvents() {
        return true;
    }

    /**
     * Gets the value of the dqmRate property.
     * 
     */
    @Basic
    @Column(name = "FCT_DQM_RATE", precision = 20, scale = 10)
    public long getDqmRate() {
        return dqmRate;
    }

    /**
     * Sets the value of the dqmRate property.
     * 
     */
    public void setDqmRate(long value) {
        this.dqmRate = value;
    }

    @Transient
    public boolean isSetDqmRate() {
        return true;
    }

    /**
     * Gets the value of the cscRate property.
     * 
     */
    @Basic
    @Column(name = "FCT_CSC_RATE", precision = 20, scale = 10)
    public long getCscRate() {
        return cscRate;
    }

    /**
     * Sets the value of the cscRate property.
     * 
     */
    public void setCscRate(long value) {
        this.cscRate = value;
    }

    @Transient
    public boolean isSetCscRate() {
        return true;
    }

    /**
     * Gets the value of the cscDetected property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_CSC_DETECTED", precision = 20, scale = 10)
    public BigInteger getCscDetected() {
        return cscDetected;
    }

    /**
     * Sets the value of the cscDetected property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setCscDetected(BigInteger value) {
        this.cscDetected = value;
    }

    @Transient
    public boolean isSetCscDetected() {
        return (this.cscDetected!= null);
    }

    /**
     * Gets the value of the cscUnpacked property.
     * 
     */
    @Basic
    @Column(name = "FCT_CSC_UNPACKED", precision = 20, scale = 10)
    public long getCscUnpacked() {
        return cscUnpacked;
    }

    /**
     * Sets the value of the cscUnpacked property.
     * 
     */
    public void setCscUnpacked(long value) {
        this.cscUnpacked = value;
    }

    @Transient
    public boolean isSetCscUnpacked() {
        return true;
    }

}
