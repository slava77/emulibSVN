package org.cern.cms.csc.dw.metadata;

import java.io.Serializable;
import java.util.LinkedHashSet;
import java.util.Set;
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
    private String title;
    private Table table;
    private Integer daysToStore;
    private Set<ComponentClassType> limitComponents = new LinkedHashSet<ComponentClassType>();
    private boolean limitComponentsRecursive = false;

    public FactMd() {
    }

    public FactMd(Class<? extends Fact> factClass) {
        this.factClass = factClass;
        this.table = factClass.getAnnotation(Table.class);
        FactAnn ann = factClass.getAnnotation(FactAnn.class);
        this.title = ann.title();
        this.daysToStore = ann.daysToStore();
        for (String componentClassTypeName: ann.limitComponents()) {
            limitComponents.add(ComponentClassType.fromValue(componentClassTypeName));
        }
        this.limitComponentsRecursive = ann.limitComponentsRecursive();
    }

    @Transient
    public String getTitle() {
        return title;
    }

    @Transient
    public Class<?extends Fact> getFactClass() {
        return factClass;
    }

    @Id
    @Column(name="FMD_TABLE_NAME")
    public String getTableName() {
        return table.name();
    }

    public void setTableName(String tableName) {
    }

    @Basic
    @Column(name="FMD_STORE_DAYS")
    public Integer getDaysToStore() {
        return daysToStore;
    }

    public void setDaysToStore(Integer daysToStore) {
        this.daysToStore = daysToStore;
    }

    @Transient
    public Set<ComponentClassType> getLimitComponents() {
        return limitComponents;
    }

    @Transient
    public boolean isLimitComponentsRecursive() {
        return limitComponentsRecursive;
    }

    @Transient
    public Table getTableAnn() {
        return table;
    }

}
