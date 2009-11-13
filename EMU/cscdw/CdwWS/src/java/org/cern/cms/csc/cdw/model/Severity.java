/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.model;

import java.io.Serializable;
import java.util.List;
import javax.persistence.Basic;
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
@Table(name = "CDW_SEVERITIES")
@NamedQueries({@NamedQuery(name = "Severity.findAll", query = "SELECT s FROM Severity s"), @NamedQuery(name = "Severity.findBySevId", query = "SELECT s FROM Severity s WHERE s.sevId = :sevId")})
public class Severity implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "SEV_ID")
    private String sevId;
    @OneToMany(mappedBy = "pvaSevId", fetch = FetchType.LAZY)
    private List<ParamValue> paramValueCollection;

    public Severity() {
    }

    public Severity(String sevId) {
        this.sevId = sevId;
    }

    public String getSevId() {
        return sevId;
    }

    public void setSevId(String sevId) {
        this.sevId = sevId;
    }

    public List<ParamValue> getParamValueCollection() {
        return paramValueCollection;
    }

    public void setParamValueCollection(List<ParamValue> paramValueCollection) {
        this.paramValueCollection = paramValueCollection;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (sevId != null ? sevId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Severity)) {
            return false;
        }
        Severity other = (Severity) object;
        if ((this.sevId == null && other.sevId != null) || (this.sevId != null && !this.sevId.equals(other.sevId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.Severity[sevId=" + sevId + "]";
    }

}
