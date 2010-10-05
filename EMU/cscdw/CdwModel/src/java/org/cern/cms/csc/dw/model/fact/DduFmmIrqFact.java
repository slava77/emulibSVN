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
 * <p>Java class for DDUFMMIRQFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DDUFMMIRQFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="crateNumber" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="slotNumber" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="rui" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="hardError" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="syncError" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="resetRequested" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="fiberStatus" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="advancedFiberStatus" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="xorStatus" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="ddufpgaDebugTrap" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="infpga0DebugTrap" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="infpga1DebugTrap" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DDUFMMIRQFactType", propOrder = {
    "crateNumber",
    "slotNumber",
    "rui",
    "hardError",
    "syncError",
    "resetRequested",
    "fiberStatus",
    "advancedFiberStatus",
    "xorStatus",
    "ddufpgaDebugTrap",
    "infpga0DebugTrap",
    "infpga1DebugTrap"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.DduFmmIrqFact")
@Table(name = "CDW_DDU_FMM_IRQ_FACTS")
@FactAnn(limitComponents = {
    "DDU"
}, title = "DDU FMM IRQ Information")
public class DduFmmIrqFact
    extends Fact
    implements Serializable
{

    @XmlElement(required = true)
    protected BigInteger crateNumber;
    @XmlElement(required = true)
    protected BigInteger slotNumber;
    @XmlElement(required = true)
    protected BigInteger rui;
    protected boolean hardError;
    protected boolean syncError;
    protected boolean resetRequested;
    @XmlElement(required = true)
    protected BigInteger fiberStatus;
    @XmlElement(required = true)
    protected BigInteger advancedFiberStatus;
    @XmlElement(required = true)
    protected BigInteger xorStatus;
    @XmlElement(required = true)
    protected String ddufpgaDebugTrap;
    @XmlElement(required = true)
    protected String infpga0DebugTrap;
    @XmlElement(required = true)
    protected String infpga1DebugTrap;

    /**
     * Gets the value of the crateNumber property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_CRATE_NUMBER", precision = 3, scale = 0)
    public BigInteger getCrateNumber() {
        return crateNumber;
    }

    /**
     * Sets the value of the crateNumber property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setCrateNumber(BigInteger value) {
        this.crateNumber = value;
    }

    @Transient
    public boolean isSetCrateNumber() {
        return (this.crateNumber!= null);
    }

    /**
     * Gets the value of the slotNumber property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_SLOT_NUMBER", precision = 2, scale = 0)
    public BigInteger getSlotNumber() {
        return slotNumber;
    }

    /**
     * Sets the value of the slotNumber property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setSlotNumber(BigInteger value) {
        this.slotNumber = value;
    }

    @Transient
    public boolean isSetSlotNumber() {
        return (this.slotNumber!= null);
    }

    /**
     * Gets the value of the rui property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_RUI", precision = 2, scale = 0)
    public BigInteger getRui() {
        return rui;
    }

    /**
     * Sets the value of the rui property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setRui(BigInteger value) {
        this.rui = value;
    }

    @Transient
    public boolean isSetRui() {
        return (this.rui!= null);
    }

    /**
     * Gets the value of the hardError property.
     * 
     */
    @Basic
    @Column(name = "FCT_HARD_ERROR")
    public boolean isHardError() {
        return hardError;
    }

    /**
     * Sets the value of the hardError property.
     * 
     */
    public void setHardError(boolean value) {
        this.hardError = value;
    }

    @Transient
    public boolean isSetHardError() {
        return true;
    }

    /**
     * Gets the value of the syncError property.
     * 
     */
    @Basic
    @Column(name = "FCT_SYNC_ERROR")
    public boolean isSyncError() {
        return syncError;
    }

    /**
     * Sets the value of the syncError property.
     * 
     */
    public void setSyncError(boolean value) {
        this.syncError = value;
    }

    @Transient
    public boolean isSetSyncError() {
        return true;
    }

    /**
     * Gets the value of the resetRequested property.
     * 
     */
    @Basic
    @Column(name = "FCT_RESET_REQUESTED")
    public boolean isResetRequested() {
        return resetRequested;
    }

    /**
     * Sets the value of the resetRequested property.
     * 
     */
    public void setResetRequested(boolean value) {
        this.resetRequested = value;
    }

    @Transient
    public boolean isSetResetRequested() {
        return true;
    }

    /**
     * Gets the value of the fiberStatus property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_FIBER_STATUS", precision = 10, scale = 0)
    public BigInteger getFiberStatus() {
        return fiberStatus;
    }

    /**
     * Sets the value of the fiberStatus property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setFiberStatus(BigInteger value) {
        this.fiberStatus = value;
    }

    @Transient
    public boolean isSetFiberStatus() {
        return (this.fiberStatus!= null);
    }

    /**
     * Gets the value of the advancedFiberStatus property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_ADVANCED_FIBER_STATUS", precision = 10, scale = 0)
    public BigInteger getAdvancedFiberStatus() {
        return advancedFiberStatus;
    }

    /**
     * Sets the value of the advancedFiberStatus property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setAdvancedFiberStatus(BigInteger value) {
        this.advancedFiberStatus = value;
    }

    @Transient
    public boolean isSetAdvancedFiberStatus() {
        return (this.advancedFiberStatus!= null);
    }

    /**
     * Gets the value of the xorStatus property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "FCT_XOR_STATUS", precision = 10, scale = 0)
    public BigInteger getXorStatus() {
        return xorStatus;
    }

    /**
     * Sets the value of the xorStatus property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setXorStatus(BigInteger value) {
        this.xorStatus = value;
    }

    @Transient
    public boolean isSetXorStatus() {
        return (this.xorStatus!= null);
    }

    /**
     * Gets the value of the ddufpgaDebugTrap property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_DDU_FPGA_DEBUG_TRAP", length = 255)
    public String getDdufpgaDebugTrap() {
        return ddufpgaDebugTrap;
    }

    /**
     * Sets the value of the ddufpgaDebugTrap property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setDdufpgaDebugTrap(String value) {
        this.ddufpgaDebugTrap = value;
    }

    @Transient
    public boolean isSetDdufpgaDebugTrap() {
        return (this.ddufpgaDebugTrap!= null);
    }

    /**
     * Gets the value of the infpga0DebugTrap property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_IN_FPGA0_DEBUG_TRAP", length = 255)
    public String getInfpga0DebugTrap() {
        return infpga0DebugTrap;
    }

    /**
     * Sets the value of the infpga0DebugTrap property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setInfpga0DebugTrap(String value) {
        this.infpga0DebugTrap = value;
    }

    @Transient
    public boolean isSetInfpga0DebugTrap() {
        return (this.infpga0DebugTrap!= null);
    }

    /**
     * Gets the value of the infpga1DebugTrap property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_IN_FPGA1_DEBUG_TRAP", length = 255)
    public String getInfpga1DebugTrap() {
        return infpga1DebugTrap;
    }

    /**
     * Sets the value of the infpga1DebugTrap property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setInfpga1DebugTrap(String value) {
        this.infpga1DebugTrap = value;
    }

    @Transient
    public boolean isSetInfpga1DebugTrap() {
        return (this.infpga1DebugTrap!= null);
    }

}
