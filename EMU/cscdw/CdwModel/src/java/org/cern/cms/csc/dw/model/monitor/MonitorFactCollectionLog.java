package org.cern.cms.csc.dw.model.monitor;

import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;
import org.cern.cms.csc.dw.model.fact.FactCollection;

@Entity
@Table(name = "DWM_FACT_COLLECTION_LOG")
public class MonitorFactCollectionLog extends MonitorObject {

    private String provider;
    private Integer numberOfFacts;

    public MonitorFactCollectionLog() {
    }

    public MonitorFactCollectionLog(FactCollection fc) {
        setProvider(fc.getSource());
        setNumberOfFacts(fc.getFacts().size());
    }

    @Basic
    @Column(name = "MON_PROVIDER")
    public String getProvider() {
        return provider;
    }

    public void setProvider(String provider) {
        this.provider = provider;
    }

    @Basic
    @Column(name = "MON_NUM_OF_FACTS", nullable=false)
    public Integer getNumberOfFacts() {
        return numberOfFacts;
    }

    public void setNumberOfFacts(Integer numberOfFacts) {
        this.numberOfFacts = numberOfFacts;
    }

}
