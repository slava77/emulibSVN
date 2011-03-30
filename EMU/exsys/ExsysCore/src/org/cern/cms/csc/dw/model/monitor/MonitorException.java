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
public class MonitorException extends MonitorEntity {

    private String message;
    private String className;
    private List<MonitorExceptionStackTrace> stackTrace;

    public MonitorException() { }

    public MonitorException(Class clazz, Throwable th) {
        this(clazz, th.getMessage(), th);
    }

    public MonitorException(Class clazz, String message, Throwable th) {
        this.className = clazz.getName();
        this.message = message;
        if (th != null) {
            this.stackTrace = new ArrayList<MonitorExceptionStackTrace>();
            for (int i = 0; i < th.getStackTrace().length; i++) {
                this.stackTrace.add(
                    new MonitorExceptionStackTrace(i, th.getStackTrace()[i])
                );
            }
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

    @Basic
    @Column(name = "MON_CLASSNAME", length=255, nullable=true)
    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
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
