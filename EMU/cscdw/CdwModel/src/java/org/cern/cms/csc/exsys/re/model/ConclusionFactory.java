//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.05.04 at 05:42:02 PM CEST 
//


package org.cern.cms.csc.exsys.re.model;

import java.io.Serializable;
import java.math.BigInteger;
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
import javax.persistence.OneToMany;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.persistence.UniqueConstraint;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for conclusionFactoryType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="conclusionFactoryType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="name" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *         &lt;element name="certainty" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *         &lt;element name="actionFactories" type="{http://www.cern.ch/cms/csc/exsys/re/model}actionFactoryType" maxOccurs="unbounded" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "conclusionFactoryType", propOrder = {
    "name",
    "certainty",
    "actionFactories"
})
@Entity(name = "org.cern.cms.csc.exsys.re.model.ConclusionFactory")
@Table(name = "RE_CONCLUSION_FACTORIES", uniqueConstraints = {
    @UniqueConstraint(columnNames = {
        "RECF_NAME"
    })
})
@Inheritance(strategy = InheritanceType.JOINED)
public class ConclusionFactory
    extends EntityBase
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected String name;
    @XmlElement(required = true)
    protected BigInteger certainty;
    protected List<ActionFactory> actionFactories = new Vector<ActionFactory>();
    @XmlAttribute(name = "id")
    protected Long id;

    /**
     * Gets the value of the name property.
     * 
     * @return
     *     possible object is
     *     {@link String }
     *     
     */
    @Basic
    @Column(name = "RECF_NAME", length = 64)
    public String getName() {
        return name;
    }

    /**
     * Sets the value of the name property.
     * 
     * @param value
     *     allowed object is
     *     {@link String }
     *     
     */
    public void setName(String value) {
        this.name = value;
    }

    @Transient
    public boolean isSetName() {
        return (this.name!= null);
    }

    /**
     * Gets the value of the certainty property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "RECF_CERTAINTY", precision = 3, scale = 0)
    public BigInteger getCertainty() {
        return certainty;
    }

    /**
     * Sets the value of the certainty property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setCertainty(BigInteger value) {
        this.certainty = value;
    }

    @Transient
    public boolean isSetCertainty() {
        return (this.certainty!= null);
    }

    /**
     * Gets the value of the actionFactories property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the actionFactories property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getActionFactories().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link ActionFactory }
     * 
     * 
     */
    @OneToMany(targetEntity = ActionFactory.class, cascade = {
        CascadeType.ALL
    }, mappedBy = "conclusionFactory")
    @JoinColumn(name = "ACTIONFACTORIES_CONCLUSIONFA_0")
    public List<ActionFactory> getActionFactories() {
        if (actionFactories == null) {
            actionFactories = new Vector<ActionFactory>();
        }
        return this.actionFactories;
    }

    /**
     * 
     * 
     */
    public void setActionFactories(List<ActionFactory> actionFactories) {
        this.actionFactories = actionFactories;
    }

    @Transient
    public boolean isSetActionFactories() {
        return ((this.actionFactories!= null)&&(!this.actionFactories.isEmpty()));
    }

    public void unsetActionFactories() {
        this.actionFactories = null;
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
    @Column(name = "RECF_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "RE_CONCL_FACTORY_ID_SEQ", sequenceName = "RE_CONCL_FACTORY_ID_SEQ")
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

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof ConclusionFactory)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final ConclusionFactory that = ((ConclusionFactory) object);
        equalsBuilder.append(this.getName(), that.getName());
        equalsBuilder.append(this.getCertainty(), that.getCertainty());
        equalsBuilder.append(this.getActionFactories(), that.getActionFactories());
    }

    public boolean equals(Object object) {
        if (!(object instanceof ConclusionFactory)) {
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
        hashCodeBuilder.append(this.getName());
        hashCodeBuilder.append(this.getCertainty());
        hashCodeBuilder.append(this.getActionFactories());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            String theName;
            theName = this.getName();
            toStringBuilder.append("name", theName);
        }
        {
            BigInteger theCertainty;
            theCertainty = this.getCertainty();
            toStringBuilder.append("certainty", theCertainty);
        }
        {
            List<ActionFactory> theActionFactories;
            theActionFactories = this.getActionFactories();
            toStringBuilder.append("actionFactories", theActionFactories);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
