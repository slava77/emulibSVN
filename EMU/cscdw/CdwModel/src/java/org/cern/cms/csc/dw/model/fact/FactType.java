//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.02.05 at 02:38:48 PM CET 
//


package org.cern.cms.csc.dw.model.fact;

import javax.xml.bind.annotation.XmlEnum;
import javax.xml.bind.annotation.XmlEnumValue;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for factTypeType.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * <p>
 * <pre>
 * &lt;simpleType name="factTypeType">
 *   &lt;restriction base="{http://www.w3.org/2001/XMLSchema}string">
 *     &lt;enumeration value="SlidingTmbTriggerCounterFact"/>
 *     &lt;enumeration value="CumulativeTmbTriggerCounterFact"/>
 *     &lt;enumeration value="DqmCscAfebFact"/>
 *     &lt;enumeration value="DqmCscCfebFact"/>
 *     &lt;enumeration value="DqmCscFact"/>
 *     &lt;enumeration value="DqmCscHvSegmentFact"/>
 *     &lt;enumeration value="DqmDduFact"/>
 *     &lt;enumeration value="DqmDduInputFact"/>
 *     &lt;enumeration value="DqmEmuFact"/>
 *   &lt;/restriction>
 * &lt;/simpleType>
 * </pre>
 * 
 */
@XmlType(name = "factTypeType")
@XmlEnum
public enum FactType {

    @XmlEnumValue("SlidingTmbTriggerCounterFact")
    SLIDING_TMB_TRIGGER_COUNTER_FACT("SlidingTmbTriggerCounterFact"),
    @XmlEnumValue("CumulativeTmbTriggerCounterFact")
    CUMULATIVE_TMB_TRIGGER_COUNTER_FACT("CumulativeTmbTriggerCounterFact"),
    @XmlEnumValue("DqmCscAfebFact")
    DQM_CSC_AFEB_FACT("DqmCscAfebFact"),
    @XmlEnumValue("DqmCscCfebFact")
    DQM_CSC_CFEB_FACT("DqmCscCfebFact"),
    @XmlEnumValue("DqmCscFact")
    DQM_CSC_FACT("DqmCscFact"),
    @XmlEnumValue("DqmCscHvSegmentFact")
    DQM_CSC_HV_SEGMENT_FACT("DqmCscHvSegmentFact"),
    @XmlEnumValue("DqmDduFact")
    DQM_DDU_FACT("DqmDduFact"),
    @XmlEnumValue("DqmDduInputFact")
    DQM_DDU_INPUT_FACT("DqmDduInputFact"),
    @XmlEnumValue("DqmEmuFact")
    DQM_EMU_FACT("DqmEmuFact");
    private final String value;

    FactType(String v) {
        value = v;
    }

    public String value() {
        return value;
    }

    public static FactType fromValue(String v) {
        for (FactType c: FactType.values()) {
            if (c.value.equals(v)) {
                return c;
            }
        }
        throw new IllegalArgumentException(v);
    }

}
