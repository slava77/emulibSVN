//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for dimPublicationActionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="dimPublicationActionType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/exsys/re/model}commandActionType">
 *       &lt;sequence>
 *         &lt;element name="serviceName" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="dataType" type="{http://www.cern.ch/cms/csc/exsys/re/model}dimServiceDataTypeType"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "dimPublicationActionType", propOrder = {
    "serviceName",
    "dataType"
})
@XmlSeeAlso({
    DcsCommandAction.class
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.DimPublicationAction")
@Table(name = "RE_DIM_PUBLICATION_ACTIONS")
public abstract class DimPublicationAction
    extends CommandAction
    implements Serializable
{

    @XmlElement(required = true)
    protected String serviceName;
    @XmlElement(required = true)
    protected DimServiceDataType dataType;

    /**
     * Gets the value of the serviceName property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "REA_DIM_SERVICE_NAME", nullable = false, length = 128)
    public String getServiceName() {
        return serviceName;
    }

    /**
     * Sets the value of the serviceName property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setServiceName(String value) {
        this.serviceName = value;
    }

    @Transient
    public boolean isSetServiceName() {
        return (this.serviceName!= null);
    }

    /**
     * Gets the value of the dataType property.
     * 
     * @return
     *     possible object is
     *     {@link DimServiceDataType }
     *     
     */
    @Transient
    public DimServiceDataType getDataType() {
        return dataType;
    }

    /**
     * Sets the value of the dataType property.
     * 
     * @param value
     *     allowed object is
     *     {@link DimServiceDataType }
     *     
     */
    public void setDataType(DimServiceDataType value) {
        this.dataType = value;
    }

    @Transient
    public boolean isSetDataType() {
        return (this.dataType!= null);
    }

    @Basic
    @Column(name = "REA_DIM_DATA_TYPE", nullable = false, length = 20)
    public String getDataTypeItem() {
        return ((this.getDataType() == null)?null:this.getDataType().value());
    }

    public void setDataTypeItem(String target) {
        setDataType(((target == null)?null:DimServiceDataType.fromValue(target)));
    }

}
