/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.BaseUnit;

/**
 *
 * @author valdo
 */
@Local
public interface BaseUnitFacadeLocal {

    void create(BaseUnit baseUnit);

    void edit(BaseUnit baseUnit);

    void remove(BaseUnit baseUnit);

    BaseUnit find(Object id);

    List<BaseUnit> findAll();

}
