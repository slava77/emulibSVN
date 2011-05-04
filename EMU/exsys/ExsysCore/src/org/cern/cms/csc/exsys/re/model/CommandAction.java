//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, vhudson-jaxb-ri-2.1-2 
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
import javax.xml.bind.annotation.XmlSeeAlso;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for commandActionType complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="commandActionType">
 *   &lt;complexContent>
 *     &lt;extension base="{http://www.cern.ch/cms/csc/exsys/re/model}actionType">
 *       &lt;sequence>
 *         &lt;element name="closeConclusionOnExecution" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *       &lt;/sequence>
 *     &lt;/extension>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "commandActionType", propOrder = {
    "closeConclusionOnExecution"
})
@XmlSeeAlso({
    DimPublicationAction.class
})
@Entity(name = "CommandAction")
@Table(name = "RE_COMMAND_ACTIONS")
public abstract class CommandAction
    extends Action
    implements Serializable
{

    protected boolean closeConclusionOnExecution;

    /**
     * Gets the value of the closeConclusionOnExecution property.
     * 
     */
    @Basic
    @Column(name = "RECA_CLOSE_CONCL_ON_EXEC", nullable = false)
    public boolean isCloseConclusionOnExecution() {
        return closeConclusionOnExecution;
    }

    /**
     * Sets the value of the closeConclusionOnExecution property.
     * 
     */
    public void setCloseConclusionOnExecution(boolean value) {
        this.closeConclusionOnExecution = value;
    }

    @Transient
    public boolean isSetCloseConclusionOnExecution() {
        return true;
    }

}
