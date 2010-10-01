//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.ontology;

import javax.xml.bind.annotation.XmlEnum;
import javax.xml.bind.annotation.XmlEnumValue;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for componentClassIdType.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * <p>
 * <pre>
 * &lt;simpleType name="componentClassIdType">
 *   &lt;restriction base="{http://www.w3.org/2001/XMLSchema}string">
 *     &lt;enumeration value="/dev/schar"/>
 *     &lt;enumeration value="AFEB"/>
 *     &lt;enumeration value="ALCT"/>
 *     &lt;enumeration value="CCB"/>
 *     &lt;enumeration value="CFEB"/>
 *     &lt;enumeration value="Chamber"/>
 *     &lt;enumeration value="Computer"/>
 *     &lt;enumeration value="ComputingDevice"/>
 *     &lt;enumeration value="CoolingCircuit"/>
 *     &lt;enumeration value="CoolingDevice"/>
 *     &lt;enumeration value="CSC"/>
 *     &lt;enumeration value="DataChannel"/>
 *     &lt;enumeration value="DataDevice"/>
 *     &lt;enumeration value="DCC"/>
 *     &lt;enumeration value="DDU"/>
 *     &lt;enumeration value="DDUInput"/>
 *     &lt;enumeration value="Device"/>
 *     &lt;enumeration value="DMB"/>
 *     &lt;enumeration value="EmuMonitor"/>
 *     &lt;enumeration value="Endcap"/>
 *     &lt;enumeration value="FactProvider"/>
 *     &lt;enumeration value="FEDCrate"/>
 *     &lt;enumeration value="FEDSystem"/>
 *     &lt;enumeration value="GasChannel"/>
 *     &lt;enumeration value="GasDevice"/>
 *     &lt;enumeration value="GasMixerInputLine"/>
 *     &lt;enumeration value="GasRack"/>
 *     &lt;enumeration value="HVBoard"/>
 *     &lt;enumeration value="HVChannel"/>
 *     &lt;enumeration value="HVCrate"/>
 *     &lt;enumeration value="HVDevice"/>
 *     &lt;enumeration value="HVDistribution"/>
 *     &lt;enumeration value="HVMaster"/>
 *     &lt;enumeration value="HVPrimary"/>
 *     &lt;enumeration value="HVRack"/>
 *     &lt;enumeration value="HVSegment"/>
 *     &lt;enumeration value="Layer"/>
 *     &lt;enumeration value="LogicalComponent"/>
 *     &lt;enumeration value="LogicalDomain"/>
 *     &lt;enumeration value="LTC"/>
 *     &lt;enumeration value="LVDevice"/>
 *     &lt;enumeration value="Maraton"/>
 *     &lt;enumeration value="MPC"/>
 *     &lt;enumeration value="PCrateRack"/>
 *     &lt;enumeration value="PeripheralCrate"/>
 *     &lt;enumeration value="RackTurbine"/>
 *     &lt;enumeration value="RAT"/>
 *     &lt;enumeration value="Ring"/>
 *     &lt;enumeration value="RUI"/>
 *     &lt;enumeration value="SectorProcessor"/>
 *     &lt;enumeration value="SectorProcessorInput"/>
 *     &lt;enumeration value="Slink"/>
 *     &lt;enumeration value="Software"/>
 *     &lt;enumeration value="Station"/>
 *     &lt;enumeration value="TFDDU"/>
 *     &lt;enumeration value="TFRUI"/>
 *     &lt;enumeration value="TMB"/>
 *     &lt;enumeration value="TTC"/>
 *     &lt;enumeration value="VCC"/>
 *   &lt;/restriction>
 * &lt;/simpleType>
 * </pre>
 * 
 */
@XmlType(name = "componentClassIdType")
@XmlEnum
public enum ComponentClassType {

    @XmlEnumValue("/dev/schar")
    __DEV___SCHAR("/dev/schar"),
    AFEB("AFEB"),
    ALCT("ALCT"),
    CCB("CCB"),
    CFEB("CFEB"),
    @XmlEnumValue("Chamber")
    CHAMBER("Chamber"),
    @XmlEnumValue("Computer")
    COMPUTER("Computer"),
    @XmlEnumValue("ComputingDevice")
    COMPUTING_DEVICE("ComputingDevice"),
    @XmlEnumValue("CoolingCircuit")
    COOLING_CIRCUIT("CoolingCircuit"),
    @XmlEnumValue("CoolingDevice")
    COOLING_DEVICE("CoolingDevice"),
    CSC("CSC"),
    @XmlEnumValue("DataChannel")
    DATA_CHANNEL("DataChannel"),
    @XmlEnumValue("DataDevice")
    DATA_DEVICE("DataDevice"),
    DCC("DCC"),
    DDU("DDU"),
    @XmlEnumValue("DDUInput")
    DDU_INPUT("DDUInput"),
    @XmlEnumValue("Device")
    DEVICE("Device"),
    DMB("DMB"),
    @XmlEnumValue("EmuMonitor")
    EMU_MONITOR("EmuMonitor"),
    @XmlEnumValue("Endcap")
    ENDCAP("Endcap"),
    @XmlEnumValue("FactProvider")
    FACT_PROVIDER("FactProvider"),
    @XmlEnumValue("FEDCrate")
    FED_CRATE("FEDCrate"),
    @XmlEnumValue("FEDSystem")
    FED_SYSTEM("FEDSystem"),
    @XmlEnumValue("GasChannel")
    GAS_CHANNEL("GasChannel"),
    @XmlEnumValue("GasDevice")
    GAS_DEVICE("GasDevice"),
    @XmlEnumValue("GasMixerInputLine")
    GAS_MIXER_INPUT_LINE("GasMixerInputLine"),
    @XmlEnumValue("GasRack")
    GAS_RACK("GasRack"),
    @XmlEnumValue("HVBoard")
    HV_BOARD("HVBoard"),
    @XmlEnumValue("HVChannel")
    HV_CHANNEL("HVChannel"),
    @XmlEnumValue("HVCrate")
    HV_CRATE("HVCrate"),
    @XmlEnumValue("HVDevice")
    HV_DEVICE("HVDevice"),
    @XmlEnumValue("HVDistribution")
    HV_DISTRIBUTION("HVDistribution"),
    @XmlEnumValue("HVMaster")
    HV_MASTER("HVMaster"),
    @XmlEnumValue("HVPrimary")
    HV_PRIMARY("HVPrimary"),
    @XmlEnumValue("HVRack")
    HV_RACK("HVRack"),
    @XmlEnumValue("HVSegment")
    HV_SEGMENT("HVSegment"),
    @XmlEnumValue("Layer")
    LAYER("Layer"),
    @XmlEnumValue("LogicalComponent")
    LOGICAL_COMPONENT("LogicalComponent"),
    @XmlEnumValue("LogicalDomain")
    LOGICAL_DOMAIN("LogicalDomain"),
    LTC("LTC"),
    @XmlEnumValue("LVDevice")
    LV_DEVICE("LVDevice"),
    @XmlEnumValue("Maraton")
    MARATON("Maraton"),
    MPC("MPC"),
    @XmlEnumValue("PCrateRack")
    P_CRATE_RACK("PCrateRack"),
    @XmlEnumValue("PeripheralCrate")
    PERIPHERAL_CRATE("PeripheralCrate"),
    @XmlEnumValue("RackTurbine")
    RACK_TURBINE("RackTurbine"),
    RAT("RAT"),
    @XmlEnumValue("Ring")
    RING("Ring"),
    RUI("RUI"),
    @XmlEnumValue("SectorProcessor")
    SECTOR_PROCESSOR("SectorProcessor"),
    @XmlEnumValue("SectorProcessorInput")
    SECTOR_PROCESSOR_INPUT("SectorProcessorInput"),
    @XmlEnumValue("Slink")
    SLINK("Slink"),
    @XmlEnumValue("Software")
    SOFTWARE("Software"),
    @XmlEnumValue("Station")
    STATION("Station"),
    TFDDU("TFDDU"),
    TFRUI("TFRUI"),
    TMB("TMB"),
    TTC("TTC"),
    VCC("VCC");
    private final String value;

    ComponentClassType(String v) {
        value = v;
    }

    public String value() {
        return value;
    }

    public static ComponentClassType fromValue(String v) {
        for (ComponentClassType c: ComponentClassType.values()) {
            if (c.value.equals(v)) {
                return c;
            }
        }
        throw new IllegalArgumentException(v);
    }

    public String getValue() {
        return value;
    }

}
