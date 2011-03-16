package org.cern.cms.csc.dw.model.monitor;

import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;

@Entity
@Table(name = "DWM_DATABASE_STATUS")
public class MonitorDatabaseStatus extends MonitorEntity {

    private Boolean alive;

    @Basic
    @Column(name = "MON_ALIVE", nullable=false)
    public Boolean getAlive() {
        return alive;
    }

    public void setAlive(Boolean alive) {
        this.alive = alive;
    }

}
