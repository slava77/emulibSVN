/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.model;

import java.io.Serializable;
import java.util.List;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;

/**
 *
 * @author valdo
 */
@Entity
@Table(name = "CDW_DOMAINS")
@NamedQueries({@NamedQuery(name = "Domain.findAll", query = "SELECT d FROM Domain d"), @NamedQuery(name = "Domain.findByDomId", query = "SELECT d FROM Domain d WHERE d.domId = :domId"), @NamedQuery(name = "Domain.findByDomName", query = "SELECT d FROM Domain d WHERE d.domName = :domName")})
public class Domain implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "DOM_ID")
    private String domId;
    @Basic(optional = false)
    @Column(name = "DOM_NAME")
    private String domName;
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "parDomId", fetch = FetchType.LAZY)
    private List<Parameter> parameterCollection;

    public Domain() {
    }

    public Domain(String domId) {
        this.domId = domId;
    }

    public Domain(String domId, String domName) {
        this.domId = domId;
        this.domName = domName;
    }

    public String getDomId() {
        return domId;
    }

    public void setDomId(String domId) {
        this.domId = domId;
    }

    public String getDomName() {
        return domName;
    }

    public void setDomName(String domName) {
        this.domName = domName;
    }

    public List<Parameter> getParameterCollection() {
        return parameterCollection;
    }

    public void setParameterCollection(List<Parameter> parameterCollection) {
        this.parameterCollection = parameterCollection;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (domId != null ? domId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Domain)) {
            return false;
        }
        Domain other = (Domain) object;
        if ((this.domId == null && other.domId != null) || (this.domId != null && !this.domId.equals(other.domId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.Domain[domId=" + domId + "]";
    }

}
