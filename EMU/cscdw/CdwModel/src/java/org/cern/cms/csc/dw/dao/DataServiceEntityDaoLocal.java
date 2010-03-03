/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.dw.dao;

import javax.ejb.Local;
import org.cern.cms.csc.dw.model.dse.DataServiceEntity;

/**
 *
 * @author Evka
 */
@Local
public interface DataServiceEntityDaoLocal {

    void saveDataServiceEntity(DataServiceEntity dse) throws Exception;

}
