/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package org.cern.cms.csc.cdw.facade;

import java.util.List;
import javax.ejb.Local;
import org.cern.cms.csc.cdw.model.Parameter;

/**
 *
 * @author valdo
 */
@Local
public interface ParameterFacadeLocal {

    void create(Parameter parameter);

    void edit(Parameter parameter);

    void remove(Parameter parameter);

    Parameter find(Object id);

    List<Parameter> findAll();

}
