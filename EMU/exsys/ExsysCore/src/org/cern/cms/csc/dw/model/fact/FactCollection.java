//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
//


package org.cern.cms.csc.dw.model.fact;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Vector;
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
import javax.persistence.OneToMany;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlElementRef;
import javax.xml.bind.annotation.XmlSchemaType;
import javax.xml.bind.annotation.XmlType;
import javax.xml.bind.annotation.adapters.XmlJavaTypeAdapter;
import org.cern.cms.csc.dw.model.annotation.ComponentId;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.service.ServiceInstructions;
import org.jvnet.hyperjaxb3.item.ItemUtils;
import org.w3._2001.xmlschema.Adapter1;


/**
 * <p>Java class for factCollectionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="factCollectionType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element ref="{http://www.cern.ch/cms/csc/dw/service}serviceInstructions" minOccurs="0"/>
 *         &lt;element name="source" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="requestId" type="{http://www.w3.org/2001/XMLSchema}long" minOccurs="0"/>
 *         &lt;element name="time" type="{http://www.w3.org/2001/XMLSchema}dateTime" minOccurs="0"/>
 *         &lt;element name="component" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType" minOccurs="0"/>
 *         &lt;element ref="{http://www.cern.ch/cms/csc/dw/model}fact" maxOccurs="unbounded"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "factCollectionType", propOrder = {
    "serviceInstructions",
    "source",
    "request",
    "time",
    "component",
    "facts"
})
@Entity(name = "org.cern.cms.csc.dw.model.fact.FactCollection")
@Table(name = "CDW_FACT_COLLECTIONS")
@Inheritance(strategy = InheritanceType.JOINED)
public class FactCollection
    extends EntityBase
    implements Serializable
{

    @XmlElement(namespace = "http://www.cern.ch/cms/csc/dw/service")
    protected ServiceInstructions serviceInstructions;
    @XmlElement(required = true)
    protected String source;
    @XmlElement(name = "requestId")
    protected Long request;
    @XmlElement(type = String.class)
    @XmlJavaTypeAdapter(Adapter1 .class)
    @XmlSchemaType(name = "dateTime")
    protected Date time;
    protected org.cern.cms.csc.dw.model.ontology.Component component;
    @XmlElementRef(name = "fact", namespace = "http://www.cern.ch/cms/csc/dw/model", type = JAXBElement.class)
    protected List<JAXBElement<? extends Fact>> facts = new Vector<JAXBElement<? extends Fact>>();
    @XmlAttribute(name = "id")
    protected Long id;
    protected transient List<org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem> factsItems;

    /**
     * Gets the value of the serviceInstructions property.
     * 
     * @return
     *     possible object is
     *     {@link ServiceInstructions }
     *     
     */
    @Transient
    public ServiceInstructions getServiceInstructions() {
        return serviceInstructions;
    }

    /**
     * Sets the value of the serviceInstructions property.
     * 
     * @param value
     *     allowed object is
     *     {@link ServiceInstructions }
     *     
     */
    public void setServiceInstructions(ServiceInstructions value) {
        this.serviceInstructions = value;
    }

    @Transient
    public boolean isSetServiceInstructions() {
        return (this.serviceInstructions!= null);
    }

    /**
     * Gets the value of the source property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Transient
    @ComponentId(forProperty = "component", limitComponents = {
        "FactProvider"
    }, limitComponentsRecursive = true, required = true)
    public String getSource() {
        return source;
    }

    /**
     * Sets the value of the source property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setSource(String value) {
        this.source = value;
    }

    @Transient
    public boolean isSetSource() {
        return (this.source!= null);
    }

    /**
     * Gets the value of the request property.
     * 
     * @return
     *     possible object is
     *     {@link Long }
     *     
     */
    @Basic
    @Column(name = "FCO_REQUEST_ID", precision = 20, scale = 10)
    public Long getRequest() {
        return request;
    }

    /**
     * Sets the value of the request property.
     * 
     * @param value
     *     allowed object is
     *     {@link Long }
     *     
     */
    public void setRequest(Long value) {
        this.request = value;
    }

    @Transient
    public boolean isSetRequest() {
        return (this.request!= null);
    }

    /**
     * Gets the value of the time property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "FCO_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    public Date getTime() {
        return time;
    }

    /**
     * Sets the value of the time property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setTime(Date value) {
        this.time = value;
    }

    @Transient
    public boolean isSetTime() {
        return (this.time!= null);
    }

    /**
     * Gets the value of the component property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.ontology.Component }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.Component.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "FCO_CMP_ID")
    public org.cern.cms.csc.dw.model.ontology.Component getComponent() {
        return component;
    }

    /**
     * Sets the value of the component property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.ontology.Component }
     *     
     */
    public void setComponent(org.cern.cms.csc.dw.model.ontology.Component value) {
        this.component = value;
    }

    @Transient
    public boolean isSetComponent() {
        return (this.component!= null);
    }

    /**
     * Gets the value of the facts property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the facts property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getFacts().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link JAXBElement }{@code <}{@link LocalDAQStatusFact }{@code >}
     * {@link JAXBElement }{@code <}{@link ApplicationStatusFact }{@code >}
     * {@link JAXBElement }{@code <}{@link CumulativeTmbTriggerCounterFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmEmuFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DduFmmIrqFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmCscFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmCscAfebFact }{@code >}
     * {@link JAXBElement }{@code <}{@link Fact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmDduFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmDduInputFact }{@code >}
     * {@link JAXBElement }{@code <}{@link EmuMonitorFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmReportFact }{@code >}
     * {@link JAXBElement }{@code <}{@link FedFsmFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsCommunicationStatusFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsAlctLvFact }{@code >}
     * {@link JAXBElement }{@code <}{@link TmbCounterFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmCscHvSegmentFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsDisableEnableFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsAlertFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DduFmmResetFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DqmCscCfebFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsPingFact }{@code >}
     * {@link JAXBElement }{@code <}{@link SlidingTmbTriggerCounterFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsCfebLvFact }{@code >}
     * {@link JAXBElement }{@code <}{@link DcsHVOnVoltageFact }{@code >}
     * 
     * 
     */
    @Transient
    public List<JAXBElement<? extends Fact>> getFacts() {
        if (facts == null) {
            facts = new Vector<JAXBElement<? extends Fact>>();
        }
        return this.facts;
    }

    /**
     * 
     * 
     */
    public void setFacts(List<JAXBElement<? extends Fact>> facts) {
        this.facts = facts;
    }

    @Transient
    public boolean isSetFacts() {
        return ((this.facts!= null)&&(!this.facts.isEmpty()));
    }

    public void unsetFacts() {
        this.facts = null;
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
    @Column(name = "FCO_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    public Long getid() {
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
    public void setid(Long value) {
        this.id = value;
    }

    @OneToMany(targetEntity = org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem.class, cascade = {
        CascadeType.ALL
    })
    @JoinColumn(name = "FIT_FCO_ID")
    public List<org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem> getFactsItems() {
        if (this.factsItems == null) {
            this.factsItems = new ArrayList<org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.facts)) {
            this.facts = ItemUtils.wrap(this.facts, this.factsItems, org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem.class);
        }
        return this.factsItems;
    }

    public void setFactsItems(List<org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem> value) {
        this.facts = null;
        this.factsItems = null;
        this.factsItems = value;
        if (this.factsItems == null) {
            this.factsItems = new ArrayList<org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem>();
        }
        if (ItemUtils.shouldBeWrapped(this.facts)) {
            this.facts = ItemUtils.wrap(this.facts, this.factsItems, org.cern.cms.csc.dw.model.fact.FactCollectionFactsItem.class);
        }
    }
    
//--simple--preserve

    @Override
    public void onReceive(org.cern.cms.csc.dw.dao.EntityDaoLocal eDao) throws org.cern.cms.csc.dw.exception.OnReceiveProcessingException {
        super.onReceive(eDao);
        setTime(new Date());
    }

    @Override
    public void onSave(org.cern.cms.csc.dw.dao.EntityDaoLocal eDao, boolean queued) throws org.cern.cms.csc.dw.exception.OnSaveProcessingException {
        factsItems = new ArrayList<FactCollectionFactsItem>();
        for (JAXBElement<? extends Fact> fi: getFacts()) {
            Fact fact = fi.getValue();
            fact.onSave(eDao, queued);
            FactCollectionFactsItem fcfi = new FactCollectionFactsItem();
            fcfi.setItemValue(fact);
            factsItems.add(fcfi);
        }
    }

//--simple--preserve

}
