/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.Unit;

/**
 *
 * @author valdo
 */
@Local
public interface UnitFacadeLocal {

    void create(Unit unit);

    void edit(Unit unit);

    void remove(Unit unit);

    Unit find(Object id);

    List<Unit> findAll();

}
