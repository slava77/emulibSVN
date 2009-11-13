/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.model;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.Date;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.FetchType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.Lob;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.SequenceGenerator;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;

/**
 *
 * @author valdo
 */
@Entity
@Table(name = "CDW_PARAM_VALUES")
@NamedQueries({@NamedQuery(name = "ParamValue.findAll", query = "SELECT p FROM ParamValue p"), @NamedQuery(name = "ParamValue.findByPvaId", query = "SELECT p FROM ParamValue p WHERE p.pvaId = :pvaId"), @NamedQuery(name = "ParamValue.findByPvaSourceTime", query = "SELECT p FROM ParamValue p WHERE p.pvaSourceTime = :pvaSourceTime"), @NamedQuery(name = "ParamValue.findByPvaCreateTime", query = "SELECT p FROM ParamValue p WHERE p.pvaCreateTime = :pvaCreateTime"), @NamedQuery(name = "ParamValue.findByPvaComponentId", query = "SELECT p FROM ParamValue p WHERE p.pvaComponentId = :pvaComponentId"), @NamedQuery(name = "ParamValue.findByPvaRunNumber", query = "SELECT p FROM ParamValue p WHERE p.pvaRunNumber = :pvaRunNumber"), @NamedQuery(name = "ParamValue.findByPvaStringValue", query = "SELECT p FROM ParamValue p WHERE p.pvaStringValue = :pvaStringValue"), @NamedQuery(name = "ParamValue.findByPvaNumericValue", query = "SELECT p FROM ParamValue p WHERE p.pvaNumericValue = :pvaNumericValue"), @NamedQuery(name = "ParamValue.findByPvaBooleanValue", query = "SELECT p FROM ParamValue p WHERE p.pvaBooleanValue = :pvaBooleanValue"), @NamedQuery(name = "ParamValue.findByPvaLowerThreshold", query = "SELECT p FROM ParamValue p WHERE p.pvaLowerThreshold = :pvaLowerThreshold"), @NamedQuery(name = "ParamValue.findByPvaUpperThreshold", query = "SELECT p FROM ParamValue p WHERE p.pvaUpperThreshold = :pvaUpperThreshold"), @NamedQuery(name = "ParamValue.findByPvaFraction", query = "SELECT p FROM ParamValue p WHERE p.pvaFraction = :pvaFraction")})
public class ParamValue implements Serializable {
    private static final long serialVersionUID = 1L;
    
    @Id
    @Basic(optional = false)
    @SequenceGenerator(name="PVA_ID_GEN", sequenceName="CDW_PVA_ID_SEQ", allocationSize=1)
    @GeneratedValue(strategy=GenerationType.SEQUENCE, generator="PVA_ID_GEN")
    @Column(name = "PVA_ID")
    private BigDecimal pvaId;
    @Basic(optional = false)
    @Column(name = "PVA_SOURCE_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date pvaSourceTime;
    @Basic(optional = false)
    @Column(name = "PVA_CREATE_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date pvaCreateTime;
    @Basic(optional = false)
    @Column(name = "PVA_COMPONENT_ID")
    private String pvaComponentId;
    @Column(name = "PVA_RUN_NUMBER")
    private Long pvaRunNumber;
    @Column(name = "PVA_STRING_VALUE")
    private String pvaStringValue;
    @Column(name = "PVA_NUMERIC_VALUE")
    private BigDecimal pvaNumericValue;
    @Basic
    @Column(name = "PVA_BOOLEAN_VALUE")
    private Boolean pvaBooleanValue;
    @Column(name = "PVA_LOWER_THRESHOLD")
    private BigDecimal pvaLowerThreshold;
    @Column(name = "PVA_UPPER_THRESHOLD")
    private BigDecimal pvaUpperThreshold;
    @Column(name = "PVA_FRACTION")
    private BigDecimal pvaFraction;
    @Lob
    @Column(name = "PVA_DESCR")
    private String pvaDescr;
    @JoinColumn(name = "PVA_BUN_ID", referencedColumnName = "BUN_ID")
    @ManyToOne(fetch = FetchType.LAZY)
    private BaseUnit pvaBunId;
    @JoinColumn(name = "PVA_PAR_ID", referencedColumnName = "PAR_ID")
    @ManyToOne(optional = false, fetch = FetchType.LAZY)
    private Parameter pvaParId;
    @JoinColumn(name = "PVA_SEV_ID", referencedColumnName = "SEV_ID")
    @ManyToOne(fetch = FetchType.LAZY)
    private Severity pvaSevId;
    @JoinColumn(name = "PVA_SRC_ID", referencedColumnName = "SRC_ID")
    @ManyToOne(optional = false, fetch = FetchType.LAZY)
    private Source pvaSrcId;

    public ParamValue() {
    }

    public ParamValue(BigDecimal pvaId) {
        this.pvaId = pvaId;
    }

    public ParamValue(BigDecimal pvaId, Date pvaSourceTime, Date pvaCreateTime, String pvaComponentId) {
        this.pvaId = pvaId;
        this.pvaSourceTime = pvaSourceTime;
        this.pvaCreateTime = pvaCreateTime;
        this.pvaComponentId = pvaComponentId;
    }

    public BigDecimal getPvaId() {
        return pvaId;
    }

    public void setPvaId(BigDecimal pvaId) {
        this.pvaId = pvaId;
    }

    public Date getPvaSourceTime() {
        return pvaSourceTime;
    }

    public void setPvaSourceTime(Date pvaSourceTime) {
        this.pvaSourceTime = pvaSourceTime;
    }

    public Date getPvaCreateTime() {
        return pvaCreateTime;
    }

    public void setPvaCreateTime(Date pvaCreateTime) {
        this.pvaCreateTime = pvaCreateTime;
    }

    public String getPvaComponentId() {
        return pvaComponentId;
    }

    public void setPvaComponentId(String pvaComponentId) {
        this.pvaComponentId = pvaComponentId;
    }

    public Long getPvaRunNumber() {
        return pvaRunNumber;
    }

    public void setPvaRunNumber(Long pvaRunNumber) {
        this.pvaRunNumber = pvaRunNumber;
    }

    public String getPvaStringValue() {
        return pvaStringValue;
    }

    public void setPvaStringValue(String pvaStringValue) {
        this.pvaStringValue = pvaStringValue;
    }

    public BigDecimal getPvaNumericValue() {
        return pvaNumericValue;
    }

    public void setPvaNumericValue(BigDecimal pvaNumericValue) {
        this.pvaNumericValue = pvaNumericValue;
    }

    public Boolean getPvaBooleanValue() {
        return pvaBooleanValue;
    }

    public void setPvaBooleanValue(Boolean pvaBooleanValue) {
        this.pvaBooleanValue = pvaBooleanValue;
    }

    public BigDecimal getPvaLowerThreshold() {
        return pvaLowerThreshold;
    }

    public void setPvaLowerThreshold(BigDecimal pvaLowerThreshold) {
        this.pvaLowerThreshold = pvaLowerThreshold;
    }

    public BigDecimal getPvaUpperThreshold() {
        return pvaUpperThreshold;
    }

    public void setPvaUpperThreshold(BigDecimal pvaUpperThreshold) {
        this.pvaUpperThreshold = pvaUpperThreshold;
    }

    public BigDecimal getPvaFraction() {
        return pvaFraction;
    }

    public void setPvaFraction(BigDecimal pvaFraction) {
        this.pvaFraction = pvaFraction;
    }

    public String getPvaDescr() {
        return pvaDescr;
    }

    public void setPvaDescr(String pvaDescr) {
        this.pvaDescr = pvaDescr;
    }

    public BaseUnit getPvaBunId() {
        return pvaBunId;
    }

    public void setPvaBunId(BaseUnit pvaBunId) {
        this.pvaBunId = pvaBunId;
    }

    public Parameter getPvaParId() {
        return pvaParId;
    }

    public void setPvaParId(Parameter pvaParId) {
        this.pvaParId = pvaParId;
    }

    public Severity getPvaSevId() {
        return pvaSevId;
    }

    public void setPvaSevId(Severity pvaSevId) {
        this.pvaSevId = pvaSevId;
    }

    public Source getPvaSrcId() {
        return pvaSrcId;
    }

    public void setPvaSrcId(Source pvaSrcId) {
        this.pvaSrcId = pvaSrcId;
    }

    @Override
    public int hashCode() {
        int hash = 0;
        hash += (pvaId != null ? pvaId.hashCode() : 0);
        return hash;
    }

    @Override
    public boolean equals(Object object) {
        // TODO: Warning - this method won't work in the case the id fields are not set
        if (!(object instanceof ParamValue)) {
            return false;
        }
        ParamValue other = (ParamValue) object;
        if ((this.pvaId == null && other.pvaId != null) || (this.pvaId != null && !this.pvaId.equals(other.pvaId))) {
            return false;
        }
        return true;
    }

    @Override
    public String toString() {
        return "org.cern.cms.csc.cdw.model.ParamValue[pvaId=" + pvaId + "]";
    }

}
