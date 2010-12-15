/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.exsys.re.dao;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.exsys.re.model.Action;

/**
 *
 * @author evka
 */
@Local
public interface ActionDaoLocal {

    List<Action> getAllActions();

}
