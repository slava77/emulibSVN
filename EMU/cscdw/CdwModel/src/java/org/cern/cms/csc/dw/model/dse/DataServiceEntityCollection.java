//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.19 at 11:17:22 AM CEST 
//


package org.cern.cms.csc.dw.model.dse;

import java.io.Serializable;
import java.util.List;
import java.util.Vector;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElementRef;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for dataServiceEntityCollectionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="dataServiceEntityCollectionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element ref="{http://www.cern.ch/cms/csc/dw/model/dse}dataServiceEntity" maxOccurs="unbounded"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "dataServiceEntityCollectionType", propOrder = {
    "dataServiceEntities"
})
public class DataServiceEntityCollection
    implements Serializable
{

    @XmlElementRef(name = "dataServiceEntity", namespace = "http://www.cern.ch/cms/csc/dw/model/dse", type = JAXBElement.class)
    protected List<JAXBElement<? extends DataServiceEntity>> dataServiceEntities = new Vector<JAXBElement<? extends DataServiceEntity>>();

    /**
     * Gets the value of the dataServiceEntities property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the dataServiceEntities property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getDataServiceEntities().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link JAXBElement }{@code <}{@link TmbCounterDescriptionDSE }{@code >}
     * {@link JAXBElement }{@code <}{@link DataServiceEntity }{@code >}
     * 
     * 
     */
    public List<JAXBElement<? extends DataServiceEntity>> getDataServiceEntities() {
        if (dataServiceEntities == null) {
            dataServiceEntities = new Vector<JAXBElement<? extends DataServiceEntity>>();
        }
        return this.dataServiceEntities;
    }

    /**
     * 
     * 
     */
    public void setDataServiceEntities(List<JAXBElement<? extends DataServiceEntity>> dataServiceEntities) {
        this.dataServiceEntities = dataServiceEntities;
    }

    public boolean isSetDataServiceEntities() {
        return ((this.dataServiceEntities!= null)&&(!this.dataServiceEntities.isEmpty()));
    }

    public void unsetDataServiceEntities() {
        this.dataServiceEntities = null;
    }

}
