//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.05 at 03:09:15 PM CET 
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
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for DqmEmuFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmEmuFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="online" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="ALL_CHAMBER_EVENTS_AVERAGE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBER_EVENTS_COUNTER" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITH_BWORDS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITH_FORMAT_ERRORS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITH_INPUT_FIFO_FULL" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITH_INPUT_TIMEOUT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITH_L1A_OUT_OF_SYNC" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITHOUT_ALCT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITHOUT_CFEB" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITHOUT_CLCT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_CHAMBERS_WITHOUT_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_EVENTS_AVERAGE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_EVENTS_COUNTER" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_IN_READOUT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_INPUT_IN_ERROR_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_INPUT_IN_WARNING_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_WITH_DATA" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_WITH_FORMAT_ERRORS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_WITH_INPUT_IN_ERROR_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_WITH_INPUT_IN_WARN_STATE" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_DDU_WITH_LIVE_INPUTS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_HOT_CHAMBERS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_HOT_DDU_IN_READOUT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_LOW_DDU_IN_READOUT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_LOW_EFF_CHAMBERS" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="ALL_NO_DDU_IN_READOUT" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmEmuFactType", propOrder = {
    "online",
    "chamberEventsAverage",
    "chamberEventsCounter",
    "chambersWithBwords",
    "chambersWithFormatErrors",
    "chambersWithInputFifoFull",
    "chambersWithInputTimeout",
    "chambersWithL1AOutOfSync",
    "chambersWithoutAlct",
    "chambersWithoutCfeb",
    "chambersWithoutClct",
    "chambersWithoutData",
    "dduEventsAverage",
    "dduEventsCounter",
    "dduInReadout",
    "dduInputInErrorState",
    "dduInputInWarningState",
    "dduWithData",
    "dduWithFormatErrors",
    "dduWithInputInErrorState",
    "dduWithInputInWarningState",
    "dduWithLiveInputs",
    "chambersHot",
    "dduInReadoutHot",
    "dduInReadoutLow",
    "chambersLowEfficiency",
    "dduNotInReadout"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DqmEmuFact")
@Table(name = "CDW_DQM_EMU_FACTS")
public class DqmEmuFact
    extends Fact
    implements Serializable, Equals, HashCode, ToString
{

    protected boolean online;
    @XmlElement(name = "ALL_CHAMBER_EVENTS_AVERAGE")
    protected Long chamberEventsAverage;
    @XmlElement(name = "ALL_CHAMBER_EVENTS_COUNTER")
    protected Long chamberEventsCounter;
    @XmlElement(name = "ALL_CHAMBERS_WITH_BWORDS")
    protected Long chambersWithBwords;
    @XmlElement(name = "ALL_CHAMBERS_WITH_FORMAT_ERRORS")
    protected Long chambersWithFormatErrors;
    @XmlElement(name = "ALL_CHAMBERS_WITH_INPUT_FIFO_FULL")
    protected Long chambersWithInputFifoFull;
    @XmlElement(name = "ALL_CHAMBERS_WITH_INPUT_TIMEOUT")
    protected Long chambersWithInputTimeout;
    @XmlElement(name = "ALL_CHAMBERS_WITH_L1A_OUT_OF_SYNC")
    protected Long chambersWithL1AOutOfSync;
    @XmlElement(name = "ALL_CHAMBERS_WITHOUT_ALCT")
    protected Long chambersWithoutAlct;
    @XmlElement(name = "ALL_CHAMBERS_WITHOUT_CFEB")
    protected Long chambersWithoutCfeb;
    @XmlElement(name = "ALL_CHAMBERS_WITHOUT_CLCT")
    protected Long chambersWithoutClct;
    @XmlElement(name = "ALL_CHAMBERS_WITHOUT_DATA")
    protected Long chambersWithoutData;
    @XmlElement(name = "ALL_DDU_EVENTS_AVERAGE")
    protected Long dduEventsAverage;
    @XmlElement(name = "ALL_DDU_EVENTS_COUNTER")
    protected Long dduEventsCounter;
    @XmlElement(name = "ALL_DDU_IN_READOUT")
    protected Long dduInReadout;
    @XmlElement(name = "ALL_DDU_INPUT_IN_ERROR_STATE")
    protected Long dduInputInErrorState;
    @XmlElement(name = "ALL_DDU_INPUT_IN_WARNING_STATE")
    protected Long dduInputInWarningState;
    @XmlElement(name = "ALL_DDU_WITH_DATA")
    protected Long dduWithData;
    @XmlElement(name = "ALL_DDU_WITH_FORMAT_ERRORS")
    protected Long dduWithFormatErrors;
    @XmlElement(name = "ALL_DDU_WITH_INPUT_IN_ERROR_STATE")
    protected Long dduWithInputInErrorState;
    @XmlElement(name = "ALL_DDU_WITH_INPUT_IN_WARN_STATE")
    protected Long dduWithInputInWarningState;
    @XmlElement(name = "ALL_DDU_WITH_LIVE_INPUTS")
    protected Long dduWithLiveInputs;
    @XmlElement(name = "ALL_HOT_CHAMBERS")
    protected Long chambersHot;
    @XmlElement(name = "ALL_HOT_DDU_IN_READOUT")
    protected Long dduInReadoutHot;
    @XmlElement(name = "ALL_LOW_DDU_IN_READOUT")
    protected Long dduInReadoutLow;
    @XmlElement(name = "ALL_LOW_EFF_CHAMBERS")
    protected Long chambersLowEfficiency;
    @XmlElement(name = "ALL_NO_DDU_IN_READOUT")
    protected Long dduNotInReadout;

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
     * Gets the value of the chamberEventsAverage property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_EVENTS_AVERAGE", precision = 20, scale = 10)
    public Long getChamberEventsAverage() {
        return chamberEventsAverage;
    }

    /**
     * Sets the value of the chamberEventsAverage property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChamberEventsAverage(Long value) {
        this.chamberEventsAverage = value;
    }

    @Transient
    public boolean isSetChamberEventsAverage() {
        return (this.chamberEventsAverage!= null);
    }

    /**
     * Gets the value of the chamberEventsCounter property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_EVENTS_COUNTER", precision = 20, scale = 10)
    public Long getChamberEventsCounter() {
        return chamberEventsCounter;
    }

    /**
     * Sets the value of the chamberEventsCounter property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChamberEventsCounter(Long value) {
        this.chamberEventsCounter = value;
    }

    @Transient
    public boolean isSetChamberEventsCounter() {
        return (this.chamberEventsCounter!= null);
    }

    /**
     * Gets the value of the chambersWithBwords property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITH_BWORDS", precision = 20, scale = 10)
    public Long getChambersWithBwords() {
        return chambersWithBwords;
    }

    /**
     * Sets the value of the chambersWithBwords property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithBwords(Long value) {
        this.chambersWithBwords = value;
    }

    @Transient
    public boolean isSetChambersWithBwords() {
        return (this.chambersWithBwords!= null);
    }

    /**
     * Gets the value of the chambersWithFormatErrors property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITH_FORMAT_ERRORS", precision = 20, scale = 10)
    public Long getChambersWithFormatErrors() {
        return chambersWithFormatErrors;
    }

    /**
     * Sets the value of the chambersWithFormatErrors property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithFormatErrors(Long value) {
        this.chambersWithFormatErrors = value;
    }

    @Transient
    public boolean isSetChambersWithFormatErrors() {
        return (this.chambersWithFormatErrors!= null);
    }

    /**
     * Gets the value of the chambersWithInputFifoFull property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITH_INPUT_FIFO_FULL", precision = 20, scale = 10)
    public Long getChambersWithInputFifoFull() {
        return chambersWithInputFifoFull;
    }

    /**
     * Sets the value of the chambersWithInputFifoFull property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithInputFifoFull(Long value) {
        this.chambersWithInputFifoFull = value;
    }

    @Transient
    public boolean isSetChambersWithInputFifoFull() {
        return (this.chambersWithInputFifoFull!= null);
    }

    /**
     * Gets the value of the chambersWithInputTimeout property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CHA_WITH_INPUT_TIMEOUT", precision = 20, scale = 10)
    public Long getChambersWithInputTimeout() {
        return chambersWithInputTimeout;
    }

    /**
     * Sets the value of the chambersWithInputTimeout property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithInputTimeout(Long value) {
        this.chambersWithInputTimeout = value;
    }

    @Transient
    public boolean isSetChambersWithInputTimeout() {
        return (this.chambersWithInputTimeout!= null);
    }

    /**
     * Gets the value of the chambersWithL1AOutOfSync property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITH_L1A_OUT_OF_SYNC", precision = 20, scale = 10)
    public Long getChambersWithL1AOutOfSync() {
        return chambersWithL1AOutOfSync;
    }

    /**
     * Sets the value of the chambersWithL1AOutOfSync property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithL1AOutOfSync(Long value) {
        this.chambersWithL1AOutOfSync = value;
    }

    @Transient
    public boolean isSetChambersWithL1AOutOfSync() {
        return (this.chambersWithL1AOutOfSync!= null);
    }

    /**
     * Gets the value of the chambersWithoutAlct property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITHOUT_ALCT", precision = 20, scale = 10)
    public Long getChambersWithoutAlct() {
        return chambersWithoutAlct;
    }

    /**
     * Sets the value of the chambersWithoutAlct property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithoutAlct(Long value) {
        this.chambersWithoutAlct = value;
    }

    @Transient
    public boolean isSetChambersWithoutAlct() {
        return (this.chambersWithoutAlct!= null);
    }

    /**
     * Gets the value of the chambersWithoutCfeb property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITHOUT_CFEB", precision = 20, scale = 10)
    public Long getChambersWithoutCfeb() {
        return chambersWithoutCfeb;
    }

    /**
     * Sets the value of the chambersWithoutCfeb property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithoutCfeb(Long value) {
        this.chambersWithoutCfeb = value;
    }

    @Transient
    public boolean isSetChambersWithoutCfeb() {
        return (this.chambersWithoutCfeb!= null);
    }

    /**
     * Gets the value of the chambersWithoutClct property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITHOUT_CLCT", precision = 20, scale = 10)
    public Long getChambersWithoutClct() {
        return chambersWithoutClct;
    }

    /**
     * Sets the value of the chambersWithoutClct property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithoutClct(Long value) {
        this.chambersWithoutClct = value;
    }

    @Transient
    public boolean isSetChambersWithoutClct() {
        return (this.chambersWithoutClct!= null);
    }

    /**
     * Gets the value of the chambersWithoutData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_CH_WITHOUT_DATA", precision = 20, scale = 10)
    public Long getChambersWithoutData() {
        return chambersWithoutData;
    }

    /**
     * Sets the value of the chambersWithoutData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersWithoutData(Long value) {
        this.chambersWithoutData = value;
    }

    @Transient
    public boolean isSetChambersWithoutData() {
        return (this.chambersWithoutData!= null);
    }

    /**
     * Gets the value of the dduEventsAverage property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_EVENTS_AVERAGE", precision = 20, scale = 10)
    public Long getDduEventsAverage() {
        return dduEventsAverage;
    }

    /**
     * Sets the value of the dduEventsAverage property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduEventsAverage(Long value) {
        this.dduEventsAverage = value;
    }

    @Transient
    public boolean isSetDduEventsAverage() {
        return (this.dduEventsAverage!= null);
    }

    /**
     * Gets the value of the dduEventsCounter property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_EVENTS_COUNTER", precision = 20, scale = 10)
    public Long getDduEventsCounter() {
        return dduEventsCounter;
    }

    /**
     * Sets the value of the dduEventsCounter property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduEventsCounter(Long value) {
        this.dduEventsCounter = value;
    }

    @Transient
    public boolean isSetDduEventsCounter() {
        return (this.dduEventsCounter!= null);
    }

    /**
     * Gets the value of the dduInReadout property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_IN_READOUT", precision = 20, scale = 10)
    public Long getDduInReadout() {
        return dduInReadout;
    }

    /**
     * Sets the value of the dduInReadout property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduInReadout(Long value) {
        this.dduInReadout = value;
    }

    @Transient
    public boolean isSetDduInReadout() {
        return (this.dduInReadout!= null);
    }

    /**
     * Gets the value of the dduInputInErrorState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_INPUT_IN_ERROR_STATE", precision = 20, scale = 10)
    public Long getDduInputInErrorState() {
        return dduInputInErrorState;
    }

    /**
     * Sets the value of the dduInputInErrorState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduInputInErrorState(Long value) {
        this.dduInputInErrorState = value;
    }

    @Transient
    public boolean isSetDduInputInErrorState() {
        return (this.dduInputInErrorState!= null);
    }

    /**
     * Gets the value of the dduInputInWarningState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_INPUT_IN_WARNING_STATE", precision = 20, scale = 10)
    public Long getDduInputInWarningState() {
        return dduInputInWarningState;
    }

    /**
     * Sets the value of the dduInputInWarningState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduInputInWarningState(Long value) {
        this.dduInputInWarningState = value;
    }

    @Transient
    public boolean isSetDduInputInWarningState() {
        return (this.dduInputInWarningState!= null);
    }

    /**
     * Gets the value of the dduWithData property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_WITH_DATA", precision = 20, scale = 10)
    public Long getDduWithData() {
        return dduWithData;
    }

    /**
     * Sets the value of the dduWithData property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduWithData(Long value) {
        this.dduWithData = value;
    }

    @Transient
    public boolean isSetDduWithData() {
        return (this.dduWithData!= null);
    }

    /**
     * Gets the value of the dduWithFormatErrors property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_WITH_FORMAT_ERRORS", precision = 20, scale = 10)
    public Long getDduWithFormatErrors() {
        return dduWithFormatErrors;
    }

    /**
     * Sets the value of the dduWithFormatErrors property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduWithFormatErrors(Long value) {
        this.dduWithFormatErrors = value;
    }

    @Transient
    public boolean isSetDduWithFormatErrors() {
        return (this.dduWithFormatErrors!= null);
    }

    /**
     * Gets the value of the dduWithInputInErrorState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_WITH_INPUT_IN_ERROR_ST", precision = 20, scale = 10)
    public Long getDduWithInputInErrorState() {
        return dduWithInputInErrorState;
    }

    /**
     * Sets the value of the dduWithInputInErrorState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduWithInputInErrorState(Long value) {
        this.dduWithInputInErrorState = value;
    }

    @Transient
    public boolean isSetDduWithInputInErrorState() {
        return (this.dduWithInputInErrorState!= null);
    }

    /**
     * Gets the value of the dduWithInputInWarningState property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_WITH_INPUT_IN_WARN_STA", precision = 20, scale = 10)
    public Long getDduWithInputInWarningState() {
        return dduWithInputInWarningState;
    }

    /**
     * Sets the value of the dduWithInputInWarningState property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduWithInputInWarningState(Long value) {
        this.dduWithInputInWarningState = value;
    }

    @Transient
    public boolean isSetDduWithInputInWarningState() {
        return (this.dduWithInputInWarningState!= null);
    }

    /**
     * Gets the value of the dduWithLiveInputs property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_WITH_LIVE_INPUTS", precision = 20, scale = 10)
    public Long getDduWithLiveInputs() {
        return dduWithLiveInputs;
    }

    /**
     * Sets the value of the dduWithLiveInputs property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduWithLiveInputs(Long value) {
        this.dduWithLiveInputs = value;
    }

    @Transient
    public boolean isSetDduWithLiveInputs() {
        return (this.dduWithLiveInputs!= null);
    }

    /**
     * Gets the value of the chambersHot property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_HOT_CHAMBERS", precision = 20, scale = 10)
    public Long getChambersHot() {
        return chambersHot;
    }

    /**
     * Sets the value of the chambersHot property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersHot(Long value) {
        this.chambersHot = value;
    }

    @Transient
    public boolean isSetChambersHot() {
        return (this.chambersHot!= null);
    }

    /**
     * Gets the value of the dduInReadoutHot property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_HOT_DDU_IN_READOUT", precision = 20, scale = 10)
    public Long getDduInReadoutHot() {
        return dduInReadoutHot;
    }

    /**
     * Sets the value of the dduInReadoutHot property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduInReadoutHot(Long value) {
        this.dduInReadoutHot = value;
    }

    @Transient
    public boolean isSetDduInReadoutHot() {
        return (this.dduInReadoutHot!= null);
    }

    /**
     * Gets the value of the dduInReadoutLow property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_LOW_DDU_IN_READOUT", precision = 20, scale = 10)
    public Long getDduInReadoutLow() {
        return dduInReadoutLow;
    }

    /**
     * Sets the value of the dduInReadoutLow property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduInReadoutLow(Long value) {
        this.dduInReadoutLow = value;
    }

    @Transient
    public boolean isSetDduInReadoutLow() {
        return (this.dduInReadoutLow!= null);
    }

    /**
     * Gets the value of the chambersLowEfficiency property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_LOW_EFF_CHAMBERS", precision = 20, scale = 10)
    public Long getChambersLowEfficiency() {
        return chambersLowEfficiency;
    }

    /**
     * Sets the value of the chambersLowEfficiency property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setChambersLowEfficiency(Long value) {
        this.chambersLowEfficiency = value;
    }

    @Transient
    public boolean isSetChambersLowEfficiency() {
        return (this.chambersLowEfficiency!= null);
    }

    /**
     * Gets the value of the dduNotInReadout property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCT_NO_DDU_IN_READOUT", precision = 20, scale = 10)
    public Long getDduNotInReadout() {
        return dduNotInReadout;
    }

    /**
     * Sets the value of the dduNotInReadout property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setDduNotInReadout(Long value) {
        this.dduNotInReadout = value;
    }

    @Transient
    public boolean isSetDduNotInReadout() {
        return (this.dduNotInReadout!= null);
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DqmEmuFact)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        super.equals(object, equalsBuilder);
        final DqmEmuFact that = ((DqmEmuFact) object);
        equalsBuilder.append(this.isOnline(), that.isOnline());
        equalsBuilder.append(this.getChamberEventsAverage(), that.getChamberEventsAverage());
        equalsBuilder.append(this.getChamberEventsCounter(), that.getChamberEventsCounter());
        equalsBuilder.append(this.getChambersWithBwords(), that.getChambersWithBwords());
        equalsBuilder.append(this.getChambersWithFormatErrors(), that.getChambersWithFormatErrors());
        equalsBuilder.append(this.getChambersWithInputFifoFull(), that.getChambersWithInputFifoFull());
        equalsBuilder.append(this.getChambersWithInputTimeout(), that.getChambersWithInputTimeout());
        equalsBuilder.append(this.getChambersWithL1AOutOfSync(), that.getChambersWithL1AOutOfSync());
        equalsBuilder.append(this.getChambersWithoutAlct(), that.getChambersWithoutAlct());
        equalsBuilder.append(this.getChambersWithoutCfeb(), that.getChambersWithoutCfeb());
        equalsBuilder.append(this.getChambersWithoutClct(), that.getChambersWithoutClct());
        equalsBuilder.append(this.getChambersWithoutData(), that.getChambersWithoutData());
        equalsBuilder.append(this.getDduEventsAverage(), that.getDduEventsAverage());
        equalsBuilder.append(this.getDduEventsCounter(), that.getDduEventsCounter());
        equalsBuilder.append(this.getDduInReadout(), that.getDduInReadout());
        equalsBuilder.append(this.getDduInputInErrorState(), that.getDduInputInErrorState());
        equalsBuilder.append(this.getDduInputInWarningState(), that.getDduInputInWarningState());
        equalsBuilder.append(this.getDduWithData(), that.getDduWithData());
        equalsBuilder.append(this.getDduWithFormatErrors(), that.getDduWithFormatErrors());
        equalsBuilder.append(this.getDduWithInputInErrorState(), that.getDduWithInputInErrorState());
        equalsBuilder.append(this.getDduWithInputInWarningState(), that.getDduWithInputInWarningState());
        equalsBuilder.append(this.getDduWithLiveInputs(), that.getDduWithLiveInputs());
        equalsBuilder.append(this.getChambersHot(), that.getChambersHot());
        equalsBuilder.append(this.getDduInReadoutHot(), that.getDduInReadoutHot());
        equalsBuilder.append(this.getDduInReadoutLow(), that.getDduInReadoutLow());
        equalsBuilder.append(this.getChambersLowEfficiency(), that.getChambersLowEfficiency());
        equalsBuilder.append(this.getDduNotInReadout(), that.getDduNotInReadout());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DqmEmuFact)) {
            return false;
        }
        if (this == object) {
            return true;
        }
        final EqualsBuilder equalsBuilder = new JAXBEqualsBuilder();
        equals(object, equalsBuilder);
        return equalsBuilder.isEquals();
    }

    public void hashCode(HashCodeBuilder hashCodeBuilder) {
        super.hashCode(hashCodeBuilder);
        hashCodeBuilder.append(this.isOnline());
        hashCodeBuilder.append(this.getChamberEventsAverage());
        hashCodeBuilder.append(this.getChamberEventsCounter());
        hashCodeBuilder.append(this.getChambersWithBwords());
        hashCodeBuilder.append(this.getChambersWithFormatErrors());
        hashCodeBuilder.append(this.getChambersWithInputFifoFull());
        hashCodeBuilder.append(this.getChambersWithInputTimeout());
        hashCodeBuilder.append(this.getChambersWithL1AOutOfSync());
        hashCodeBuilder.append(this.getChambersWithoutAlct());
        hashCodeBuilder.append(this.getChambersWithoutCfeb());
        hashCodeBuilder.append(this.getChambersWithoutClct());
        hashCodeBuilder.append(this.getChambersWithoutData());
        hashCodeBuilder.append(this.getDduEventsAverage());
        hashCodeBuilder.append(this.getDduEventsCounter());
        hashCodeBuilder.append(this.getDduInReadout());
        hashCodeBuilder.append(this.getDduInputInErrorState());
        hashCodeBuilder.append(this.getDduInputInWarningState());
        hashCodeBuilder.append(this.getDduWithData());
        hashCodeBuilder.append(this.getDduWithFormatErrors());
        hashCodeBuilder.append(this.getDduWithInputInErrorState());
        hashCodeBuilder.append(this.getDduWithInputInWarningState());
        hashCodeBuilder.append(this.getDduWithLiveInputs());
        hashCodeBuilder.append(this.getChambersHot());
        hashCodeBuilder.append(this.getDduInReadoutHot());
        hashCodeBuilder.append(this.getDduInReadoutLow());
        hashCodeBuilder.append(this.getChambersLowEfficiency());
        hashCodeBuilder.append(this.getDduNotInReadout());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        super.toString(toStringBuilder);
        {
            boolean theOnline;
            theOnline = this.isOnline();
            toStringBuilder.append("online", theOnline);
        }
        {
            Long theChamberEventsAverage;
            theChamberEventsAverage = this.getChamberEventsAverage();
            toStringBuilder.append("chamberEventsAverage", theChamberEventsAverage);
        }
        {
            Long theChamberEventsCounter;
            theChamberEventsCounter = this.getChamberEventsCounter();
            toStringBuilder.append("chamberEventsCounter", theChamberEventsCounter);
        }
        {
            Long theChambersWithBwords;
            theChambersWithBwords = this.getChambersWithBwords();
            toStringBuilder.append("chambersWithBwords", theChambersWithBwords);
        }
        {
            Long theChambersWithFormatErrors;
            theChambersWithFormatErrors = this.getChambersWithFormatErrors();
            toStringBuilder.append("chambersWithFormatErrors", theChambersWithFormatErrors);
        }
        {
            Long theChambersWithInputFifoFull;
            theChambersWithInputFifoFull = this.getChambersWithInputFifoFull();
            toStringBuilder.append("chambersWithInputFifoFull", theChambersWithInputFifoFull);
        }
        {
            Long theChambersWithInputTimeout;
            theChambersWithInputTimeout = this.getChambersWithInputTimeout();
            toStringBuilder.append("chambersWithInputTimeout", theChambersWithInputTimeout);
        }
        {
            Long theChambersWithL1AOutOfSync;
            theChambersWithL1AOutOfSync = this.getChambersWithL1AOutOfSync();
            toStringBuilder.append("chambersWithL1AOutOfSync", theChambersWithL1AOutOfSync);
        }
        {
            Long theChambersWithoutAlct;
            theChambersWithoutAlct = this.getChambersWithoutAlct();
            toStringBuilder.append("chambersWithoutAlct", theChambersWithoutAlct);
        }
        {
            Long theChambersWithoutCfeb;
            theChambersWithoutCfeb = this.getChambersWithoutCfeb();
            toStringBuilder.append("chambersWithoutCfeb", theChambersWithoutCfeb);
        }
        {
            Long theChambersWithoutClct;
            theChambersWithoutClct = this.getChambersWithoutClct();
            toStringBuilder.append("chambersWithoutClct", theChambersWithoutClct);
        }
        {
            Long theChambersWithoutData;
            theChambersWithoutData = this.getChambersWithoutData();
            toStringBuilder.append("chambersWithoutData", theChambersWithoutData);
        }
        {
            Long theDduEventsAverage;
            theDduEventsAverage = this.getDduEventsAverage();
            toStringBuilder.append("dduEventsAverage", theDduEventsAverage);
        }
        {
            Long theDduEventsCounter;
            theDduEventsCounter = this.getDduEventsCounter();
            toStringBuilder.append("dduEventsCounter", theDduEventsCounter);
        }
        {
            Long theDduInReadout;
            theDduInReadout = this.getDduInReadout();
            toStringBuilder.append("dduInReadout", theDduInReadout);
        }
        {
            Long theDduInputInErrorState;
            theDduInputInErrorState = this.getDduInputInErrorState();
            toStringBuilder.append("dduInputInErrorState", theDduInputInErrorState);
        }
        {
            Long theDduInputInWarningState;
            theDduInputInWarningState = this.getDduInputInWarningState();
            toStringBuilder.append("dduInputInWarningState", theDduInputInWarningState);
        }
        {
            Long theDduWithData;
            theDduWithData = this.getDduWithData();
            toStringBuilder.append("dduWithData", theDduWithData);
        }
        {
            Long theDduWithFormatErrors;
            theDduWithFormatErrors = this.getDduWithFormatErrors();
            toStringBuilder.append("dduWithFormatErrors", theDduWithFormatErrors);
        }
        {
            Long theDduWithInputInErrorState;
            theDduWithInputInErrorState = this.getDduWithInputInErrorState();
            toStringBuilder.append("dduWithInputInErrorState", theDduWithInputInErrorState);
        }
        {
            Long theDduWithInputInWarningState;
            theDduWithInputInWarningState = this.getDduWithInputInWarningState();
            toStringBuilder.append("dduWithInputInWarningState", theDduWithInputInWarningState);
        }
        {
            Long theDduWithLiveInputs;
            theDduWithLiveInputs = this.getDduWithLiveInputs();
            toStringBuilder.append("dduWithLiveInputs", theDduWithLiveInputs);
        }
        {
            Long theChambersHot;
            theChambersHot = this.getChambersHot();
            toStringBuilder.append("chambersHot", theChambersHot);
        }
        {
            Long theDduInReadoutHot;
            theDduInReadoutHot = this.getDduInReadoutHot();
            toStringBuilder.append("dduInReadoutHot", theDduInReadoutHot);
        }
        {
            Long theDduInReadoutLow;
            theDduInReadoutLow = this.getDduInReadoutLow();
            toStringBuilder.append("dduInReadoutLow", theDduInReadoutLow);
        }
        {
            Long theChambersLowEfficiency;
            theChambersLowEfficiency = this.getChambersLowEfficiency();
            toStringBuilder.append("chambersLowEfficiency", theChambersLowEfficiency);
        }
        {
            Long theDduNotInReadout;
            theDduNotInReadout = this.getDduNotInReadout();
            toStringBuilder.append("dduNotInReadout", theDduNotInReadout);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }
    
//--simple--preserve

    @Override
    public boolean checkComponentClassType(org.cern.cms.csc.dw.model.ontology.ComponentClassType componentClassType) {
        return componentClassType.equals(org.cern.cms.csc.dw.model.ontology.ComponentClassType.CSC);
    }


//--simple--preserve

}
