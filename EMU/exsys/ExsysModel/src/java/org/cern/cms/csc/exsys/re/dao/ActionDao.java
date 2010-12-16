/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.EJB;
import javax.ejb.Stateless;
import javax.persistence.EntityManager;
import javax.persistence.PersistenceContext;
import org.cern.cms.csc.exsys.re.model.Action;

/**
 *
 * @author evka
 */
@Stateless
public class ActionDao implements ActionDaoLocal {
    
    @PersistenceContext(unitName="CdwPU")
    private EntityManager em;

    @SuppressWarnings("unchecked")
    public List<Action> getAllActions() {
        List<Action> actions = em.createQuery("select a from org.cern.cms.csc.exsys.re.model.Action as a " +
                                                      "order by a.name").getResultList();
        for (Action a: actions) {
            a.getConclusionTypes().iterator();
        }
        return actions;
    }

}
