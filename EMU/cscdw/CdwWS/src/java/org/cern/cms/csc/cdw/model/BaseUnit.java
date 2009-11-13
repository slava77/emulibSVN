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
@Table(name = "CDW_BASE_UNITS")
@NamedQueries({@NamedQuery(name = "BaseUnit.findAll", query = "SELECT b FROM BaseUnit b"), @NamedQuery(name = "BaseUnit.findByBunId", query = "SELECT b FROM BaseUnit b WHERE b.bunId = :bunId")})
public class BaseUnit implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "BUN_ID")
    private String bunId;
    @OneToMany(mappedBy = "pvaBunId", fetch = FetchType.LAZY)
    private List<ParamValue> paramValueCollection;
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "uniBunId", fetch = FetchType.LAZY)
    private List<Unit> unitCollection;

    public BaseUnit() {
    }

    public BaseUnit(String bunId) {
        this.bunId = bunId;
    }

    public String getBunId() {
        return bunId;
    }

    public void setBunId(String bunId) {
        this.bunId = bunId;
    }

    public List<ParamValue> getParamValueCollection() {
        return paramValueCollection;
    }

    public void setParamValueCollection(List<ParamValue> paramValueCollection) {
        this.paramValueCollection = paramValueCollection;
    }

    public List<Unit> getUnitCollection() {
        return unitCollection;
    }

    public void setUnitCollection(List<Unit> unitCollection) {
        this.unitCollection = unitCollection;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (bunId != null ? bunId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof BaseUnit)) {
            return false;
        }
        BaseUnit other = (BaseUnit) object;
        if ((this.bunId == null && other.bunId != null) || (this.bunId != null && !this.bunId.equals(other.bunId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.BaseUnit[bunId=" + bunId + "]";
    }

}
