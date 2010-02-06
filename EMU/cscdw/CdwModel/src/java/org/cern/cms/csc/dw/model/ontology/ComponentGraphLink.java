//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v3.0-03/04/2009 09:20 AM(valikov)-fcs 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2010.02.05 at 02:38:48 PM CET 
//


package org.cern.cms.csc.dw.model.ontology;

import java.io.Serializable;
import java.math.BigInteger;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Inheritance;
import javax.persistence.InheritanceType;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.SequenceGenerator;
import javax.persistence.Table;
import javax.persistence.Transient;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlAttribute;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;
import org.apache.commons.lang.builder.ToStringBuilder;
import org.hibernate.annotations.Cache;
import org.hibernate.annotations.CacheConcurrencyStrategy;
import org.jvnet.jaxb2_commons.lang.Equals;
import org.jvnet.jaxb2_commons.lang.HashCode;
import org.jvnet.jaxb2_commons.lang.ToString;
import org.jvnet.jaxb2_commons.lang.builder.JAXBEqualsBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBHashCodeBuilder;
import org.jvnet.jaxb2_commons.lang.builder.JAXBToStringBuilder;


/**
 * <p>Java class for componentGraphLinkType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="componentGraphLinkType">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="parent" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType"/>
 *         &lt;element name="child" type="{http://www.cern.ch/cms/csc/dw/ontology}componentType"/>
 *         &lt;element name="componentLinkClass" type="{http://www.cern.ch/cms/csc/dw/ontology}componentLinkClassType"/>
 *         &lt;element name="level" type="{http://www.w3.org/2001/XMLSchema}integer"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "componentGraphLinkType", propOrder = {
    "parent",
    "child",
    "componentLinkClass",
    "level"
})
@Entity(name = "org.cern.cms.csc.dw.model.ontology.ComponentGraphLink")
@Table(name = "CDW_COMPONENT_GRAPH_LINKS")
@Inheritance(strategy = InheritanceType.JOINED)
@Cache(usage = CacheConcurrencyStrategy.READ_WRITE, region = "")
public class ComponentGraphLink
    implements Serializable, Equals, HashCode, ToString
{

    @XmlElement(required = true)
    protected Component parent;
    @XmlElement(required = true)
    protected Component child;
    @XmlElement(required = true)
    protected ComponentLinkClass componentLinkClass;
    @XmlElement(required = true)
    protected BigInteger level;
    @XmlAttribute(name = "Id")
    protected Long id;

    /**
     * Gets the value of the parent property.
     * 
     * @return
     *     possible object is
     *     {@link Component }
     *     
     */
    @ManyToOne(targetEntity = Component.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "CGL_PARENT_CMP_ID")
    public Component getParent() {
        return parent;
    }

    /**
     * Sets the value of the parent property.
     * 
     * @param value
     *     allowed object is
     *     {@link Component }
     *     
     */
    public void setParent(Component value) {
        this.parent = value;
    }

    @Transient
    public boolean isSetParent() {
        return (this.parent!= null);
    }

    /**
     * Gets the value of the child property.
     * 
     * @return
     *     possible object is
     *     {@link Component }
     *     
     */
    @ManyToOne(targetEntity = Component.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "CGL_CHILD_CMP_ID")
    public Component getChild() {
        return child;
    }

    /**
     * Sets the value of the child property.
     * 
     * @param value
     *     allowed object is
     *     {@link Component }
     *     
     */
    public void setChild(Component value) {
        this.child = value;
    }

    @Transient
    public boolean isSetChild() {
        return (this.child!= null);
    }

    /**
     * Gets the value of the componentLinkClass property.
     * 
     * @return
     *     possible object is
     *     {@link ComponentLinkClass }
     *     
     */
    @ManyToOne(targetEntity = ComponentLinkClass.class, cascade = {
        CascadeType.ALL
    }, fetch = FetchType.LAZY)
    @JoinColumn(name = "CGL_LCL_ID")
    public ComponentLinkClass getComponentLinkClass() {
        return componentLinkClass;
    }

    /**
     * Sets the value of the componentLinkClass property.
     * 
     * @param value
     *     allowed object is
     *     {@link ComponentLinkClass }
     *     
     */
    public void setComponentLinkClass(ComponentLinkClass value) {
        this.componentLinkClass = value;
    }

    @Transient
    public boolean isSetComponentLinkClass() {
        return (this.componentLinkClass!= null);
    }

    /**
     * Gets the value of the level property.
     * 
     * @return
     *     possible object is
     *     {@link BigInteger }
     *     
     */
    @Basic
    @Column(name = "CGL_LEVEL", precision = 20, scale = 10)
    public BigInteger getLevel() {
        return level;
    }

    /**
     * Sets the value of the level property.
     * 
     * @param value
     *     allowed object is
     *     {@link BigInteger }
     *     
     */
    public void setLevel(BigInteger value) {
        this.level = value;
    }

    @Transient
    public boolean isSetLevel() {
        return (this.level!= null);
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
    @Column(name = "CGL_ID")
    @GeneratedValue(strategy = GenerationType.AUTO)
    @SequenceGenerator(name = "CDW_CGL_ID_SEQ")
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

    public void equals(Object object, EqualsBuilder equalsBuilder) {
        if (!(object instanceof ComponentGraphLink)) {
            equalsBuilder.appendSuper(false);
            return ;
        }
        if (this == object) {
            return ;
        }
        final ComponentGraphLink that = ((ComponentGraphLink) object);
        equalsBuilder.append(this.getParent(), that.getParent());
        equalsBuilder.append(this.getChild(), that.getChild());
        equalsBuilder.append(this.getComponentLinkClass(), that.getComponentLinkClass());
        equalsBuilder.append(this.getLevel(), that.getLevel());
    }

    public boolean equals(Object object) {
        if (!(object instanceof ComponentGraphLink)) {
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
        hashCodeBuilder.append(this.getParent());
        hashCodeBuilder.append(this.getChild());
        hashCodeBuilder.append(this.getComponentLinkClass());
        hashCodeBuilder.append(this.getLevel());
    }

    public int hashCode() {
        final HashCodeBuilder hashCodeBuilder = new JAXBHashCodeBuilder();
        hashCode(hashCodeBuilder);
        return hashCodeBuilder.toHashCode();
    }

    public void toString(ToStringBuilder toStringBuilder) {
        {
            Component theParent;
            theParent = this.getParent();
            toStringBuilder.append("parent", theParent);
        }
        {
            Component theChild;
            theChild = this.getChild();
            toStringBuilder.append("child", theChild);
        }
        {
            ComponentLinkClass theComponentLinkClass;
            theComponentLinkClass = this.getComponentLinkClass();
            toStringBuilder.append("componentLinkClass", theComponentLinkClass);
        }
        {
            BigInteger theLevel;
            theLevel = this.getLevel();
            toStringBuilder.append("level", theLevel);
        }
    }

    public String toString() {
        final ToStringBuilder toStringBuilder = new JAXBToStringBuilder(this);
        toString(toStringBuilder);
        return toStringBuilder.toString();
    }

}
