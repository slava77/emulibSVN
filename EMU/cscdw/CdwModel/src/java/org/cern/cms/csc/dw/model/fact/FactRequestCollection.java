//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.04.29 at 09:33:27 PM CEST 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import java.util.List;
import java.util.Vector;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
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
 * <p>Java class for factRequestCollectionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="factRequestCollectionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="requestId" type="{http://www.w3.org/2001/XMLSchema}long"/>
 *         &lt;element ref="{http://www.cern.ch/cms/csc/dw/model}factRequest" maxOccurs="unbounded"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "factRequestCollectionType", propOrder = {
    "requestId",
    "factRequest"
})
public class FactRequestCollection
    implements Serializable, Equals, HashCode, ToString
{

    protected long requestId;
    @XmlElement(required = true)
    protected List<FactRequest> factRequest = new Vector<FactRequest>();

    /**
     * Gets the value of the requestId property.
     * 
     */
    public long getRequestId() {
        return requestId;
    }

    /**
     * Sets the value of the requestId property.
     * 
     */
    public void setRequestId(long value) {
        this.requestId = value;
    }

    public boolean isSetRequestId() {
        return true;
    }

    /**
     * Gets the value of the factRequest property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the factRequest property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getFactRequest().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link FactRequest }
     * 
     * 
     */
    public List<FactRequest> getFactRequest() {
        if (factRequest == null) {
            factRequest = new Vector<FactRequest>();
        }
        return this.factRequest;
    }

    /**
     * 
     * 
     */
    public void setFactRequest(List<FactRequest> factRequest) {
        this.factRequest = factRequest;
    }

    public boolean isSetFactRequest() {
        return ((this.factRequest!= null)&&(!this.factRequest.isEmpty()));
    }

    public void unsetFactRequest() {
        this.factRequest = null;
    }

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof FactRequestCollection)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final FactRequestCollection that = ((FactRequestCollection) object);
        equalsBuilder.append(this.getRequestId(), that.getRequestId());
        equalsBuilder.append(this.getFactRequest(), that.getFactRequest());
    }

    public boolean equals(Object object) {
        if (!(object instanceof FactRequestCollection)) {
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
        hashCodeBuilder.append(this.getRequestId());
        hashCodeBuilder.append(this.getFactRequest());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            long theRequestId;
            theRequestId = this.getRequestId();
            toStringBuilder.append("requestId", theRequestId);
        }
        {
            List<FactRequest> theFactRequest;
            theFactRequest = this.getFactRequest();
            toStringBuilder.append("factRequest", theFactRequest);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
