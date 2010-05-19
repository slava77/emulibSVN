package org.cern.cms.csc.dw.metadata;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;
import javax.persistence.Basic;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.Id;
import javax.persistence.Table;
import javax.persistence.Transient;
import org.cern.cms.csc.dw.model.annotation.FactAnn;
import org.cern.cms.csc.dw.model.base.EntityBase;
import org.cern.cms.csc.dw.model.fact.Fact;
import org.cern.cms.csc.dw.model.ontology.ComponentClassType;

@Entity
@Table(name = "CDW_FACT_MD")
public class FactMd extends EntityBase implements Serializable {

    private Class<?extends Fact> factClass;
    private String tableName;
    private Integer daysToStore;
    private List<ComponentClassType> limitComponents = new ArrayList<ComponentClassType>();
    private boolean limitComponentsRecursive;

    public FactMd() {
    }

    public FactMd(Class<? extends Fact> factClass) {
        this.factClass = factClass;
        Table t = factClass.getAnnotation(Table.class);
        this.tableName = t.name();
        FactAnn ann = factClass.getAnnotation(FactAnn.class);
        this.daysToStore = ann.daysToStore();
        this.limitComponentsRecursive = ann.limitComponentsRecursive();
        for (String componentClassTypeName: ann.limitComponents()) {
            limitComponents.add(ComponentClassType.fromValue(componentClassTypeName));
        }
    }

    @Transient
    public Class<?extends Fact> getFactClass() {
        return factClass;
    }

    @Id
    @Column(name="FMD_TABLE_NAME")
    public String getTableName() {
        return tableName;
    }

    public void setTableName(String tableName) {
        this.tableName = tableName;
    }

    @Basic
    @Column(name="FMD_STORE_DAYS")
    public Integer getDaysToStore() {
        return daysToStore;
    }

    public void setDaysToStore(Integer daysToStore) {
        this.daysToStore = daysToStore;
    }

    /**
     * TODO: implement recursive check!
     * @param type
     * @return
     */
    public boolean checkComponentClassType(ComponentClassType type) {
        if (!limitComponentsRecursive) {
            return limitComponents.contains(type);
        }
        return true;
    }

}
