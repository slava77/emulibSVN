//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
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
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.FactAnn;
import org.cern.cms.csc.dw.model.annotation.OlapDimension;
import org.cern.cms.csc.dw.model.annotation.OlapDimensionSetter;


/**
 * <p>Java class for DqmReportFactType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="DqmReportFactType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/dw/model}factType">
 *       &lt;sequence>
 *         &lt;element name="testId" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "DqmReportFactType", propOrder = {
    "testId"
})
@Entity(name = "DqmReportFact")
@Table(name = "CDW_DQM_REPORT_FACTS")
@FactAnn(title = "DQM Report")
@OlapDimensionSetter(name = "Chamber", propertyName = "component", sharedTable = "CDW_OLAP$D_CHAMBERS")
public class DqmReportFact
    extends Fact
    implements Serializable
{

    protected String testId;

    /**
     * Gets the value of the testId property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCT_TEST_ID", length = 50)
    @OlapDimension(name = "Test Id")
    public String getTestId() {
        return testId;
    }

    /**
     * Sets the value of the testId property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTestId(String value) {
        this.testId = value;
    }

    @Transient
    public boolean isSetTestId() {
        return (this.testId!= null);
    }

}
