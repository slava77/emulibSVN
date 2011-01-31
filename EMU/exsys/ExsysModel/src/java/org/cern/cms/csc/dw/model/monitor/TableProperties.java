package org.cern.cms.csc.dw.model.monitor;

import java.io.Serializable;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Lob;
import javax.persistence.Table;

@Entity
@Table(name = "DWM_TABLE_PROPERTIES")
public class TableProperties implements Serializable {

    private String name;
    private String properties;

    public TableProperties() { }

    @Id
    @Basic
    @Column(name = "TPR_NAME")
    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    @Basic
    @Column(name = "TPR_PROPERTIES")
    @Lob
    public String getProperties() {
        return properties;
    }

    public void setProperties(String properties) {
        this.properties = properties;
    }

}
