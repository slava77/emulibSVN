//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.03.19 at 06:37:09 PM CET 
//


package org.cern.cms.csc.dw.model.fact;

import javax.xml.bind.annotation.XmlEnum;
import javax.xml.bind.annotation.XmlEnumValue;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for runTypeType.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * <p>
 * <pre>
 * &lt;simpleType name="runTypeType">
 *   &lt;restriction base="{http://www.w3.org/2001/XMLSchema}string">
 *     &lt;enumeration value="Monitor"/>
 *     &lt;enumeration value="SCA_Pedestals"/>
 *     &lt;enumeration value="Comparator_Thresh"/>
 *     &lt;enumeration value="Comparator_Spread"/>
 *     &lt;enumeration value="Comparator_LeftRight"/>
 *     &lt;enumeration value="Comparator_TimeOffset"/>
 *     &lt;enumeration value="Comparator_Rate"/>
 *     &lt;enumeration value="CFEB_Saturation"/>
 *     &lt;enumeration value="CFEB_Connectivity"/>
 *     &lt;enumeration value="CFEB_CompLogic"/>
 *     &lt;enumeration value="CFEB_CompThreshDAC"/>
 *     &lt;enumeration value="CFEB_PulseDAC"/>
 *     &lt;enumeration value="CFEB_NoiseMatrix"/>
 *     &lt;enumeration value="CFEB_Crosstalk"/>
 *     &lt;enumeration value="CFEB_Gains"/>
 *     &lt;enumeration value="AFEB_DAC"/>
 *     &lt;enumeration value="AFEB_TimeDelay"/>
 *     &lt;enumeration value="AFEB_NoiseRate"/>
 *     &lt;enumeration value="AFEB_Connectivity"/>
 *     &lt;enumeration value="AFEB_ALCT"/>
 *     &lt;enumeration value="AFEB_Pulse"/>
 *     &lt;enumeration value="HV_Scan"/>
 *     &lt;enumeration value="STEP"/>
 *     &lt;enumeration value="Debug"/>
 *   &lt;/restriction>
 * &lt;/simpleType>
 * </pre>
 * 
 */
@XmlType(name = "runTypeType")
@XmlEnum
public enum RunType {

    @XmlEnumValue("Monitor")
    MONITOR("Monitor"),
    @XmlEnumValue("SCA_Pedestals")
    SCA___PEDESTALS("SCA_Pedestals"),
    @XmlEnumValue("Comparator_Thresh")
    COMPARATOR___THRESH("Comparator_Thresh"),
    @XmlEnumValue("Comparator_Spread")
    COMPARATOR___SPREAD("Comparator_Spread"),
    @XmlEnumValue("Comparator_LeftRight")
    COMPARATOR___LEFT_RIGHT("Comparator_LeftRight"),
    @XmlEnumValue("Comparator_TimeOffset")
    COMPARATOR___TIME_OFFSET("Comparator_TimeOffset"),
    @XmlEnumValue("Comparator_Rate")
    COMPARATOR___RATE("Comparator_Rate"),
    @XmlEnumValue("CFEB_Saturation")
    CFEB___SATURATION("CFEB_Saturation"),
    @XmlEnumValue("CFEB_Connectivity")
    CFEB___CONNECTIVITY("CFEB_Connectivity"),
    @XmlEnumValue("CFEB_CompLogic")
    CFEB___COMP_LOGIC("CFEB_CompLogic"),
    @XmlEnumValue("CFEB_CompThreshDAC")
    CFEB___COMP_THRESH_DAC("CFEB_CompThreshDAC"),
    @XmlEnumValue("CFEB_PulseDAC")
    CFEB___PULSE_DAC("CFEB_PulseDAC"),
    @XmlEnumValue("CFEB_NoiseMatrix")
    CFEB___NOISE_MATRIX("CFEB_NoiseMatrix"),
    @XmlEnumValue("CFEB_Crosstalk")
    CFEB___CROSSTALK("CFEB_Crosstalk"),
    @XmlEnumValue("CFEB_Gains")
    CFEB___GAINS("CFEB_Gains"),
    @XmlEnumValue("AFEB_DAC")
    AFEB___DAC("AFEB_DAC"),
    @XmlEnumValue("AFEB_TimeDelay")
    AFEB___TIME_DELAY("AFEB_TimeDelay"),
    @XmlEnumValue("AFEB_NoiseRate")
    AFEB___NOISE_RATE("AFEB_NoiseRate"),
    @XmlEnumValue("AFEB_Connectivity")
    AFEB___CONNECTIVITY("AFEB_Connectivity"),
    @XmlEnumValue("AFEB_ALCT")
    AFEB___ALCT("AFEB_ALCT"),
    @XmlEnumValue("AFEB_Pulse")
    AFEB___PULSE("AFEB_Pulse"),
    @XmlEnumValue("HV_Scan")
    HV___SCAN("HV_Scan"),
    STEP("STEP"),
    @XmlEnumValue("Debug")
    DEBUG("Debug");
    private final String value;

    RunType(String v) {
        value = v;
    }

    public String value() {
        return value;
    }

    public static RunType fromValue(String v) {
        for (RunType c: RunType.values()) {
            if (c.value.equals(v)) {
                return c;
            }
        }
        throw new IllegalArgumentException(v);
    }

}
