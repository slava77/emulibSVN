//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a>
// Any modifications to this file will be lost upon recompilation of the source schema.
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElementRef;
import org.jvnet.hyperjaxb3.item.Item;
import org.jvnet.hyperjaxb3.xml.bind.JAXBElementUtils;

@XmlAccessorType(XmlAccessType.FIELD)
@Entity(name = "org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem")
@Table(name = "CDW_FACT_COLLECTION_ITEMS")
@Inheritance(strategy = InheritanceType.JOINED)
public class FactCollectionFactsItem
    implements Serializable, Item<JAXBElement<? extends org.cern.cms.csc.dw.model.fact.Fact>>
{

    @XmlElementRef(name = "fact", namespace = "http://www.cern.ch/cms/csc/dw/model", type = JAXBElement.class)
    protected JAXBElement<? extends org.cern.cms.csc.dw.model.fact.Fact> item;
    @XmlAttribute(name = "Id")
    protected Long id;

    /**
     *
     *
     * @return
     *     possible object is
     *     {@link JAXBElement }{@code <}{@link ApplicationStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsAlctLvFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DduFmmIrqFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link CumulativeTmbTriggerCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link FedFsmFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link EmuMonitorFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsCommunicationStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscHvSegmentFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsCfebLvFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmDduInputFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmEmuFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmReportFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscAfebFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmDduFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link TmbCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DduFmmResetFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link LocalDAQStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link SlidingTmbTriggerCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link org.cern.cms.csc.dw.model.fact.Fact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscCfebFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscFact }{@code >}
     *
     */
    @Transient
    public JAXBElement<? extends org.cern.cms.csc.dw.model.fact.Fact> getItem() {
        return item;
    }

    /**
     *
     *
     * @param value
     *     allowed object is
     *     {@link JAXBElement }{@code <}{@link ApplicationStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsAlctLvFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DduFmmIrqFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link CumulativeTmbTriggerCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link FedFsmFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link EmuMonitorFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsCommunicationStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscHvSegmentFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DcsCfebLvFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmDduInputFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmEmuFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmReportFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscAfebFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmDduFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link TmbCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DduFmmResetFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link LocalDAQStatusFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link SlidingTmbTriggerCounterFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link org.cern.cms.csc.dw.model.fact.Fact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscCfebFact }{@code >}
     *     {@link JAXBElement }{@code <}{@link DqmCscFact }{@code >}
     *
     */
    public void setItem(JAXBElement<? extends org.cern.cms.csc.dw.model.fact.Fact> value) {
        this.item = ((JAXBElement<? extends org.cern.cms.csc.dw.model.fact.Fact> ) value);
    }

    /**
     * Gets the value of the id property.
     *
     * @return
     *     possible object is
     *     {@link Long }
     *
     */
    @Id
    @Column(name = "FIT_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    public Long getId() {
        return id;
    }

    /**
     * Sets the value of the id property.
     *
     * @param value
     *     allowed object is
     *     {@link Long }
     *
     */
    public void setId(Long value) {
        this.id = value;
    }

    @Basic
    @Column(name = "FIT_TYPE")
    public String getItemName() {
        if (this.getItem() instanceof JAXBElement) {
            return JAXBElementUtils.getName(((JAXBElement<org.cern.cms.csc.dw.model.fact.Fact> ) this.getItem()));
        } else {
            return null;
        }
    }

    public void setItemName(String target) {
        if (target!= null) {
            setItem(JAXBElementUtils.wrap(this.getItem(), target, org.cern.cms.csc.dw.model.fact.Fact.class));
        }
    }

    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.fact.Fact.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "FIT_FCT_ID")
    public org.cern.cms.csc.dw.model.fact.Fact getItemValue() {
        if (this.getItem() instanceof JAXBElement) {
            return JAXBElementUtils.getValue(((JAXBElement<org.cern.cms.csc.dw.model.fact.Fact> ) this.getItem()));
        } else {
            return null;
        }
    }

    public void setItemValue(org.cern.cms.csc.dw.model.fact.Fact target) {
        if (target!= null) {
            setItem(JAXBElementUtils.wrap(this.getItem(), target));
        }
    }

}
