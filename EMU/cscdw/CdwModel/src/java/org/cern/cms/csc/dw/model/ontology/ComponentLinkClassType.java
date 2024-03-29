//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.ontology;

import javax.xml.bind.annotation.XmlEnum;
import javax.xml.bind.annotation.XmlEnumValue;
import javax.xml.bind.annotation.XmlType;
import org.neo4j.graphdb.RelationshipType;


/**
 * <p>Java class for componentLinkClassIdType.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * <p>
 * <pre>
 * &lt;simpleType name="componentLinkClassIdType">
 *   &lt;restriction base="{http://www.w3.org/2001/XMLSchema}string">
 *     &lt;enumeration value="consistsOf"/>
 *     &lt;enumeration value="contains"/>
 *     &lt;enumeration value="controlPath"/>
 *     &lt;enumeration value="controls"/>
 *     &lt;enumeration value="coolingPath"/>
 *     &lt;enumeration value="cools"/>
 *     &lt;enumeration value="dataPath"/>
 *     &lt;enumeration value="deliversGasTo"/>
 *     &lt;enumeration value="deliversHVTo"/>
 *     &lt;enumeration value="deliversLVTo"/>
 *     &lt;enumeration value="gasSupplyPath"/>
 *     &lt;enumeration value="getsGasFrom"/>
 *     &lt;enumeration value="getsHVFrom"/>
 *     &lt;enumeration value="getsLVFrom"/>
 *     &lt;enumeration value="isControlledBy"/>
 *     &lt;enumeration value="isCooledBy"/>
 *     &lt;enumeration value="isIn"/>
 *     &lt;enumeration value="isMonitoredBy"/>
 *     &lt;enumeration value="isPartOf"/>
 *     &lt;enumeration value="locationPath"/>
 *     &lt;enumeration value="logicalComposition"/>
 *     &lt;enumeration value="monitorPath"/>
 *     &lt;enumeration value="monitors"/>
 *     &lt;enumeration value="powerSupplyPath"/>
 *     &lt;enumeration value="receivesDataFrom"/>
 *     &lt;enumeration value="receivesL1AFrom"/>
 *     &lt;enumeration value="receivesTriggerFrom"/>
 *     &lt;enumeration value="sendsDataTo"/>
 *     &lt;enumeration value="sendsL1ATo"/>
 *     &lt;enumeration value="sendsTriggerTo"/>
 *     &lt;enumeration value="triggerPath"/>
 *   &lt;/restriction>
 * &lt;/simpleType>
 * </pre>
 * 
 */
@XmlType(name = "componentLinkClassIdType")
@XmlEnum
public enum ComponentLinkClassType
    implements RelationshipType
{

    @XmlEnumValue("consistsOf")
    CONSISTS_OF("consistsOf"),
    @XmlEnumValue("contains")
    CONTAINS("contains"),
    @XmlEnumValue("controlPath")
    CONTROL_PATH("controlPath"),
    @XmlEnumValue("controls")
    CONTROLS("controls"),
    @XmlEnumValue("coolingPath")
    COOLING_PATH("coolingPath"),
    @XmlEnumValue("cools")
    COOLS("cools"),
    @XmlEnumValue("dataPath")
    DATA_PATH("dataPath"),
    @XmlEnumValue("deliversGasTo")
    DELIVERS_GAS_TO("deliversGasTo"),
    @XmlEnumValue("deliversHVTo")
    DELIVERS_HV_TO("deliversHVTo"),
    @XmlEnumValue("deliversLVTo")
    DELIVERS_LV_TO("deliversLVTo"),
    @XmlEnumValue("gasSupplyPath")
    GAS_SUPPLY_PATH("gasSupplyPath"),
    @XmlEnumValue("getsGasFrom")
    GETS_GAS_FROM("getsGasFrom"),
    @XmlEnumValue("getsHVFrom")
    GETS_HV_FROM("getsHVFrom"),
    @XmlEnumValue("getsLVFrom")
    GETS_LV_FROM("getsLVFrom"),
    @XmlEnumValue("isControlledBy")
    IS_CONTROLLED_BY("isControlledBy"),
    @XmlEnumValue("isCooledBy")
    IS_COOLED_BY("isCooledBy"),
    @XmlEnumValue("isIn")
    IS_IN("isIn"),
    @XmlEnumValue("isMonitoredBy")
    IS_MONITORED_BY("isMonitoredBy"),
    @XmlEnumValue("isPartOf")
    IS_PART_OF("isPartOf"),
    @XmlEnumValue("locationPath")
    LOCATION_PATH("locationPath"),
    @XmlEnumValue("logicalComposition")
    LOGICAL_COMPOSITION("logicalComposition"),
    @XmlEnumValue("monitorPath")
    MONITOR_PATH("monitorPath"),
    @XmlEnumValue("monitors")
    MONITORS("monitors"),
    @XmlEnumValue("powerSupplyPath")
    POWER_SUPPLY_PATH("powerSupplyPath"),
    @XmlEnumValue("receivesDataFrom")
    RECEIVES_DATA_FROM("receivesDataFrom"),
    @XmlEnumValue("receivesL1AFrom")
    RECEIVES_L_1_A_FROM("receivesL1AFrom"),
    @XmlEnumValue("receivesTriggerFrom")
    RECEIVES_TRIGGER_FROM("receivesTriggerFrom"),
    @XmlEnumValue("sendsDataTo")
    SENDS_DATA_TO("sendsDataTo"),
    @XmlEnumValue("sendsL1ATo")
    SENDS_L_1_A_TO("sendsL1ATo"),
    @XmlEnumValue("sendsTriggerTo")
    SENDS_TRIGGER_TO("sendsTriggerTo"),
    @XmlEnumValue("triggerPath")
    TRIGGER_PATH("triggerPath");
    private final String value;

    ComponentLinkClassType(String v) {
        value = v;
    }

    public String value() {
        return value;
    }

    public static ComponentLinkClassType fromValue(String v) {
        for (ComponentLinkClassType c: ComponentLinkClassType.values()) {
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
