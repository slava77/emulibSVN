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
@Table(name = "CDW_SOURCES")
@NamedQueries({@NamedQuery(name = "Source.findAll", query = "SELECT s FROM Source s"), @NamedQuery(name = "Source.findBySrcId", query = "SELECT s FROM Source s WHERE s.srcId = :srcId"), @NamedQuery(name = "Source.findBySrcName", query = "SELECT s FROM Source s WHERE s.srcName = :srcName")})
public class Source implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "SRC_ID")
    private String srcId;
    @Basic(optional = false)
    @Column(name = "SRC_NAME")
    private String srcName;
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "pvaSrcId", fetch = FetchType.LAZY)
    private List<ParamValue> paramValueCollection;

    public Source() {
    }

    public Source(String srcId) {
        this.srcId = srcId;
    }

    public Source(String srcId, String srcName) {
        this.srcId = srcId;
        this.srcName = srcName;
    }

    public String getSrcId() {
        return srcId;
    }

    public void setSrcId(String srcId) {
        this.srcId = srcId;
    }

    public String getSrcName() {
        return srcName;
    }

    public void setSrcName(String srcName) {
        this.srcName = srcName;
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
        hash += (srcId != null ? srcId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Source)) {
            return false;
        }
        Source other = (Source) object;
        if ((this.srcId == null && other.srcId != null) || (this.srcId != null && !this.srcId.equals(other.srcId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.Source[srcId=" + srcId + "]";
    }

}
