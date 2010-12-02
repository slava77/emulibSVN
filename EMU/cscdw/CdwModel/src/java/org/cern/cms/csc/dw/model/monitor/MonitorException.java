package org.cern.cms.csc.dw.model.monitor;

import java.util.ArrayList;
import java.util.List;
import javax.persistence.Basic;
import javax.persistence.CascadeType;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.JoinColumn;
import javax.persistence.OneToMany;
import javax.persistence.Table;

@Entity
@Table(name = "DWM_EXCEPTIONS")
public class MonitorException extends MonitorObject {

    private String message;
    private List<MonitorExceptionStackTrace> stackTrace;

    public MonitorException() { }

    public MonitorException(Exception ex) {
        this.setMessage(ex.getMessage());
        this.stackTrace = new ArrayList<MonitorExceptionStackTrace>();
        for (int i = 0; i < ex.getStackTrace().length; i++) {
            this.stackTrace.add(
                new MonitorExceptionStackTrace(i, ex.getStackTrace()[i])
            );
        }
    }

    @Basic
    @Column(name = "MON_MESSAGE", length=32672, nullable=true)
    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    @OneToMany(targetEntity = MonitorExceptionStackTrace.class, cascade = { CascadeType.ALL })
    @JoinColumn(name = "EST_QST_ID")
    public List<MonitorExceptionStackTrace> getStackTrace() {
        return stackTrace;
    }

    public void setStackTrace(List<MonitorExceptionStackTrace> stackTrace) {
        this.stackTrace = stackTrace;
    }

}
