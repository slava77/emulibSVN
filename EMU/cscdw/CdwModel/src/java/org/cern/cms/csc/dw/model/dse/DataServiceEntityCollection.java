//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.04 at 05:42:02 PM CEST 
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
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


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
    implements Serializable, Equals, HashCode, ToString
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

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof DataServiceEntityCollection)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final DataServiceEntityCollection that = ((DataServiceEntityCollection) object);
        equalsBuilder.append(this.getDataServiceEntities(), that.getDataServiceEntities());
    }

    public boolean equals(Object object) {
        if (!(object instanceof DataServiceEntityCollection)) {
            return false;
        }
        if (this == object) {
            return true;
        }
        final EqualsBuilder equalsBuilder = new JAXBEqualsBuilder();
        equals(object, equalsBuilder);
        return equalsBuilder.isEquals();
    }

    public void hashCode(HashCodeBuilder hashCodeBuilder) {
        hashCodeBuilder.append(this.getDataServiceEntities());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            List<JAXBElement<? extends DataServiceEntity>> theDataServiceEntities;
            theDataServiceEntities = this.getDataServiceEntities();
            toStringBuilder.append("dataServiceEntities", theDataServiceEntities);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
