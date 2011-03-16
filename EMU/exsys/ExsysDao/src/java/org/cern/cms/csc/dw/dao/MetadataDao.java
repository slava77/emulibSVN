package org.cern.cms.csc.dw.dao;

import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.dw.metadata.FactMd;

@Stateless
public class MetadataDao {

    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    public FactMd getFactMd(String tableName) {
        return em.find(FactMd.class, tableName);
    }

}
