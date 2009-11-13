/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.ParamValue;

/**
 *
 * @author valdo
 */
@Local
public interface ParamValueFacadeLocal {

    void create(ParamValue paramValue);

    void edit(ParamValue paramValue);

    void remove(ParamValue paramValue);

    ParamValue find(Object id);

    List<ParamValue> findAll();

}
