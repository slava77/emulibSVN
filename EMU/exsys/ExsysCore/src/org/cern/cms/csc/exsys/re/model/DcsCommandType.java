//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import javax.xml.bind.annotation.XmlEnum;
import javax.xml.bind.annotation.XmlEnumValue;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for dcsCommandTypeType.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * <p>
 * <pre>
 * &lt;simpleType name="dcsCommandTypeType">
 *   &lt;restriction base="{http://www.w3.org/2001/XMLSchema}string">
 *     &lt;enumeration value="hvChannelForceOn"/>
 *     &lt;enumeration value="hvChannelVset"/>
 *     &lt;enumeration value="hvChannelDisable"/>
 *     &lt;enumeration value="alert"/>
 *     &lt;enumeration value="pong"/>
 *   &lt;/restriction>
 * &lt;/simpleType>
 * </pre>
 * 
 */
@XmlType(name = "dcsCommandTypeType")
@XmlEnum
public enum DcsCommandType {

    @XmlEnumValue("hvChannelForceOn")
    HV_CHANNEL_FORCE_ON("hvChannelForceOn"),
    @XmlEnumValue("hvChannelVset")
    HV_CHANNEL_VSET("hvChannelVset"),
    @XmlEnumValue("hvChannelDisable")
    HV_CHANNEL_DISABLE("hvChannelDisable"),
    @XmlEnumValue("alert")
    ALERT("alert"),
    @XmlEnumValue("pong")
    PONG("pong");
    private final String value;

    DcsCommandType(String v) {
        value = v;
    }

    public String value() {
        return value;
    }

    public static DcsCommandType fromValue(String v) {
        for (DcsCommandType c: DcsCommandType.values()) {
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
