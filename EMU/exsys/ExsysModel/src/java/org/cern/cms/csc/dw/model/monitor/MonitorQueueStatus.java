package org.cern.cms.csc.dw.model.monitor;

import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;

@Entity
@Table(name = "DWM_QUEUE_STATUS")
public class MonitorQueueStatus extends MonitorEntity {

    private String queueName;
    private Integer queueSize;

    @Basic
    @Column(name = "MON_QUEUE_NAME", nullable=false)
    public String getQueueName() {
        return queueName;
    }

    public void setQueueName(String queueName) {
        this.queueName = queueName;
    }

    @Basic
    @Column(name = "MON_QUEUE_SIZE", nullable=false)
    public Integer getQueueSize() {
        return queueSize;
    }

    public void setQueueSize(Integer queueSize) {
        this.queueSize = queueSize;
    }

}
