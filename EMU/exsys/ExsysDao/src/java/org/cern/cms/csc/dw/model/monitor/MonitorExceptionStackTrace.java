package org.cern.cms.csc.dw.model.monitor;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.Table;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.hibernate.annotations.Index;

@Entity
@Table(name = "DWM_EXCEPTION_STACK_TRACE")
public class MonitorExceptionStackTrace extends EntityBase implements Serializable {

    private Long id;
    private Integer number;
    private String className;
    private String fileName;
    private String methodName;
    private Integer lineNumber;

    public MonitorExceptionStackTrace() { }

    public MonitorExceptionStackTrace(Integer number, StackTraceElement ste) {
        this.setNumber(number);
        this.setClassName(ste.getClassName());
        this.setFileName(ste.getFileName());
        this.setMethodName(ste.getMethodName());
        this.setLineNumber(ste.getLineNumber());
    }

    @Id
    @Column(name = "EST_ID")
    @GeneratedValue(strategy=GenerationType.AUTO)
    public Long getId() {
        return id;
    }

    public void setId(Long id) {
        this.id = id;
    }

    @Basic
    @Column(name = "EST_NUMBER", nullable=false)
    @Index(name="DWM_EST_NUMBER_IDX")
    public Integer getNumber() {
        return number;
    }

    public void setNumber(Integer number) {
        this.number = number;
    }

    @Basic
    @Column(name = "EST_CLASS_NAME")
    @Index(name="DWM_EST_CLASS_NAME_IDX")
    public String getClassName() {
        return className;
    }

    public void setClassName(String className) {
        this.className = className;
    }

    @Basic
    @Column(name = "EST_FILE_NAME")
    public String getFileName() {
        return fileName;
    }

    public void setFileName(String fileName) {
        this.fileName = fileName;
    }

    @Basic
    @Column(name = "EST_METHOD_NAME")
    public String getMethodName() {
        return methodName;
    }

    public void setMethodName(String methodName) {
        this.methodName = methodName;
    }

    @Basic
    @Column(name = "EST_LINE_NUMBER")
    public Integer getLineNumber() {
        return lineNumber;
    }

    public void setLineNumber(Integer lineNumber) {
        this.lineNumber = lineNumber;
    }

}
