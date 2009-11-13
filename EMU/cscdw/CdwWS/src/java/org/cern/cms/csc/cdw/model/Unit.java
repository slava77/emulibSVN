/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.model;

import java.io.Serializable;
import java.math.BigInteger;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;

/**
 *
 * @author valdo
 */
@Entity
@Table(name = "CDW_UNITS")
@NamedQueries({@NamedQuery(name = "Unit.findAll", query = "SELECT u FROM Unit u"), @NamedQuery(name = "Unit.findByUniId", query = "SELECT u FROM Unit u WHERE u.uniId = :uniId"), @NamedQuery(name = "Unit.findByUniRate", query = "SELECT u FROM Unit u WHERE u.uniRate = :uniRate")})
public class Unit implements Serializable {
    private static final long serialVersionUID = 1L;
    @Id
    @Basic(optional = false)
    @Column(name = "UNI_ID")
    private String uniId;
    @Basic(optional = false)
    @Column(name = "UNI_RATE")
    private BigInteger uniRate;
    @JoinColumn(name = "UNI_BUN_ID", referencedColumnName = "BUN_ID")
    @ManyToOne(optional = false, fetch = FetchType.LAZY)
    private BaseUnit uniBunId;

    public Unit() {
    }

    public Unit(String uniId) {
        this.uniId = uniId;
    }

    public Unit(String uniId, BigInteger uniRate) {
        this.uniId = uniId;
        this.uniRate = uniRate;
    }

    public String getUniId() {
        return uniId;
    }

    public void setUniId(String uniId) {
        this.uniId = uniId;
    }

    public BigInteger getUniRate() {
        return uniRate;
    }

    public void setUniRate(BigInteger uniRate) {
        this.uniRate = uniRate;
    }

    public BaseUnit getUniBunId() {
        return uniBunId;
    }

    public void setUniBunId(BaseUnit uniBunId) {
        this.uniBunId = uniBunId;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (uniId != null ? uniId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof Unit)) {
            return false;
        }
        Unit other = (Unit) object;
        if ((this.uniId == null && other.uniId != null) || (this.uniId != null && !this.uniId.equals(other.uniId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.Unit[uniId=" + uniId + "]";
    }

}
