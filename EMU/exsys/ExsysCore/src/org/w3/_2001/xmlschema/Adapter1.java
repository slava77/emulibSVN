//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2011.05.04 at 11:36:34 AM CEST 
//


package org.w3._2001.xmlschema;

import java.util.Date;
import javax.xml.bind.annotation.adapters.XmlAdapter;

public class Adapter1
    extends XmlAdapter<String, Date>
{


    public Date unmarshal(String value) {
        return (org.cern.cms.csc.dw.util.JaxbDateConverter.parseDate(value));
    }

    public String marshal(Date value) {
        return (org.cern.cms.csc.dw.util.JaxbDateConverter.printDate(value));
    }

}
