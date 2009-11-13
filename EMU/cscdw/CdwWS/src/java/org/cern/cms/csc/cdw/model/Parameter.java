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
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.OneToMany;
import javax.persistence.Table;

/**
 *
 * @author valdo
 */
@Entity
@Table(name = "CDW_PARAMETERS")
@NamedQueries({@NamedQuery(name = "Parameter.findAll", query = "SELECT p FROM Parameter p"), @NamedQuery(name = "Parameter.findByParId", query = "SELECT p FROM Parameter p WHERE p.parId = :parId"), @NamedQuery(name = "Parameter.findByParDescr", query = "SELECT p FROM Parameter p WHERE p.parDescr = :parDescr")})
public class Parameter implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "PAR_ID")
    private String parId;
    @Basic(optional = false)
    @Column(name = "PAR_DESCR")
    private String parDescr;
    @JoinColumn(name = "PAR_DOM_ID", referencedColumnName = "DOM_ID")
    @ManyToOne(optional = false, fetch = FetchType.LAZY)
    private Domain parDomId;
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "pvaParId", fetch = FetchType.LAZY)
    private List<ParamValue> paramValueCollection;

    public Parameter() {
    }

    public Parameter(String parId) {
        this.parId = parId;
    }

    public Parameter(String parId, String parDescr) {
        this.parId = parId;
        this.parDescr = parDescr;
    }

    public String getParId() {
        return parId;
    }

    public void setParId(String parId) {
        this.parId = parId;
    }

    public String getParDescr() {
        return parDescr;
    }

    public void setParDescr(String parDescr) {
        this.parDescr = parDescr;
    }

    public Domain getParDomId() {
        return parDomId;
    }

    public void setParDomId(Domain parDomId) {
        this.parDomId = parDomId;
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
        hash += (parId != null ? parId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Parameter)) {
            return false;
        }
        Parameter other = (Parameter) object;
        if ((this.parId == null && other.parId != null) || (this.parId != null && !this.parId.equals(other.parId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.Parameter[parId=" + parId + "]";
    }

}
