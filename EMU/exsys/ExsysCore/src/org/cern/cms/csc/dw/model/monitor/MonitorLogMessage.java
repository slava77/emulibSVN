/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.model.monitor;

import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Table;

/**
 *
 * @author evka
 */
@Entity
@Table(name = "DWM_LOG_ENTRIES")
public class MonitorLogMessage extends MonitorEntity {

    private static final int MAX_ENTRY_SIZE = 32672;

    private String className;
    private String message;

    public MonitorLogMessage() {}

    public MonitorLogMessage(Class clazz, String message) {
        this.className = clazz.getCanonicalName();
        this.message = message;
    }

    @Basic
    @Column(name = "MON_MESSAGE", length=MAX_ENTRY_SIZE, nullable=false)
    public String getMessage() {
        return message;
    }

    @Basic
    @Column(name = "MON_CLASSNAME", length=1024, nullable=false)
    public String getClazzName() {
        return className;
    }
    
    public void setMessage(String message) {
        if (message.length() > MAX_ENTRY_SIZE) {
            message = message.substring(0, MAX_ENTRY_SIZE - 4) + "...";
        }
        this.message = message;
    }

    public void setClazzName(String className) {
        this.className = className;
    }

}
