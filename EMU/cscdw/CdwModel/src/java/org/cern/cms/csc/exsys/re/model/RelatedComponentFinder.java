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
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.cern.cms.csc.dw.model.annotation.gui.ImmutableReference;


/**
 * <p>Java class for relatedComponentFinderType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="relatedComponentFinderType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/exsys/re/model}componentFinderType">
 *       &lt;sequence>
 *         &lt;element name="linkClass" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassType" minOccurs="0"/>
 *         &lt;element name="mergePolicy" type="{http://www.cern.ch/cms/csc/exsys/re/model}mergePolicyType"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "relatedComponentFinderType", propOrder = {
    "linkClass",
    "mergePolicy"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.RelatedComponentFinder")
public class RelatedComponentFinder
    extends ComponentFinder
    implements Serializable
{

    @ImmutableReference
    @org.cern.cms.csc.dw.model.annotation.gui.Label(description = "(Mandaroty) This link class will be used to find the related components. Note that it's also mandatory to specify component class(es)", name = "Link Class")
    protected org.cern.cms.csc.dw.model.ontology.ComponentLinkClass linkClass;
    @XmlElement(required = true)
    @org.cern.cms.csc.dw.model.annotation.gui.Label(description = "This is used to tell the component resolver what to do when multiple source components are given. With union merge policy, all related components are simply added together, while with intersection policy all related component sets retrieved from different source components are intersected.", name = "Merge Policy")
    protected MergePolicy mergePolicy;

    /**
     * Gets the value of the linkClass property.
     * 
     * @return
     *     possible object is
     *     {@link org.cern.cms.csc.dw.model.ontology.ComponentLinkClass }
     *     
     */
    @ManyToOne(targetEntity = org.cern.cms.csc.dw.model.ontology.ComponentLinkClass.class)
    @JoinColumn(name = "RCF_RELATING_LINK_CLASS")
    public org.cern.cms.csc.dw.model.ontology.ComponentLinkClass getLinkClass() {
        return linkClass;
    }

    /**
     * Sets the value of the linkClass property.
     * 
     * @param value
     *     allowed object is
     *     {@link org.cern.cms.csc.dw.model.ontology.ComponentLinkClass }
     *     
     */
    public void setLinkClass(org.cern.cms.csc.dw.model.ontology.ComponentLinkClass value) {
        this.linkClass = value;
    }

    @Transient
    public boolean isSetLinkClass() {
        return (this.linkClass!= null);
    }

    /**
     * Gets the value of the mergePolicy property.
     * 
     * @return
     *     possible object is
     *     {@link MergePolicy }
     *     
     */
    @Transient
    public MergePolicy getMergePolicy() {
        return mergePolicy;
    }

    /**
     * Sets the value of the mergePolicy property.
     * 
     * @param value
     *     allowed object is
     *     {@link MergePolicy }
     *     
     */
    public void setMergePolicy(MergePolicy value) {
        this.mergePolicy = value;
    }

    @Transient
    public boolean isSetMergePolicy() {
        return (this.mergePolicy!= null);
    }

    @Basic
    @Column(name = "RCF_MERGE_POLICY", length = 12)
    public String getMergePolicyItem() {
        return ((this.getMergePolicy() == null)?null:this.getMergePolicy().value());
    }

    public void setMergePolicyItem(String target) {
        setMergePolicy(((target == null)?null:MergePolicy.fromValue(target)));
    }

}
